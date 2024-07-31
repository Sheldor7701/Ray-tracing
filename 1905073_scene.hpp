#include "1905073_camera.hpp"

using namespace std;

int recursion_level, pixels;
vector<Object*> objects;
vector<Light> lights;
InputHandler inputHandler;
Camera camera;

void loadSceneParameters(ifstream& input) {
    input >> recursion_level >> pixels;
}


void readAndSetProperties(Object* object, ifstream& input) {
    Color color;
    ReflectionCoefficients reflectionCoefficient;
    int shininess;

    input >> color >> reflectionCoefficient >> shininess;

    object->setColor(color);
    object->setCoefficients(reflectionCoefficient);
    object->setShine(shininess);
}

Sphere* readSphere(ifstream& input) {
    Vector3D center;
    double radius;
    input >> center >> radius;
    return new Sphere(center, radius);
}

Triangle* readTriangle(ifstream& input) {
    Vector3D a, b, c;
    input >> a >> b >> c;
    return new Triangle(a, b, c);
}

GeneralQuadricSurface* readGeneralQuadricSurface(ifstream& input) {
    Vector3D cubeReferencePoint;
    double A, B, C, D, E, F, G, H, I, J, length, width, height;
    input >> A >> B >> C >> D >> E >> F >> G >> H >> I >> J;
    input >> cubeReferencePoint >> length >> width >> height;
    return new GeneralQuadricSurface(A, B, C, D, E, F, G, H, I, J, length, width, height, cubeReferencePoint);
}

Object* readObject(ifstream& input) {
    string objectShape;
    input >> objectShape;

    if (objectShape == "sphere") {
        return readSphere(input);
    } else if (objectShape == "triangle") {
        return readTriangle(input);
    } else if (objectShape == "general") {
        return readGeneralQuadricSurface(input);
    } else {
        cout << objectShape << ": invalid object shape found" << endl;
        return nullptr;
    }
}

void loadObjects(ifstream& input) {
    int object_count;
    input >> object_count;

    for (int i = 0; i < object_count; i++) {
        Object* object = readObject(input);
        if (object) {
            readAndSetProperties(object, input);
            objects.push_back(object);
        } else {
            break;
        }
    }
}

Light createLight(ifstream& input) {
    Vector3D position;
    Color color;
    input >> position >> color;

    return Light(position, color);
}

Light createSpotLight(ifstream& input) {
    Vector3D position, direction;    
    Color color;
    double cutoffAngle;

    input >> position >> color >> direction >> cutoffAngle;

    Light sl(position, color);
    sl.setSpotLight(direction, cutoffAngle);

    return sl;
}

void loadLights(ifstream& input) {
    int lightsCount;
    input >> lightsCount;

    for (int i = 0; i < lightsCount; ++i) {
        lights.push_back(createLight(input));
    }
    
    int spotlightsCount;
    input >> spotlightsCount;

    for (int i = 0; i < spotlightsCount; ++i) {
        lights.push_back(createSpotLight(input));
    }
    
}

void addFloor(double floorWidth, double tileWidth, ReflectionCoefficients coefficients){
    Object* floor = new Floor(floorWidth, tileWidth);
    floor->setCoefficients(coefficients);
    floor->setShine(30);

    objects.push_back(floor);
}

void loadData() {
    ifstream input("scene.txt");
    if (!input) {
        cerr << "Unable to open file scene.txt" << endl;
        exit(1);
    }

    ReflectionCoefficients floor_coef(.3,.3,.3,.3);

    loadSceneParameters(input);
    loadObjects(input);
    loadLights(input);
    addFloor(1000, 20, floor_coef);

    input.close();
}