#include "RenderWindow.h"

#include <fstream>

#include "RenderParameters.h"

RenderWindow::RenderWindow(
    // the object to be rendered
    TriangleMesh* triangleMesh,
    RenderParameters* renderParameters,
    const std::string& windowName
) : QWidget(nullptr),
    renderParameters(renderParameters) {
    // Consider subdivisions[0] as first surface
    this->subdivisions = {*triangleMesh};

    setWindowTitle(QString(windowName.c_str()));

    windowLayout = new QGridLayout(this);

    renderWidget = new RenderWidget(triangleMesh, renderParameters, this);

    lightRotator = new ArcBallWidget(this);
    modelRotator = new ArcBallWidget(this);

    showVerticesBox = new QCheckBox("Show Vertices", this);
    flatNormalsBox = new QCheckBox("Flat Normals", this);
    writeHdsFile = new QPushButton("Write .hds", this);
    writeObjFile = new QPushButton("Write .obj", this);

    xTranslateSlider = new QSlider(Qt::Horizontal, this);
    yTranslateSlider = new QSlider(Qt::Vertical, this);
    zoomSlider = new QSlider(Qt::Vertical, this);

    subdivisionSlider = new QSlider(Qt::Horizontal, this);

    vertexSizeSlider = new QSlider(Qt::Horizontal, this);

    modelRotatorLabel = new QLabel("Model", this);
    lightRotatorLabel = new QLabel("Light", this);
    yTranslateLabel = new QLabel("Y", this);
    zoomLabel = new QLabel("Zm", this);
    vertexSizeLabel = new QLabel("Vertex Size", this);
    const std::string subdivisionLabelText = "Subdivisions [" + std::to_string(MINIMUM_SUBDIVISION_NUMBER) + ", " +
                                             std::to_string(MAXIMUM_SUBDIVISION_NUMBER) + "]";
    subdivisionLabel = new QLabel(subdivisionLabelText.c_str(), this);

    // Add the widgets to the grid | Row | Column | Row Span | Column Span |

    // the top two widgets have to fit to the widgets stack between them
    constexpr int nStacked = 6;

    windowLayout->addWidget(renderWidget, 0, 1, nStacked, 1);
    windowLayout->addWidget(yTranslateSlider, 0, 2, nStacked, 1);
    windowLayout->addWidget(zoomSlider, 0, 4, nStacked, 1);

    // Stack in the middle
    windowLayout->addWidget(lightRotator, 0, 3, 1, 1);
    windowLayout->addWidget(lightRotatorLabel, 1, 3, 1, 1);
    windowLayout->addWidget(modelRotator, 2, 3, 1, 1);
    windowLayout->addWidget(modelRotatorLabel, 3, 3, 1, 1);
    windowLayout->addWidget(flatNormalsBox, 4, 3, 1, 1);
    windowLayout->addWidget(showVerticesBox, 5, 3, 1, 1);
    windowLayout->addWidget(writeHdsFile, 6, 3, 1, 1);
    windowLayout->addWidget(writeObjFile, 7, 3, 1, 1);

    // Translate Slider Row
    windowLayout->addWidget(xTranslateSlider, nStacked, 1, 1, 1);
    windowLayout->addWidget(yTranslateLabel, nStacked, 2, 1, 1);
    // nothing in column 3
    windowLayout->addWidget(zoomLabel, nStacked, 4, 1, 1);

    // Vertex Size slider row
    windowLayout->addWidget(vertexSizeSlider, nStacked + 1, 1, 1, 1);
    windowLayout->addWidget(vertexSizeLabel, nStacked + 1, 2, 1, 1);

    // Subdivision Row
    windowLayout->addWidget(subdivisionSlider, nStacked + 2, 1, 1, 1);
    windowLayout->addWidget(subdivisionLabel, nStacked + 2, 2, 1, 1);

    resetInterface();
}

// routine to reset the interface
// sets every visual control to match the model
// gets called by the controller after each change in the model
void RenderWindow::resetInterface() {
    // RenderWindow::ResetInterface()
    // Check if subdivisions need to be generated
    for (unsigned int i = subdivisions.size(); i <= renderParameters->subdivisionNumber; i++) {
        std::cout << "Generating Subdivision " << i << "..." << std::endl;
        TriangleMesh subdivision = subdivisions[i - 1].subdivide();
        subdivisions.push_back(subdivision);
        std::cout << "Finished generating Subdivision " << i << std::endl;
    }
    // Render target subdivision, guaranteed to be ready by this point
    renderWidget->triangleMesh = &subdivisions[renderParameters->subdivisionNumber];

    // set check boxes
    showVerticesBox->setChecked(renderParameters->showVertices);
    flatNormalsBox->setChecked(renderParameters->useFlatNormals);

    // set sliders
    // x & y translate are scaled to notional unit sphere in render widgets
    // but because the slider is defined as integer, we multiply by a 100 for all sliders
    xTranslateSlider->setMinimum(static_cast<int>(TRANSLATE_MIN * PARAMETER_SCALING));
    xTranslateSlider->setMaximum(static_cast<int>(TRANSLATE_MAX * PARAMETER_SCALING));
    xTranslateSlider->setValue(static_cast<int>(renderParameters->xTranslate * PARAMETER_SCALING));

    yTranslateSlider->setMinimum(static_cast<int>(TRANSLATE_MIN * PARAMETER_SCALING));
    yTranslateSlider->setMaximum(static_cast<int>(TRANSLATE_MAX * PARAMETER_SCALING));
    yTranslateSlider->setValue(static_cast<int>(renderParameters->yTranslate * PARAMETER_SCALING));

    // zoom slider is a logarithmic scale, so we want a narrow range
    zoomSlider->setMinimum(static_cast<int>(ZOOM_SCALE_LOG_MIN * PARAMETER_SCALING));
    zoomSlider->setMaximum(static_cast<int>(ZOOM_SCALE_LOG_MAX * PARAMETER_SCALING));
    zoomSlider->setValue(static_cast<int>(std::log10(renderParameters->zoomScale) * PARAMETER_SCALING));

    // subdivision slider [MINIMUM_SUBDIVISION_NUMBER, MAXIMUM_SUBDIVISION_SLIDER]
    subdivisionSlider->setMinimum(MINIMUM_SUBDIVISION_NUMBER);
    subdivisionSlider->setMaximum(MAXIMUM_SUBDIVISION_NUMBER);
    subdivisionSlider->setValue(renderParameters->subdivisionNumber);
    subdivisionSlider->setTickInterval(1);
    subdivisionSlider->setSingleStep(1);

    // vertex scale is a linear variable in %
    vertexSizeSlider->setMinimum(0);
    vertexSizeSlider->setMaximum(512);
    vertexSizeSlider->setValue(512 * renderParameters->vertexSize);

    // flag them all for update
    renderWidget->update();
    modelRotator->update();
    lightRotator->update();
    xTranslateSlider->update();
    yTranslateSlider->update();
    zoomSlider->update();
    vertexSizeSlider->update();
    showVerticesBox->update();
    flatNormalsBox->update();
    subdivisionSlider->update();
}
