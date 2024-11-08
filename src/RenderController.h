#ifndef RENDER_CONTROLLER_H
#define RENDER_CONTROLLER_H

#include <QtGui>

#include "RenderWindow.h"
#include "TriangleMesh.h"
#include "RenderParameters.h"

class RenderController : public QObject {
    Q_OBJECT

    TriangleMesh* triangleMesh;

    RenderParameters* renderParameters;

    RenderWindow* renderWindow;

    int dragButton;
    int startX, startY;
    int vertexId;

public:
    RenderController(
        TriangleMesh* triangleMesh,
        RenderParameters* renderParameters,
        RenderWindow* renderWindow
    );

public slots:
    // slot for responding to arcball rotation for object
    void objectRotationChanged() const;

    void lightRotationChanged() const;

    // slots for responding to zoom & translate sliders
    void zoomChanged(int value) const;

    void xTranslateChanged(int value) const;

    void yTranslateChanged(int value) const;

    // slot for responding to vertex size slider
    void vertexSizeChanged(int value) const;

    // slots for responding to check boxes
    void showVerticesCheckChanged(int state) const;

    void flatNormalsCheckChanged(int state) const;

    // slot for subdivision slider
    void subdivisionNumberChanged(int number) const;

    // slots for responding to widget manipulations
    void mouseDown(int whichButton, int x, int y);

    // note that Continue & End assume the button has already been set
    void mouseDrag(int x, int y);

    void mouseUp(int x, int y);
};

#endif
