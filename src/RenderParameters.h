#ifndef RENDER_PARAMETERS_H
#define RENDER_PARAMETERS_H

#include "Matrix4.h"

class RenderParameters {
public:
    float xTranslate, yTranslate;
    float zoomScale;
    std::array<float, 4> lightPosition;

    // rotations for the model and the light
    Matrix4 rotationMatrix;
    Matrix4 lightMatrix;

    bool useFlatNormals;
    bool showVertices;

    float vertexSize;

    unsigned int subdivisionNumber;

    RenderParameters();
};

inline RenderParameters::RenderParameters()
    : xTranslate(0.0),
      yTranslate(0.0),
      zoomScale(1.0),
      // start the lighting at the viewer's direction
      lightPosition({0.0f, 0.0f, 1.0f, 0.0f}),
      useFlatNormals(true),
      showVertices(true),
      vertexSize(0.25),
      subdivisionNumber(0) {
    rotationMatrix = Matrix4::identity();
    lightMatrix = Matrix4::identity();
}

// now define some macros for bounds on parameters
#define TRANSLATE_MIN (-1.0)
#define TRANSLATE_MAX 1.0

#define ZOOM_SCALE_LOG_MIN (-2.0)
#define ZOOM_SCALE_LOG_MAX 2.0
#define ZOOM_SCALE_MIN 0.01
#define ZOOM_SCALE_MAX 100.0

#define MINIMUM_SUBDIVISION_NUMBER 0
// Only for the brave, or the willing to wait for O(N^2)
#define MAXIMUM_SUBDIVISION_NUMBER 8

// this is to scale to/from integer values
#define PARAMETER_SCALING 100

#endif
