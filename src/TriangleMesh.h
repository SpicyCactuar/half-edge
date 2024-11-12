#ifndef TRIANGLE_MESH
#define TRIANGLE_MESH

#include <functional>
#include <vector>
#include <iostream>
#include <optional>

#include "Cartesian3.h"

typedef unsigned int VertexId;
typedef unsigned int EdgeId;
typedef unsigned int FaceIndex;

/**
 * Describes a mesh with triangular faces. The half-edge data structure
 * serves as backing mechanism.
 *
 * The mesh can be inflated using a custom .hds file (samples are provided)
 * and .tri triangle soup files.
 */
class TriangleMesh {
public:
    std::vector<Cartesian3> vertices;
    std::vector<Cartesian3> normals;
    std::vector<VertexId> faceVertices;
    std::vector<EdgeId> firstDirectedEdge;
    std::vector<EdgeId> otherHalf;

    Cartesian3 centreOfGravity;

    // radius of circumscribing sphere centred at centre of gravity
    float objectSize;

    TriangleMesh();

    // create 1-level subdivision
    TriangleMesh subdivide() const;

    bool readHdsFile(std::istream& hdsFile);

    std::optional<EdgeId> findHalfEdgeFor(VertexId from, VertexId to) const;

    bool readTriFile(std::istream& triFile);

    void computeNormals();

    // Write to files
    void writeToHdsFile(std::ostream& hdsStream) const;

    void writeToObjFile(std::ostream& objStream) const;

private:
    void computeCentreOfGravity();

    // Transforms edgeId to the index for the edge [x -> edge[to]]
    static unsigned int idToIndex(EdgeId edgeId);

    // Computes the next halfEdge id within the face of edgeId
    static EdgeId nextIdInFace(EdgeId edgeId);

    // Returns <edge[from], edge[to]>
    std::pair<VertexId, VertexId> vertexIndicesOf(EdgeId edgeId) const;

    void visitNeighbourhoodOf(VertexId vertexId,
                              const std::function<void(EdgeId, VertexId, VertexId)>& visitor) const;

    Cartesian3 centroidLerp(VertexId vertexId) const;
};

class OtherHalfNotFound : public std::runtime_error {
public:
    OtherHalfNotFound(const EdgeId edgeId, const Cartesian3& from, const Cartesian3& to)
        : std::runtime_error(
            "OtherHalfNotFound:\n"
            "\tFailed to find half-edge\n"
            "\tedgeId = " + std::to_string(edgeId) + "\n"
            "\tfrom = " + from.toString() + "\n"
            "\tto = " + to.toString() + "\n"
            "This happens when a mesh is malformed. A mesh can be malformed due, but not limited, to:\n"
            "* The edge is disconnected -> Check whether the face of the reported edge is connected to another face in the .tri file\n"
            "* Windedness of the faces is inconsistent -> Check whether the adjacent faces have consistent windedness .tri file\n"
            "* Floating point precision issues -> Check whether from & to match values present in the .tri file") {
    }
};

#endif
