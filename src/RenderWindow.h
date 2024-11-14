#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

#include <QtWidgets>

#include "ArcBallWidget.h"
#include "RenderWidget.h"

// window that displays a geometric model with controls
class RenderWindow : public QWidget {
    // subdivision[renderParameters->subdivisionNumber] is the one displayed
    std::vector<TriangleMesh> subdivisions;

    RenderParameters* renderParameters;

    QGridLayout* windowLayout;

    ArcBallWidget* modelRotator;
    ArcBallWidget* lightRotator;
    RenderWidget* renderWidget;

    QCheckBox* flatNormalsBox;
    QCheckBox* showVerticesBox;
    QPushButton* writeHalfedgeFile;
    QPushButton* writeObjFile;

    QSlider* xTranslateSlider;
    QSlider* yTranslateSlider;
    QSlider* zoomSlider;

    QSlider* subdivisionSlider;

    QSlider* vertexSizeSlider;

    QLabel* modelRotatorLabel;
    QLabel* lightRotatorLabel;
    QLabel* yTranslateLabel;
    QLabel* zoomLabel;
    QLabel* vertexSizeLabel;
    QLabel* subdivisionLabel;

public:
    RenderWindow(
        TriangleMesh* triangleMesh,
        RenderParameters* renderParameters,
        const std::string& windowName = "Half-Edge Renderer"
    );

    void resetInterface();

    // declare the render controller class a friend so it can access the UI elements
    friend class RenderController;
};

#endif
