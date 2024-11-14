#include "RenderController.h"

#include <cstdio>
#include <filesystem>
#include <fstream>

RenderController::RenderController(
    RenderParameters* renderParameters,
    RenderWindow* renderWindow,
    const std::string& meshName)
    : renderParameters(renderParameters),
      renderWindow(renderWindow),
      meshName(meshName),
      dragButton(Qt::NoButton) {
    // signals for arcballs
    QObject::connect(renderWindow->modelRotator, SIGNAL(rotationChanged()),
                     this, SLOT(objectRotationChanged()));
    QObject::connect(renderWindow->lightRotator, SIGNAL(rotationChanged()),
                     this, SLOT(lightRotationChanged()));

    // signals for main widget to control arcball
    QObject::connect(renderWindow->renderWidget, SIGNAL(beginScaledDrag(int, float, float)),
                     this, SLOT(mouseDown(int, float, float)));
    QObject::connect(renderWindow->renderWidget, SIGNAL(continueScaledDrag(float, float)),
                     this, SLOT(mouseDrag(float, float)));
    QObject::connect(renderWindow->renderWidget, SIGNAL(endScaledDrag(float, float)),
                     this, SLOT(mouseUp(float, float)));

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

    // Output to files
    QObject::connect(renderWindow->writeHalfedgeFile, SIGNAL(clicked()),
                     this, SLOT(writeToHalfedgeFile()));

    QObject::connect(renderWindow->writeObjFile, SIGNAL(clicked()),
                     this, SLOT(writeToObjFile()));

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
    renderParameters->zoomScale =
            std::clamp(std::pow(10.0f, static_cast<float>(value) / 100.0f), ZOOM_SCALE_MIN, ZOOM_SCALE_MAX);

    renderWindow->resetInterface();
}

void RenderController::xTranslateChanged(const int value) const {
    renderParameters->xTranslate =
            std::clamp(static_cast<float>(value) / 100.0f, TRANSLATE_MIN, TRANSLATE_MAX);

    renderWindow->resetInterface();
}

void RenderController::yTranslateChanged(const int value) const {
    renderParameters->yTranslate =
            std::clamp(static_cast<float>(value) / 100.0f, TRANSLATE_MIN, TRANSLATE_MAX);;

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

void RenderController::writeToHalfedgeFile() const {
    std::cout << "Writing .halfedge file..." << std::endl;

    const auto outFolder = std::filesystem::current_path() / "out";
    if (!exists(outFolder) && !create_directories(outFolder)) {
        std::cerr << "Failed to create /out folder. Abort." << std::endl << std::endl;
    }

    std::string fileStem = QString("%1_%2.halfedge")
            .arg(meshName.c_str()).arg(renderParameters->subdivisionNumber).toStdString();
    std::string outputMeshPath = outFolder / fileStem;
    std::ofstream outputFile(outputMeshPath);
    if (!outputFile.good()) {
        std::cerr << "Failed to output: " << std::endl << outputMeshPath << std::endl << std::endl;
    } else {
        renderWindow->renderWidget->triangleMesh->writeToHalfedgeFile(outputFile);
        std::cout << "Written to file: " << outputMeshPath << std::endl << std::endl;
    }
}

void RenderController::writeToObjFile() const {
    std::cout << "Writing .obj file..." << std::endl;

    const auto outFolder = std::filesystem::current_path() / "out";
    if (!exists(outFolder) && !create_directories(outFolder)) {
        std::cerr << "Failed to create /out folder. Abort." << std::endl << std::endl;
    }

    std::string fileStem = QString("%1_%2.obj")
            .arg(meshName.c_str()).arg(renderParameters->subdivisionNumber).toStdString();
    std::string outputMeshPath = outFolder / fileStem;
    std::ofstream outputFile(outputMeshPath);
    if (!outputFile.good()) {
        std::cerr << "Failed to output: " << std::endl << outputMeshPath << std::endl << std::endl;
    } else {
        renderWindow->renderWidget->triangleMesh->writeToObjFile(outputFile);
        std::cout << "Written to file: " << outputMeshPath << std::endl << std::endl;
    }
}

void RenderController::mouseDown(int whichButton, float x, float y) {
}

void RenderController::mouseDrag(float x, float y) {
}

void RenderController::mouseUp(float x, float y) {
}
