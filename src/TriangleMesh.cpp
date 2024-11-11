#include "TriangleMesh.h"

#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <limits>
#include <unordered_map>

#define MAXIMUM_LINE_LENGTH 1024

constexpr unsigned int NO_VALUE = std::numeric_limits<unsigned int>::max();

constexpr float NEAR_NEIGHBOUR_WEIGHT = 0.375f; // 3 / 8
constexpr float FAR_NEIGHBOUR_WEIGHT = 0.125f; // 1 / 8

constexpr float N_3_ALPHA = 0.1875f; // 3 / 16

TriangleMesh::TriangleMesh()
    : centreOfGravity(0.0f, 0.0f, 0.0f),
      objectSize(0.0f) {
    vertices.clear();
    normals.clear();
    firstDirectedEdge.clear();
    faceVertices.clear();
    otherHalf.clear();
}

/**
 * @brief Processes the half-edge structure as-is from an .hds file
 *
 * @param hdsFile .hds half-edge file
 *
 * @return whether the read was successful
 */
bool TriangleMesh::readHdsFile(std::istream& hdsFile) {
    // Read contents, discard bad lines
    while (!hdsFile.eof()) {
        char readBuffer[MAXIMUM_LINE_LENGTH];
        std::string token;
        hdsFile >> token;

        if (token == "#") {
            hdsFile.getline(readBuffer, MAXIMUM_LINE_LENGTH);
        } else if (token == "Vertex") {
            unsigned int vertexID;
            hdsFile >> vertexID;

            if (vertexID != vertices.size()) {
                hdsFile.getline(readBuffer, MAXIMUM_LINE_LENGTH);
            }

            Cartesian3 newVertex;
            hdsFile >> newVertex;

            vertices.push_back(newVertex);
        } else if (token == "Normal") {
            unsigned int normalId;
            hdsFile >> normalId;

            if (normalId != normals.size()) {
                hdsFile.getline(readBuffer, MAXIMUM_LINE_LENGTH);
            }

            Cartesian3 newNormal;
            hdsFile >> newNormal;

            normals.push_back(newNormal);
        } else if (token == "FirstDirectedEdge") {
            EdgeId fdeId;
            hdsFile >> fdeId;

            if (fdeId != firstDirectedEdge.size()) {
                hdsFile.getline(readBuffer, MAXIMUM_LINE_LENGTH);
            }

            unsigned int newFde;
            hdsFile >> newFde;

            firstDirectedEdge.push_back(newFde);
        } else if (token == "Face") {
            unsigned int faceId;
            hdsFile >> faceId;

            if (faceId != faceVertices.size() / 3) {
                hdsFile.getline(readBuffer, MAXIMUM_LINE_LENGTH);
            }

            VertexId newFaceVertex;
            hdsFile >> newFaceVertex;
            faceVertices.push_back(newFaceVertex);
            hdsFile >> newFaceVertex;
            faceVertices.push_back(newFaceVertex);
            hdsFile >> newFaceVertex;
            faceVertices.push_back(newFaceVertex);
        } else if (token == "OtherHalf") {
            unsigned int otherHalfID;
            hdsFile >> otherHalfID;

            if (otherHalfID != otherHalf.size()) {
                hdsFile.getline(readBuffer, MAXIMUM_LINE_LENGTH);
            }

            unsigned int newOtherHalf;
            hdsFile >> newOtherHalf;
            otherHalf.push_back(newOtherHalf);
        }
    }

    computeCentreOfGravity();

    return true;
}

/**
 * @return std::optional with the value EdgeId of the edge [to -> from] if it exists,
 *         otherwise std::nullopt (i.e. half-edge [to -> from] was not found)
 */
std::optional<EdgeId> TriangleMesh::findHalfEdgeFor(const VertexId from, const VertexId to) const {
    for (EdgeId edgeId = 0; edgeId < faceVertices.size(); edgeId++) {
        auto [otherFrom, otherTo] = vertexIndicesOf(edgeId);
        if (otherFrom == to && otherTo == from) {
            return {edgeId};
        }
    }

    return std::nullopt;
}

