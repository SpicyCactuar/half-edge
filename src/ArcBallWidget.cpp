#define ARCBALL_WIDGET_SIZE 100

#include "ArcBallWidget.h"
#include "SphereVertices.h"

ArcBallWidget::ArcBallWidget(QWidget* parent)
    : QOpenGLWidget(parent) {
    // let QT know we are fixed size
    setFixedSize(QSize(ARCBALL_WIDGET_SIZE, ARCBALL_WIDGET_SIZE));
}

Matrix4 ArcBallWidget::rotationMatrix() {
    return arcBall.getRotation();
}

void ArcBallWidget::initializeGL() {
    // no lighting, but we need depth test
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.9, 0.9, 0.9, 1.0);
}

void ArcBallWidget::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);
    // set projection matrix to have range of -1.0 - 1.0 in x, y, z
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

void ArcBallWidget::paintGL() {
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set model view matrix based on arcball rotation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw a single white quad to block back half of arcball
    glColor3f(0.92, 0.92, 0.92);
    glBegin(GL_QUADS);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
    glVertex3f(1.0, 1.0, 0.0);
    glVertex3f(-1.0, 1.0, 0.0);
    glEnd();

    // set colour to black
    glColor3f(0.0, 0.0, 0.0);

    // render outline of sphere
    renderWireframeSphereOutline();

    // retrieve rotation from arcball & apply
    Matrix4 rotMatrix = arcBall.getRotation();
    glMultMatrixf(reinterpret_cast<GLfloat*>(rotMatrix.columnMajor().coordinates));

    // and render a unit radius sphere
    renderWireframeSphere();
}

void ArcBallWidget::mousePressEvent(QMouseEvent* event) {
    const float x = event->x();
    const float y = event->y();
    const float width = this->width();
    const float height = this->height();
    const float scaledX = (2.0f * x - width) / width;
    // flip from Qt coordinates to Cartesian
    const float scaledY = (height - 2.0f * y) / height;
    // set the initial rotation for the drag
    arcBall.beginDrag(scaledX, scaledY);
    // signal we've changed
    emit rotationChanged();
}

void ArcBallWidget::mouseMoveEvent(QMouseEvent* event) {
    // ArcBallWidget::mouseMoveEvent()
    const float x = event->x();
    const float y = event->y();
    const float width = this->width();
    const float height = this->height();
    const float scaledX = (2.0f * x - width) / width;
    // flip from Qt coordinates to Cartesian
    const float scaledY = (height - 2.0f * y) / height;
    // set the mid point of the drag
    arcBall.continueDrag(scaledX, scaledY);
    // signal we've changed
    emit rotationChanged();
}

void ArcBallWidget::mouseReleaseEvent(QMouseEvent* event) {
    const float x = event->x();
    const float y = event->y();
    const float width = this->width();
    const float height = this->height();
    const float scaledX = (2.0f * x - width) / width;
    // flip from Qt coordinates to Cartesian
    const float scaledY = (height - 2.0f * y) / height;
    // set the final rotation for the drag
    arcBall.endDrag(scaledX, scaledY);
    // signal we've changed
    emit rotationChanged();
}

// routines called to allow synchronised rotation with other widget
// coordinates are assumed to be in range of [-1..1] in x,y
// if outside that range, will be clamped
void ArcBallWidget::beginDrag(const float x, const float y) {
    arcBall.beginDrag(x, y);

    emit rotationChanged();
}

void ArcBallWidget::continueDrag(const float x, const float y) {
    arcBall.continueDrag(x, y);

    emit rotationChanged();
}

void ArcBallWidget::endDrag(const float x, const float y) {
    arcBall.endDrag(x, y);

    emit rotationChanged();
}
