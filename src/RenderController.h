#ifndef RENDER_CONTROLLER_H
#define RENDER_CONTROLLER_H

#include <QtGui>

#include "RenderWindow.h"
#include "RenderParameters.h"

class RenderController : public QObject {
    Q_OBJECT

    RenderParameters* renderParameters;
    RenderWindow* renderWindow;
    std::string meshName;

    int dragButton;

public:
    RenderController(
        RenderParameters* renderParameters,
        RenderWindow* renderWindow,
        const std::string& meshName
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

    // Output to files
    void writeToHalfedgeFile() const;

    void writeToObjFile() const;

    // slots for responding to widget manipulations
    void mouseDown(int whichButton, float x, float y);

    void mouseDrag(float x, float y);

    void mouseUp(float x, float y);
};

#endif
