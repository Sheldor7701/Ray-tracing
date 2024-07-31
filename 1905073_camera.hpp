#ifndef CAMERA_H
#define CAMERA_H

#include "1905073_rayTracing.hpp"

class Camera {
public:
    Vector3D pos, l, r, u;
    double angleRotationZ;

    Camera() : angleRotationZ(90.0) {

        double val = 1.0/(sqrt(2.0));

        pos.setVector(-120, -120, 60);

        u.setVector(0, 0, 1);
        u.normalize();

        r.setVector(val, -val, 0);
        r.normalize();

        l.setVector(val, val, 0);
        l.normalize();
    }

    void initialize() {
        pos = {15, 0, 6};
        l = {-1, 0, 0};
        r = {0, 1, 0};
        u = {0, 0, 1};
    }

        void rotateLeft(double angle) {
        r = r * cos(angle) + l * sin(angle);
        l = l * cos(angle) - r * sin(angle);
    }

    void rotateRight(double angle) {
        r = r * cos(-angle) + l * sin(-angle);
        l = l * cos(-angle) - r * sin(-angle);
    }

    void lookUp(double angle) {
        l = l * cos(angle) + u * sin(angle);
        u = u * cos(angle) - l * sin(angle);
    }

    void lookDown(double angle) {
        l = l * cos(-angle) + u * sin(-angle);
        u = u * cos(-angle) - l * sin(-angle);
    }

    void tiltCounterclockwise(double angle) {
        u = u * cos(angle) + r * sin(angle);
        r = r * cos(angle) - u * sin(angle);
    }

    void tiltClockwise(double angle) {
        u = u * cos(-angle) + r * sin(-angle);
        r = r * cos(-angle) - u * sin(-angle);
    }

    void moveUpWithoutChangingReference(double distance) {
        pos = pos + u * distance;
    }

    void moveDownWithoutChangingReference(double distance) {
        pos = pos - u * distance;
    }

    void rotateZ(double angle) {
        angleRotationZ += angle;
    }

    void moveForward(double distance) {
        pos = pos + l * distance;
    }

    void moveBackward(double distance) {
        pos = pos - l * distance;
    }

    void moveRight(double distance) {
        pos = pos + r * distance;
    }

    void moveLeft(double distance) {
        pos = pos - r * distance;
    }

    void moveUp(double distance) {
        pos = pos + u * distance;
    }

    void moveDown(double distance) {
        pos = pos - u * distance;
    }

    void updateLookAt() {
        gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);
    }
};

class InputHandler {
public:
    void handlelengthalKey(unsigned char key, Camera &camera) {

        double rotationAngle = 5*PI/360;

        switch (key) {
            // Rotation
            case '1':
                camera.rotateLeft(rotationAngle);
                break;
            case '2':
                camera.rotateRight(rotationAngle);
                break;
            case '3':
                camera.lookUp(rotationAngle);
                break;
            case '4':
                camera.lookDown(rotationAngle);
                break;
            case '5':
                camera.tiltCounterclockwise(rotationAngle);
                break;
            case '6':
                camera.tiltClockwise(rotationAngle);
                break;
            default:
                break;
        }
                glutPostRedisplay();

    }

    void handleSpecialKey(int key, Camera &camera) {
        double translationSpeed = 0.5;

        switch (key) {
            // Translation
            case GLUT_KEY_UP:
                camera.moveForward(translationSpeed);
                break;
            case GLUT_KEY_DOWN:
                camera.moveBackward(translationSpeed);
                break;
            case GLUT_KEY_RIGHT:
                camera.moveRight(translationSpeed);
                break;
            case GLUT_KEY_LEFT:
                camera.moveLeft(translationSpeed);
                break;
            case GLUT_KEY_PAGE_UP:
                camera.moveUp(translationSpeed);
                break;
            case GLUT_KEY_PAGE_DOWN:
                camera.moveDown(translationSpeed);
                break;
            default:
                break;

        }
        glutPostRedisplay();
    }
};

#endif // CAMERA_H