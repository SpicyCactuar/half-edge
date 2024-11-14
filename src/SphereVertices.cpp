#include "SphereVertices.h"

#include <array>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

constexpr std::array<std::array<float, 3>, 84> sphereVert{
    {
        {0, 0, -1},
        {0.5, 0, -0.86603},
        {0.86603, 0, -0.5},
        {1, 0, 0},
        {0.86603, 0, 0.5},
        {0.5, 0, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {0.43301, 0.25, -0.86603},
        {0.75, 0.43301, -0.5},
        {0.86603, 0.5, 0},
        {0.75, 0.43301, 0.5},
        {0.43301, 0.25, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {0.25, 0.43301, -0.86603},
        {0.43301, 0.75, -0.5},
        {0.5, 0.86603, 0},
        {0.43301, 0.75, 0.5},
        {0.25, 0.43301, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {0, 0.5, -0.86603},
        {0, 0.86603, -0.5},
        {0, 1, 0},
        {0, 0.86603, 0.5},
        {0, 0.5, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {-0.25, 0.43301, -0.86603},
        {-0.43301, 0.75, -0.5},
        {-0.5, 0.86603, 0},
        {-0.43301, 0.75, 0.5},
        {-0.25, 0.43301, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {-0.43301, 0.25, -0.86603},
        {-0.75, 0.43301, -0.5},
        {-0.86603, 0.5, 0},
        {-0.75, 0.43301, 0.5},
        {-0.43301, 0.25, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {-0.5, 0, -0.86603},
        {-0.86603, 0, -0.5},
        {-1, 0, 0},
        {-0.86603, 0, 0.5},
        {-0.5, 0, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {-0.43301, -0.25, -0.86603},
        {-0.75, -0.43301, -0.5},
        {-0.86603, -0.5, 0},
        {-0.75, -0.43301, 0.5},
        {-0.43301, -0.25, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {-0.25, -0.43301, -0.86603},
        {-0.43301, -0.75, -0.5},
        {-0.5, -0.86603, 0},
        {-0.43301, -0.75, 0.5},
        {-0.25, -0.43301, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {0, -0.5, -0.86603},
        {0, -0.86603, -0.5},
        {0, -1, 0},
        {0, -0.86603, 0.5},
        {0, -0.5, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {0.25, -0.43301, -0.86603},
        {0.43301, -0.75, -0.5},
        {0.5, -0.86603, 0},
        {0.43301, -0.75, 0.5},
        {0.25, -0.43301, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {0.43301, -0.25, -0.86603},
        {0.75, -0.43301, -0.5},
        {0.86603, -0.5, 0},
        {0.75, -0.43301, 0.5},
        {0.43301, -0.25, 0.86603},
        {0, 0, 1}
    }
};

void renderWireframeSphereOutline() {
    // draw a circle around the edge of the sphere
    glBegin(GL_LINE_LOOP);
    for (int j = 3; j < sphereVert.size(); j += 7) {
        glVertex3f(sphereVert[j][0], sphereVert[j][1], 0.5);
    }
    glEnd();
}

void renderWireframeSphere() {
    // loop through verticals of sphere
    for (int i = 0; i < 12; i++) {
        // loop drawing verticals
        glBegin(GL_LINE_STRIP);
        for (int j = i * 7; j < 7 + i * 7; j++) {
            glVertex3fv(sphereVert[j].data());
        }
        glEnd();
    }
    // loop through horizontals
    for (int i = 1; i < 6; i++) {
        // loop for horizontals
        glBegin(GL_LINE_LOOP);
        for (int j = i; j < sphereVert.size(); j += 7) {
            glVertex3fv(sphereVert[j].data());
        }
        glEnd();
    }
}

void renderTriangulatedSphere() {
    glBegin(GL_TRIANGLES);

    // start with the triangles at the N Pole
    for (int segment = 0; segment < 12; segment++) {
        const unsigned int nextSegment = (segment + 1) % 12;

        glVertex3fv(sphereVert[7 * segment + 0].data());
        glVertex3fv(sphereVert[7 * segment + 1].data());
        glVertex3fv(sphereVert[7 * nextSegment + 1].data());

        // loop down the next few, drawing quads
        for (unsigned int layer = 1; layer < 6; layer++) {
            glVertex3fv(sphereVert[7 * segment + layer].data());
            glVertex3fv(sphereVert[7 * segment + layer + 1].data());
            glVertex3fv(sphereVert[7 * nextSegment + layer + 1].data());
            glVertex3fv(sphereVert[7 * segment + layer].data());
            glVertex3fv(sphereVert[7 * nextSegment + layer + 1].data());
            glVertex3fv(sphereVert[7 * nextSegment + layer].data());
        }

        // triangle at the bottom
        glVertex3fv(sphereVert[7 * segment + 5].data());
        glVertex3fv(sphereVert[7 * segment + 6].data());
        glVertex3fv(sphereVert[7 * nextSegment + 5].data());
    }

    glEnd();
}
