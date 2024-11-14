#ifndef ARCBALL_WIDGET_H
#define ARCBALL_WIDGET_H

#include <QOpenGLWidget>
#include <QMouseEvent>

#include "ArcBall.h"
#include "Matrix4.h"

class ArcBallWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    ArcBall arcBall;

    ArcBallWidget(QWidget* parent);

    Matrix4 rotationMatrix() const;

protected:
    void initializeGL();

    void resizeGL(int w, int h);

    void paintGL();

    virtual void mousePressEvent(QMouseEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);

    virtual void mouseReleaseEvent(QMouseEvent* event);

public slots:
    // routines called to allow synchronised rotation with other widget
    // coordinates are assumed to be in range of [-1..1] in x,y
    // if outside that range, will be clamped
    void beginDrag(float x, float y);

    void continueDrag(float x, float y);

    void endDrag(float x, float y);

signals:
    void rotationChanged();
};

#endif
