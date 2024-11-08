#ifndef ARCBALL_H
#define ARCBALL_H

#include "Quaternion.h"
#include "Matrix4.h"

class ArcBall {
public:
    Quaternion baseRotation;
    Quaternion currentRotation;
    Quaternion dragFrom;

    ArcBall();

    // convert an (x,y) point to a quaternion
    static Quaternion findQuat(float x, float y);

    // start the dragging process with a point in the range [-1..1]^2
    void beginDrag(float x, float y);

    // continue the dragging process with another such point
    void continueDrag(float x, float y);

    // stop dragging
    void endDrag(float x, float y);

    // extract the rotation matrix for rendering purposes
    Matrix4 getRotation() const;
};

#endif
