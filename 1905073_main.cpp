#include "bitmap_image.hpp"
#include "1905073_scene.hpp"

using namespace std;

int windowWidth = 500;
int windowHeight = 500;
int captureCount  = 11;
double viewAngle = 80;

void drawObjects()
{
    for (Object* o : objects) {
        o->draw();
    }
}

void setDefaultBackgroundColor(bitmap_image& image, int imageWidth, int imageHeight) {
    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageHeight; j++) {
            image.set_pixel(i, j, 0, 0, 0);
        }
    }
}

Vector3D calculateTopLeft(Camera& camera, double windowWidth, double windowHeight) {
    Vector3D temp = camera.pos + camera.l * (windowHeight * 0.5) / tan((viewAngle * 0.5) * (PI / 180));
    temp = temp - camera.r * (windowWidth / 2.0);
    return temp + camera.u * (windowHeight / 2.0);
}

void calculatePixelParameters(Camera& camera, int imageWidth, int imageHeight, double du, double dv, Vector3D& topLeft) {
    Vector3D temp = camera.r * (du / 2.0) - camera.u * (dv / 2.0);
    topLeft = topLeft + temp;
}

Ray calculateRay(Camera& camera, Vector3D& topLeft, double du, double dv, int i, int j) {
    Vector3D curPixel = topLeft + camera.r * (du * i) - camera.u * (dv * j);
    return Ray(camera.pos, (curPixel - camera.pos));
}

int findNearestObject(Ray& ray, Color& color, vector<Object*>& objects) {
    int nearest = -1;
    double tMin = INFINITY;

    for (int k = 0; k < objects.size(); k++) {
        double t = objects[k]->intersectWithIllumination(ray, color, 0);
        if (t < tMin && t > 0) {
            nearest = k;
            tMin = t;
        }
    }
    return nearest;
}

void capture() {
    cout << "Capturing bitmap image " << pixels << endl;

    int imageWidth = pixels;
    int imageHeight = pixels;

    bitmap_image image(pixels, pixels);
    setDefaultBackgroundColor(image, imageWidth, imageHeight);

    double planeDistance = (windowHeight * 0.5) / tan((viewAngle * 0.5) * (PI / 180));
    Vector3D topLeft = calculateTopLeft(camera, windowWidth, windowHeight);

    double du = (double)windowWidth / imageWidth;
    double dv = (double)windowHeight / imageHeight;

    calculatePixelParameters(camera, imageWidth, imageHeight, du, dv, topLeft);

    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageHeight; j++) {
            Ray ray = calculateRay(camera, topLeft, du, dv, i, j);
            Color color;

            int nearest = findNearestObject(ray, color, objects);

            if (nearest != -1) {
                double tNear = objects[nearest]->intersectWithIllumination(ray, color, 1);
            }

            color.fix();
            image.set_pixel(i, j, (color.getR() * 255), (color.getG() * 255), (color.getB()) * 255);
        }
    }

    string outPath = "output_" + to_string(captureCount) + ".bmp";
    captureCount++;

    image.save_image(outPath);
    image.clear();

    cout << "Finished Capturing bitmap image. Path: " << outPath << endl;
}

void drawLights()
{
    for (Light l : lights) {
        l.draw();
    }
}


void handlelengthalKey(unsigned char key, Camera &camera) {

    double rotationAngle = 5*PI/360;

    switch (key) {
        case '0':
            capture();
            break;
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
    double translationSpeed = 5;

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



static void keyboardCallback(unsigned char key, int x, int y) {
    handlelengthalKey(key, camera);
}

static void specialKeyCallback(int key, int x, int y) {
    handleSpecialKey(key, camera);
}


void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);	//color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    gluLookAt(  camera.pos.getX(), camera.pos.getY(), camera.pos.getZ(), 
                camera.pos.getX() + camera.l.getX(), camera.pos.getY() + camera.l.getY(), camera.pos.getZ() + camera.l.getZ(), 
                camera.u.getX(), camera.u.getY(), camera.u.getZ()   );

    glMatrixMode(GL_MODELVIEW);

    drawLights();
    drawObjects();

    glutSwapBuffers();
}

void animate() {
	glutPostRedisplay();
}

void init() {
    camera = Camera();

    glClearColor(0,0,0,0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(viewAngle,	1,	1,	1000.0);
}

void clearMemory() {
    objects.clear();
    lights.clear();
}


int main(int argc, char **argv){

    glutInit(&argc,argv);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);

    glutCreateWindow("Ray Tracing 1905073");

    loadData();
    init();

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutIdleFunc(animate);

    glutKeyboardFunc(keyboardCallback);
    glutSpecialFunc(specialKeyCallback);


    glutMainLoop();

    clearMemory();
    return 0;
}