/**
 * @brief Computes the half-edge structure from a .tri file
 *
 * @param triFile .tri triangle soup file
 *
 * @return whether the read was successful
 */
bool TriangleMesh::readTriFile(std::istream& triFile) {
    unsigned int trianglesAmount;
    triFile >> trianglesAmount;

    // Since file is a triangle soup, totalVerticesAmount = T * 3, where T = #triangle
    const unsigned int totalVerticesAmount = trianglesAmount * 3;

    /*
     * For each vertex:
     *      - Process vertex value
     *      - Linear lookup to find vertexId in indexedVertices:
     *          -- If vertex is new (not found), vertexId = #vertices and store indexedVertices.push_back(vertex)
     *          -- Otherwise, vertexId = #index of vertex in indexedVertices
     *      - Current vertex is assumed to be the tail of an edge within a face, therefore:
     *          -- Store faceVertices[edgeId] = vertexId, where edgeId = #edges
     *             See TriangleMesh.faceVertices for a more detailed explanation
     */
    for (unsigned int v = 0; v < totalVerticesAmount; v++) {
        Cartesian3 vertex;
        triFile >> vertex;

        const auto vertexIdLookup = std::find(vertices.begin(), vertices.end(), vertex);
        const bool isVertexNew = vertexIdLookup == vertices.end();

        VertexId vertexId = isVertexNew ? vertices.size() : vertexIdLookup - vertices.begin();

        if (isVertexNew) {
            vertices.push_back(vertex);
        }

        faceVertices.push_back(vertexId);
    }

    /*
     * For each edge:
     *      - Set from = faceVertices[edge]
     *      - If FDE[from] already has a value, skip it
     *      - Otherwise, set FDE[from] = edge
     */
    firstDirectedEdge.resize(vertices.size(), NO_VALUE);
    for (EdgeId edgeId = 0; edgeId < faceVertices.size(); edgeId++) {
        const VertexId vertexIdFrom = faceVertices[idToIndex(edgeId)];

        // FDE has already been processed, skip
        if (firstDirectedEdge[vertexIdFrom] != NO_VALUE) {
            continue;
        }

        firstDirectedEdge[vertexIdFrom] = idToIndex(edgeId);
    }

    /*
     * For each edge, find and map its other half
     */
    otherHalf.resize(faceVertices.size(), NO_VALUE);
    for (EdgeId edgeId = 0; edgeId < faceVertices.size(); edgeId++) {
        auto [from, to] = vertexIndicesOf(edgeId);
        auto halfEdgeLookup = findHalfEdgeFor(from, to);

        if (!halfEdgeLookup.has_value()) {
            const auto& fromVertex = vertices[from];
            const auto& toVertex = vertices[to];
            throw OtherHalfNotFound(edgeId, fromVertex, toVertex);
        }

        const EdgeId halfEdge = halfEdgeLookup.value();

        otherHalf[edgeId] = halfEdge;
        otherHalf[halfEdge] = edgeId;
    }

    computeNormals();
    computeCentreOfGravity();

    return true;
}

/*
 * Based on: https://iquilezles.org/articles/normals/
 */
void TriangleMesh::computeNormals() {
    normals.resize(vertices.size(), {0.0f, 0.0f, 0.0f});

    for (size_t faceId = 0; faceId < faceVertices.size(); faceId += 3) {
        const VertexId pId = faceVertices[faceId];
        const VertexId qId = faceVertices[faceId + 1];
        const VertexId rId = faceVertices[faceId + 2];
        const auto& p = vertices[pId];
        const auto& q = vertices[qId];
        const auto& r = vertices[rId];

        // Accumulate cross product
        Cartesian3 pq = q - p;
        Cartesian3 pr = r - p;

        const Cartesian3 cross = pq.cross(pr);

        normals[pId] += cross;
        normals[qId] += cross;
        normals[rId] += cross;
    }

    // Normalise the accumulation
    for (auto& normal : normals) {
        normal = normal.unit();
    }
}

