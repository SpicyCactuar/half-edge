#include "ArcBall.h"

#include <cmath>

ArcBall::ArcBall() {
    // set to (0, 0, 0, 1) - no rotation
    baseRotation = Quaternion(0, 0, 0, 1);
    // set to (0, 0, 0, 1) - no rotation
    currentRotation = Quaternion(0, 0, 0, 1);
    // set dragLast to something predictable
    dragFrom = Quaternion(0, 0, 1, 0);
}

// convert an (x,y) point to a quaternion
Quaternion ArcBall::findQuat(const float x, const float y) {
    Cartesian3 resultVector;
    const float dSquared = x * x + y * y;
    if (dSquared > 1.0) {
        // outside the circle
        // constrain to the circle with scaling
        resultVector = Cartesian3(x / sqrt(dSquared), y / sqrt(dSquared), 0.0);
    } else {
        // inside the circle, so 1-dSquared = z squared
        resultVector = Cartesian3(x, y, sqrt(1.0 - dSquared));
    }
    return Quaternion(resultVector);
}

void ArcBall::beginDrag(const float x, const float y) {
    dragFrom = findQuat(x, y);
}

void ArcBall::continueDrag(const float x, const float y) {
    // find the quaternion representing the point
    const Quaternion nextQuat = findQuat(x, y);
    // invert the previous quaternion
    const Quaternion fromInverse = dragFrom.inverse();
    // use it to compute the current rotation
    currentRotation = nextQuat * fromInverse;
}

// stop dragging
void ArcBall::endDrag(float x, float y) {
    // find the quaternion representing the point
    const Quaternion nextQuat = findQuat(x, y);
    // invert the previous quaternion
    const Quaternion fromInverse = dragFrom.inverse();
    // use it to compute the current rotation
    currentRotation = nextQuat * fromInverse;

    // reset the quaternions ready for next time
    dragFrom = Quaternion(0, 0, 1, 0);
    // reset current and base
    baseRotation = currentRotation * baseRotation;
    currentRotation = Quaternion(0, 0, 0, 1);
}

// extract the rotation matrix for rendering purposes
Matrix4 ArcBall::getRotation() const {
    return currentRotation.asMatrix() * baseRotation.asMatrix();
}
