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
    TriangleMesh* triangleMesh;

    RenderWidget(
        TriangleMesh* triangleMesh,
        RenderParameters* renderParameters,
        QWidget* parent
    );

protected:
    void initializeGL();

    void resizeGL(int width, int height);

    void paintGL();

    void mousePressEvent(QMouseEvent* event);

    void mouseMoveEvent(QMouseEvent* event);

    void mouseReleaseEvent(QMouseEvent* event);

private:
    void renderMesh() const;

signals:
    // these are general purpose signals, which scale the drag to
    // the notional unit sphere and pass it to the controller for handling
    void beginScaledDrag(int whichButton, float x, float y);

    void continueScaledDrag(float x, float y);

    void endScaledDrag(float x, float y);
};

#endif
