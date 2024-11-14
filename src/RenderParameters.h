#ifndef RENDER_PARAMETERS_H
#define RENDER_PARAMETERS_H

#include <array>

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
    : xTranslate(0.0f),
      yTranslate(0.0f),
      zoomScale(1.0f),
      // start the lighting at the viewer's direction
      lightPosition({0.0f, 0.0f, 1.0f, 0.0f}),
      useFlatNormals(true),
      showVertices(true),
      vertexSize(0.25f),
      subdivisionNumber(0) {
    rotationMatrix = Matrix4::identity();
    lightMatrix = Matrix4::identity();
}

// Define some macros for bounds on parameters
#define TRANSLATE_MIN (-1.0f)
#define TRANSLATE_MAX 1.0f

#define ZOOM_SCALE_LOG_MIN (-2.0)
#define ZOOM_SCALE_LOG_MAX 2.0
#define ZOOM_SCALE_MIN 0.01f
#define ZOOM_SCALE_MAX 100.0f

#define MINIMUM_SUBDIVISION_NUMBER 0
// Only for the brave, or the willing to wait for O(N^2)
#define MAXIMUM_SUBDIVISION_NUMBER 8

// Scale to/from integer values
#define PARAMETER_SCALING 100

#endif