void TriangleMesh::computeCentreOfGravity() {
    // note that very large files may have numerical problems with this
    centreOfGravity = Cartesian3(0.0, 0.0, 0.0);

    // if there are no vertices, leave centre at (0.0, 0.0, 0.0)
    if (vertices.empty()) {
        return;
    }

    // sum up all vertex positions
    for (const auto& vertex : vertices) {
        centreOfGravity = centreOfGravity + vertex;
    }

    // and divide through by the number to get the average position
    // also known as the barycentre
    centreOfGravity = centreOfGravity / vertices.size();

    // start with 0 radius
    objectSize = 0.0;

    // now compute the largest distance from the origin to a vertex
    for (const auto& vertex : vertices) {
        // now test for maximality
        if (const float distance = (vertex - centreOfGravity).length();
            distance > objectSize) {
            objectSize = distance;
        }
    }
}

unsigned int TriangleMesh::idToIndex(const EdgeId edgeId) {
    return 3 * (edgeId / 3) + (3 + edgeId - 1) % 3;
}

EdgeId TriangleMesh::nextIdInFace(const EdgeId edgeId) {
    return 3 * (edgeId / 3) + (edgeId + 1) % 3;
}

std::pair<VertexId, VertexId> TriangleMesh::vertexIndicesOf(const EdgeId edgeId) const {
    const unsigned int fromIndex = idToIndex(edgeId);
    const unsigned int toIndex = idToIndex(nextIdInFace(edgeId));

    return {faceVertices[fromIndex], faceVertices[toIndex]};
}

/**
 * Returns a Loop Subdivision of the TriangleMesh.
 * Assumes that the surface is 2-manifold and the edges are in the format edge[to].
 */
