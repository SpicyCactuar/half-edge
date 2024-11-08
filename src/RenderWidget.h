#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <QOpenGLWidget>
#include <QMouseEvent>

#include "TriangleMesh.h"
#include "RenderParameters.h"

// class for a render widget with arcball linked to an external arcball widget
class RenderWidget : public QOpenGLWidget {
    Q_OBJECT

    RenderParameters* renderParameters;

public:
    RenderWidget(
        TriangleMesh* triangleMesh,
        RenderParameters* renderParameters,
        QWidget* parent
    );

protected:
    void initializeGL();

    void resizeGL(int width, int height);

    void paintGL();

public:
    // the geometric object to be rendered
    TriangleMesh* triangleMesh;

    void render() const;

    void renderMesh(const TriangleMesh& mesh) const;


signals:
    void mouseDown(int whichButton, int x, int y);

    void mouseDrag(int x, int y);

    void mouseUp(int x, int y);
};

#endif
