#include "RenderController.h"

#include <cstdio>

RenderController::RenderController(
    TriangleMesh* triangleMesh,
    RenderParameters* renderParameters,
    RenderWindow* renderWindow)
    : triangleMesh(triangleMesh),
      renderParameters(renderParameters),
      renderWindow(renderWindow),
      dragButton(Qt::NoButton),
      startX(-1),
      startY(-1),
      vertexId(-1) {
    // signals for arcballs
    QObject::connect(renderWindow->modelRotator, SIGNAL(rotationChanged()),
                     this, SLOT(objectRotationChanged()));
    QObject::connect(renderWindow->lightRotator, SIGNAL(rotationChanged()),
                     this, SLOT(lightRotationChanged()));

    // signals for main widget to control arcball
    QObject::connect(renderWindow->renderWidget, SIGNAL(mouseDown(int, int, int)),
                     this, SLOT(mouseDown(int, int, int)));
    QObject::connect(renderWindow->renderWidget, SIGNAL(mouseDrag(int, int)),
                     this, SLOT(mouseDrag(int, int)));
    QObject::connect(renderWindow->renderWidget, SIGNAL(mouseUp(int, int)),
                     this, SLOT(mouseUp(int, int)));

    // signal for zoom slider
    QObject::connect(renderWindow->zoomSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(zoomChanged(int)));

    // signal for x translate sliders
    QObject::connect(renderWindow->xTranslateSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(xTranslateChanged(int)));

    // signal for y translate slider
    QObject::connect(renderWindow->yTranslateSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(yTranslateChanged(int)));

    // signal for vertex scale slider
    QObject::connect(renderWindow->vertexSizeSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(vertexSizeChanged(int)));

    // signal for check box for flat normals
    QObject::connect(renderWindow->flatNormalsBox, SIGNAL(stateChanged(int)),
                     this, SLOT(flatNormalsCheckChanged(int)));

    // signal for check box for showing vertices
    QObject::connect(renderWindow->showVerticesBox, SIGNAL(stateChanged(int)),
                     this, SLOT(showVerticesCheckChanged(int)));

    // signal for check box for showing vertices
    QObject::connect(renderWindow->subdivisionSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(subdivisionNumberChanged(int)));

    // copy the rotation matrix from the widgets to the model
    renderParameters->rotationMatrix = renderWindow->modelRotator->rotationMatrix();
    renderParameters->lightMatrix = renderWindow->lightRotator->rotationMatrix();
}

void RenderController::objectRotationChanged() const {
    renderParameters->rotationMatrix = renderWindow->modelRotator->rotationMatrix();

    renderWindow->resetInterface();
}

void RenderController::lightRotationChanged() const {
    renderParameters->lightMatrix = renderWindow->lightRotator->rotationMatrix();

    renderWindow->resetInterface();
}

void RenderController::zoomChanged(const int value) const {
    float newZoomScale = std::pow(10.0, static_cast<float>(value) / 100.0f);

    if (newZoomScale < ZOOM_SCALE_MIN) {
        newZoomScale = ZOOM_SCALE_MIN;
    } else if (newZoomScale > ZOOM_SCALE_MAX) {
        newZoomScale = ZOOM_SCALE_MAX;
    }

    renderParameters->zoomScale = newZoomScale;

    renderWindow->resetInterface();
}

void RenderController::xTranslateChanged(const int value) const {
    renderParameters->xTranslate = static_cast<float>(value) / 100.0;

    if (renderParameters->xTranslate < TRANSLATE_MIN) {
        renderParameters->xTranslate = TRANSLATE_MIN;
    } else if (renderParameters->xTranslate > TRANSLATE_MAX) {
        renderParameters->xTranslate = TRANSLATE_MAX;
    }

    renderWindow->resetInterface();
}

void RenderController::yTranslateChanged(const int value) const {
    renderParameters->yTranslate = static_cast<float>(value) / 100.0f;

    if (renderParameters->yTranslate < TRANSLATE_MIN) {
        renderParameters->yTranslate = TRANSLATE_MIN;
    } else if (renderParameters->yTranslate > TRANSLATE_MAX) {
        renderParameters->yTranslate = TRANSLATE_MAX;
    }

    renderWindow->resetInterface();
}

void RenderController::vertexSizeChanged(const int value) const {
    renderParameters->vertexSize = static_cast<float>(value) / 512.0f;

    renderWindow->resetInterface();
}

void RenderController::showVerticesCheckChanged(const int state) const {
    renderParameters->showVertices = state == Qt::Checked;

    renderWindow->resetInterface();
}

void RenderController::flatNormalsCheckChanged(const int state) const {
    renderParameters->useFlatNormals = state == Qt::Checked;

    renderWindow->resetInterface();
}

void RenderController::subdivisionNumberChanged(const int number) const {
    renderParameters->subdivisionNumber = number;

    renderWindow->resetInterface();
}

void RenderController::mouseDown(int whichButton, int x, int y) {
}

void RenderController::mouseDrag(int x, int y) {
}

void RenderController::mouseUp(int x, int y) {
}