TriangleMesh TriangleMesh::subdivide() const {
    TriangleMesh subdivision;

    // copy all old vertices to retain their indices
    subdivision.vertices.insert(subdivision.vertices.end(), vertices.begin(), vertices.end());

    // edgeId -> fulledgeId
    std::vector<unsigned int> fulledges(faceVertices.size(), NO_VALUE);
    // fulledgeId -> vertexId
    std::vector<unsigned int> fulledgeToEdgeVertex;
    // subdivisionVertexId -> <halfEdge, otherHalf>
    std::unordered_map<unsigned int, std::pair<unsigned int, unsigned int>> edgeVertexToHalves;

    for (EdgeId edgeId = 0; edgeId < faceVertices.size(); edgeId++) {
        if (fulledges[edgeId] == NO_VALUE) {
            unsigned int nextFulledgeIndex = fulledgeToEdgeVertex.size();
            // Assign fulledge to both half-edges
            fulledges[edgeId] = nextFulledgeIndex;
            fulledges[otherHalf[edgeId]] = nextFulledgeIndex;
            // avoid overlapping with existing vertices and associates vertex index to fulledge
            unsigned int subdivisionVertexId = vertices.size() + nextFulledgeIndex;
            fulledgeToEdgeVertex.push_back(subdivisionVertexId);
            edgeVertexToHalves[subdivisionVertexId] = {edgeId, otherHalf[edgeId]};
        }
    }

    // Compute subdivided faces
    std::vector<unsigned int> centralFaces;
    std::vector<unsigned int> adjacentFaces;
    for (unsigned int faceIndex = 0; faceIndex < faceVertices.size(); faceIndex += 3) {
        // Create vertex indices of central subdivided face
        VertexId vc0 = fulledgeToEdgeVertex[fulledges[faceIndex]];
        VertexId vc1 = fulledgeToEdgeVertex[fulledges[faceIndex + 1]];
        VertexId vc2 = fulledgeToEdgeVertex[fulledges[faceIndex + 2]];

        centralFaces.insert(centralFaces.end(), {vc0, vc1, vc2});

        // Create vertex indices of adjacent subdivided faces
        VertexId v0 = faceVertices[faceIndex];
        adjacentFaces.insert(adjacentFaces.end(), {v0, vc1, vc0});

        VertexId v1 = faceVertices[faceIndex + 1];
        adjacentFaces.insert(adjacentFaces.end(), {v1, vc2, vc1});

        VertexId v2 = faceVertices[faceIndex + 2];
        adjacentFaces.insert(adjacentFaces.end(), {v2, vc0, vc2});
    }
    // subdivision faces = central faces + adjacent faces
    // Guarantees that central faces come first, then adjacent faces
    subdivision.faceVertices.reserve(centralFaces.size() + adjacentFaces.size());
    subdivision.faceVertices.insert(subdivision.faceVertices.end(), centralFaces.begin(), centralFaces.end());
    subdivision.faceVertices.insert(subdivision.faceVertices.end(), adjacentFaces.begin(), adjacentFaces.end());

    // Compute subdivision otherHalf & firstDirectedEdge
    subdivision.otherHalf.resize(subdivision.faceVertices.size(), NO_VALUE);
    // #subdivision.vertices = #vertices + #fulledgeVertices
    subdivision.firstDirectedEdge.resize(vertices.size() + fulledgeToEdgeVertex.size(), NO_VALUE);
    for (EdgeId edgeId = 0; edgeId < subdivision.faceVertices.size(); edgeId++) {
        // Skip already assigned halfEdges
        if (subdivision.otherHalf[edgeId] != NO_VALUE) {
            continue;
        }

        // halfEdge = [from -> to]
        const auto [from, to] = subdivision.vertexIndicesOf(edgeId);

        // Set first directed edge, if not present
        if (subdivision.firstDirectedEdge[from] == NO_VALUE) {
            subdivision.firstDirectedEdge[from] = edgeId;
        }

        // Find otherHalfEdge = [to -> from]
        for (unsigned int otherEdgeId = 0; otherEdgeId < subdivision.faceVertices.size(); otherEdgeId++) {
            // Skip same edge
            if (otherEdgeId == edgeId) continue;

            const auto [otherFrom, otherTo] = subdivision.vertexIndicesOf(otherEdgeId);

            if (from == otherTo && to == otherFrom) {
                subdivision.otherHalf[edgeId] = otherEdgeId;
                subdivision.otherHalf[otherEdgeId] = edgeId;
                break;
            }
        }
    }

    // Compute new vertices spatial values (xyz)
    for (unsigned int edgeVertexId : fulledgeToEdgeVertex) {
        const auto [halfEdge, otherHalf] = edgeVertexToHalves[edgeVertexId];

        const auto [v2, v1] = vertexIndicesOf(halfEdge);
        const VertexId v3 = faceVertices[nextIdInFace(halfEdge)];
        const VertexId v4 = faceVertices[nextIdInFace(otherHalf)];

        Cartesian3 edgeVertex = NEAR_NEIGHBOUR_WEIGHT * (vertices[v1] + vertices[v2]) +
                                FAR_NEIGHBOUR_WEIGHT * (vertices[v3] + vertices[v4]);

        // Push new vertices at the end
        subdivision.vertices.push_back(edgeVertex);
    }

    // Compute old vertices in spatial values (xyz)
    for (unsigned int oldVertexId = 0; oldVertexId < vertices.size(); oldVertexId++) {
        // Reference this to make explicit that centroid calculation uses old neighbourhoods
        subdivision.vertices[oldVertexId] = this->centroidLerp(oldVertexId);
    }

    subdivision.computeCentreOfGravity();
    subdivision.computeNormals();
    return subdivision;
}

/**
 * @param vertexId of the vertex
 *
 * @return the (x, y, z) resulting of lerping b
 */
Cartesian3 TriangleMesh::centroidLerp(const VertexId vertexId) const {
    Cartesian3 neighbourhoudSum;
    unsigned int n = 0;

    visitNeighbourhoodOf(vertexId, [&](EdgeId, VertexId, const VertexId neighbour) {
        neighbourhoudSum = neighbourhoudSum + vertices[neighbour];
        n++;
    });

    float alpha;

    if (n == 3) {
        alpha = N_3_ALPHA;
    } else {
        alpha = (0.625f - std::pow(0.375f + 0.25f * std::cos(2.0f * M_PI / n), 2.0f)) / n;
    }

    return (1.0f - n * alpha) * vertices[vertexId] + alpha * neighbourhoudSum;
}

