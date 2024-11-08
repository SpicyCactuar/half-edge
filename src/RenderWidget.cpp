#include "RenderWidget.h"

#include <cmath>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "SphereVertices.h"

RenderWidget::RenderWidget(
    TriangleMesh* triangleMesh,
    RenderParameters* renderParameters,
    QWidget* parent
) : QOpenGLWidget(parent),
    renderParameters(renderParameters),
    triangleMesh(triangleMesh) {
}

void RenderWidget::initializeGL() {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
}

void RenderWidget::resizeGL(const int width, const int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    // we want to capture a sphere of radius 1.0 without distortion
    // so we set the ortho projection based on whether the window is portrait (> 1.0) or landscape
    if (aspectRatio > 1.0) {
        // portrait ratio is wider, so make bottom & top -1.0 & 1.0
        glOrtho(-aspectRatio, aspectRatio, -1.0, 1.0, -1.1, 1.1);
    } else {
        // otherwise, make left & right -1.0 & 1.0
        glOrtho(-1.0, 1.0, -1.0 / aspectRatio, 1.0 / aspectRatio, -1.1, 1.1);
    }
}

void RenderWidget::paintGL() {
    render();
}

void RenderWidget::render() const {
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.8, 0.8, 0.6, 1.0);
    glEnable(GL_LIGHTING);

    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set model view matrix based on stored translation, rotation &c.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // set light position first, pushing/popping matrix so that it the transformation does
    // not affect the position of the geometric object
    glPushMatrix();
    glMultMatrixf(reinterpret_cast<GLfloat*>(renderParameters->lightMatrix.columnMajor().coordinates));
    glLightfv(GL_LIGHT0, GL_POSITION, renderParameters->lightPosition.data());
    glPopMatrix();

    // translate by the visual translation
    glTranslatef(renderParameters->xTranslate, renderParameters->yTranslate, 0.0f);

    // apply rotation matrix from arcball
    glMultMatrixf(reinterpret_cast<GLfloat*>(renderParameters->rotationMatrix.columnMajor().coordinates));

    // tell the object to draw itself,
    // passing in the render parameters for reference
    renderMesh(*triangleMesh);
}

void RenderWidget::renderMesh(const TriangleMesh& mesh) const {
    // Render()
    // Ideally, we would apply a global transformation to the object, but sadly that breaks down
    // when we want to scale things, as unless we normalise the normal vectors, we end up affecting
    // the illumination.  Known solutions include:
    // 1.   Normalising the normal vectors
    // 2.   Explicitly dividing the normal vectors by the scale to balance
    // 3.   Scaling only the vertex position (slower, but safer)
    // 4.   Not allowing spatial zoom (note: sniper scopes are a modified projection matrix)
    //
    // Inside a game engine, zoom usually doesn't apply. Normalisation of normal vectors is expensive,
    // so we will choose option 2.
    float scale = renderParameters->zoomScale;
    scale /= mesh.objectSize;
    glScalef(scale, scale, scale);

    const Cartesian3 centreOfGravity = mesh.centreOfGravity;
    glTranslatef(-centreOfGravity.x, -centreOfGravity.y, -centreOfGravity.z);

    // render triangles
    glBegin(GL_TRIANGLES);

    // set colour for pick render - ignored for regular render
    glColor3f(1.0, 1.0, 1.0);

    // loop through the faces
    for (unsigned int face = 0; face < mesh.faceVertices.size(); face += 3) {
        if (renderParameters->useFlatNormals) {
            const auto& p = mesh.vertices[mesh.faceVertices[face]];
            const auto& q = mesh.vertices[mesh.faceVertices[face + 1]];
            const auto& r = mesh.vertices[mesh.faceVertices[face + 2]];

            // Compute flat face normal
            Cartesian3 pq = q - p;
            Cartesian3 pr = r - p;
            const Cartesian3 faceNormal = pq.cross(pr).unit();

            glNormal3f(faceNormal.x * scale, faceNormal.y * scale, faceNormal.z * scale);
        }

        for (unsigned int vertex = face; vertex < face + 3; vertex++) {
            const auto faceVertex = mesh.faceVertices[vertex];
            if (!renderParameters->useFlatNormals) {
                // hard assumption: we have enough normals
                glNormal3f(
                    mesh.normals[faceVertex].x * scale,
                    mesh.normals[faceVertex].y * scale,
                    mesh.normals[faceVertex].z * scale
                );
            }

            glVertex3f(
                mesh.vertices[faceVertex].x,
                mesh.vertices[faceVertex].y,
                mesh.vertices[faceVertex].z
            );
        }
    }

    glEnd();

    if (!renderParameters->showVertices) {
        return;
    }

    glDisable(GL_LIGHTING);

    // loop through the vertices
    for (const auto& vertex : mesh.vertices) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(vertex.x, vertex.y, vertex.z);
        glScalef(0.1f * renderParameters->vertexSize,
                 0.1f * renderParameters->vertexSize,
                 0.1f * renderParameters->vertexSize);
        renderTriangulatedSphere();
        glPopMatrix();
    }
}
