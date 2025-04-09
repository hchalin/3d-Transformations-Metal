//
// Created by Hayden Chalin on 4/8/25.
//

#include "Transform.h"

#include <iostream>


Transform::Transform() {
    std::cout << "Transform::Transform()" << std::endl;
    transformMatrix = Eigen::Matrix4f::Identity();
}

void Transform::setTranslation(float x, float y, float z) {


    Eigen::Matrix4f translationMatrix = Eigen::Matrix4f::Identity();
    translationMatrix(0, 3) = x;
    translationMatrix(1, 3) = y;
    translationMatrix(2, 3) = z;

    transformMatrix = translationMatrix * transformMatrix;

}

void Transform::setRotation(float angleRadians, float x, float y, float z) {
    Eigen::Vector3f axis(x, y, z);
    Eigen::AngleAxisf rotation(angleRadians, axis.normalized());
    Eigen::Matrix3f rotMatrix = rotation.toRotationMatrix();

    Eigen::Matrix4f rotationMatrix = Eigen::Matrix4f::Identity();
    rotationMatrix.block<3,3>(0,0) = rotMatrix;

    transformMatrix = rotationMatrix * transformMatrix;
}

void Transform::setScale(float x, float y, float z) {
    Eigen::Matrix4f scaleMatrix = Eigen::Matrix4f::Identity();
    scaleMatrix(0, 0) = x;
    scaleMatrix(1, 1) = y;
    scaleMatrix(2, 2) = z;

    transformMatrix = scaleMatrix * transformMatrix;
}

void Transform::reset() {
    transformMatrix = Eigen::Matrix4f::Identity();
}

const Eigen::Matrix4f& Transform::getMatrix() const {
    return transformMatrix;
}