/** @brief visits the 1-ring neighbourhood of a vertexId, starting from FDE[vertexId]
 *
 * @param vertexId the target id of the vertex
 * @param visitor a lambda which, for each visited edge, is invoked with:
 *      - edgeId of the visited edge
 *      - vertexId of the tail of the visited edge (same value as vertexId, left for testing purposes)
 *      - vertexId of the head of the visited edge, will be a neighbour
 */
void TriangleMesh::visitNeighbourhoodOf(const VertexId vertexId,
                                        const std::function<void(EdgeId, VertexId, VertexId)>& visitor) const {
    if (vertexId >= vertices.size()) {
        std::cerr << "Attempting to visit neighborhoud of invalid vertexId = " << vertexId << std::endl;
        return;
    }

    unsigned int firstEdge = firstDirectedEdge[vertexId];
    unsigned int currentEdge = firstEdge;

    // Invariant: Tail of currentEdge is vertexId, head of currentEdge != vertexId
    //            The neighbourhood of vertexId is composed of the visited vertices
    do {
        auto [currentVertex, adjacentVertex] = vertexIndicesOf(currentEdge);
        visitor(currentEdge, currentVertex, adjacentVertex);
        // Obtain otherHalf[currentEdge] to move to half-edge in another face
        // Then, get the next in the face, which invariantly has vertexId as tail
        currentEdge = nextIdInFace(otherHalf[currentEdge]);
    } while (currentEdge != firstEdge);
}

void TriangleMesh::writeToHdsFile(std::ostream& hdsStream) const {
    hdsStream << "# Created by SpicyCactuar/half-edge\n"
            << "#\n"
            << "# Surface vertices=" << vertices.size()
            << " faces=" << faceVertices.size() / 3 << "\n#\n";

    for (unsigned int vertex = 0; vertex < vertices.size(); ++vertex) {
        hdsStream << "Vertex " << vertex << " " << std::fixed << vertices[vertex] << '\n';
    }

    for (unsigned int normal = 0; normal < normals.size(); ++normal) {
        hdsStream << "Normal " << normal << " " << std::fixed << normals[normal] << '\n';
    }

    for (unsigned int vertex = 0; vertex < firstDirectedEdge.size(); ++vertex) {
        hdsStream << "FirstDirectedEdge " << vertex << " " << std::fixed << firstDirectedEdge[vertex] << '\n';
    }

    for (unsigned int face = 0; face < faceVertices.size(); face += 3) {
        hdsStream << "Face " << face / 3 << " "
                << faceVertices[face] << " "
                << faceVertices[face + 1] << " "
                << faceVertices[face + 2] << '\n';
    }

    for (unsigned int dirEdge = 0; dirEdge < otherHalf.size(); ++dirEdge) {
        hdsStream << "OtherHalf " << dirEdge << " " << otherHalf[dirEdge] << '\n';
    }
}

void TriangleMesh::writeToObjFile(std::ostream& objStream) const {
    objStream << "# Created by SpicyCactuar/half-edge\n"
            << "#\n"
            << "# Surface vertices=" << vertices.size()
            << " faces=" << faceVertices.size() / 3 << "\n#\n";

    for (const auto& v : vertices) {
        objStream << "v " << v.x << ' ' << v.y << ' ' << v.z << std::endl;
    }

    for (const auto& vn : normals) {
        objStream << "vn " << vn.x << ' ' << vn.y << ' ' << vn.z << std::endl;
    }

    for (size_t i = 0; i < faceVertices.size(); i += 3) {
        // OBJ faces are 1-based, so we need to adjust the index by adding 1
        objStream << "f "
                << faceVertices[i] + 1 << "//" << faceVertices[i] + 1 << " "
                << faceVertices[i + 1] + 1 << "//" << faceVertices[i + 1] + 1 << " "
                << faceVertices[i + 2] + 1 << "//" << faceVertices[i + 2] + 1
                << std::endl;
    }
}
