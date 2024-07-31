#ifndef CLASSES_H
#define CLASSES_H

#include<bits/stdc++.h>

#ifdef __linux__
    #include <GL/glut.h>
#elif WIN32
    #include <windows.h>
    #include <glut.h>
#endif

using namespace std;

#define PI 2 * acos(0.0)
#define INF std::numeric_limits<double>::infinity()
#define EPSILON 0.0000001

class Color;

class Vector3D;

class Ray;

class Light;

class Object;

class ReflectionCoefficients;

class Sphere;

class Triangle;

class GeneralQuadricSurface;

class Floor;

class Color {
    double normalize(double value) {
        return (value > 1.0) ? 1.0 : (value < 0.0) ? 0.0 : value;
    }

    double r, g, b;

public:
    Color() : r(0.0), g(0.0), b(0.0) {}

    Color(double r, double g, double b) : r(r), g(g), b(b) {}

    double getR() { return r; }
    double getG() { return g; }
    double getB() { return b; }

    void setR(double r) { this->r = r; }
    void setG(double g) { this->g = g; }
    void setB(double b) { this->b = b; }

    void setColor(double r, double g, double b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    void fix() {
        r = normalize(r);
        g = normalize(g);
        b = normalize(b);
    }

    Color operator+(Color c) {
        return Color(r + c.getR(), g + c.getG(), b + c.getB());
    }

    Color operator*(double k) {
        return Color(r * k, g * k, b * k);
    }

    Color operator*(Color c) {
        return Color(r * c.getR(), g * c.getG(), b * c.getB());
    }

    friend std::ifstream &operator>>(std::ifstream &in, Color &c);
};

std::ifstream &operator>>(std::ifstream &in, Color &c) {
    in >> c.r >> c.g >> c.b;
    return in;
}

class Vector3D {

public:
    double x, y, z;

    Vector3D() : x(0.0), y(0.0), z(0.0) {}

    Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

    double getX() { return x; }
    double getY() { return y; }
    double getZ() { return z; }

    void setX(double x) { this->x = x; }
    void setY(double y) { this->y = y; }
    void setZ(double z) { this->z = z; }

    void setVector(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void normalize() {
        double length = std::sqrt(x * x + y * y + z * z);
        x /= length;
        y /= length;
        z /= length;
    }

    double getDistanceVector(Vector3D v) {
        double dx = x - v.getX();
        double dy = y - v.getY();
        double dz = z - v.getZ();

        double squaredDistance = dx * dx + dy * dy + dz * dz;

        return std::sqrt(squaredDistance);
    }


    Vector3D operator+(Vector3D v) {
        return Vector3D(x + v.getX(), y + v.getY(), z + v.getZ());
    }

    Vector3D operator-(Vector3D v) {
        return Vector3D(x - v.getX(), y - v.getY(), z - v.getZ());
    }

    Vector3D operator*(double d) {
        return Vector3D(x * d, y * d, z * d);
    }

    Vector3D operator/(double d) {
        return Vector3D(x / d, y / d, z / d);
    }

    double dot(Vector3D v) {
        return x * v.getX() + y * v.getY() + z * v.getZ();
    }

    Vector3D cross(Vector3D v) {

        double crossX = y * v.getZ() - z * v.getY();
        double crossY = z * v.getX() - x * v.getZ();
        double crossZ = x * v.getY() - y * v.getX();

        return Vector3D(crossX, crossY, crossZ);
    }


    friend std::ifstream &operator>>(std::ifstream &in, const Vector3D &v);
};

std::ifstream &operator>>(std::ifstream &in, Vector3D &v) {
    in >> v.x >> v.y >> v.z;
    return in;
}

class Ray {
    Vector3D origin, direction;

public:
    Ray() {
        origin = Vector3D(0.0, 0.0, 0.0);
        direction = Vector3D(0.0, 0.0, 0.0);
    }

    Ray(Vector3D origin, Vector3D direction) : origin(origin) {
        direction.normalize();
        this->direction = direction;
    }

    Vector3D getOrigin() { return origin; }
    Vector3D getDirection() { return direction; }

    void setOrigin(Vector3D &origin) { this->origin = origin; }
    void setDirection(Vector3D &direction) { this->direction = direction; }

    Vector3D getPointAtParameter(double t) {
        return origin + (direction*t);
    }
};


class Light {
private:
    Vector3D light_pos;
    Color color;
    bool is_SpotLight;
    Vector3D spotDirection;
    double spotCutoff;
    double radius;
    int segments;
    int stacks;

public:
    Light();
    Light(Vector3D l_pos, Color c, double r = 1.0, int seg = 12, int stck = 4);

    void setSpotLight(Vector3D dir, double cutoff);
    bool isSpotLight() const;
    Vector3D getSpotDirection() const;
    double getSpotCutoff() const;
    Color getColor() const;
    Vector3D getLightPos() const;
    void draw();

};

class ReflectionCoefficients {
    double ka, kd, ks, kr;

public:
    ReflectionCoefficients() : ka(0.0), kd(0.0), ks(0.0), kr(0.0) {}

    ReflectionCoefficients(double ka, double kd, double ks, double kr) : ka(ka), kd(kd), ks(ks), kr(kr) {}

    double getKa() { return ka; }
    double getKd() { return kd; }
    double getKs() { return ks; }
    double getKr() { return kr; }

    void setKa(double ka) { this->ka = ka; }
    void setKd(double kd) { this->kd = kd; }
    void setKs(double ks) { this->ks = ks; }
    void setKr(double kr) { this->kr = kr; }

    friend std::ifstream &operator>>(std::ifstream &in, ReflectionCoefficients &coefficients);
};

std::ifstream &operator>>(std::ifstream &in, ReflectionCoefficients &coefficients) {
    in >> coefficients.ka >> coefficients.kd >> coefficients.ks >> coefficients.kr;
    return in;
}

class Object {
protected:
    Color color;
    ReflectionCoefficients coefficients;

    Vector3D reference_point;
    
    int shine;
    double height, width, length;

public:
    Object() = default;
    virtual void draw();
    virtual double intersect(Ray r, Color clr, int level);
    virtual Vector3D getNormalAt(Vector3D intersectionPoint);
    virtual Color getColorAt(Vector3D intersectionPoint);
    void setColor(Color c);
    void setColor(double c1, double c2, double c3);
    void setShine(int s);
    void setCoefficients(double c1, double c2, double c3, double c4);
    void setCoefficients(ReflectionCoefficients c);
    double getLength();
    double intersectWithIllumination(Ray& r, Color& clr, int level);
    Color calculateAmbientColor(Vector3D& intersectionPoint);
    Vector3D calculateAndNormalizeNormal(Vector3D& intersectionPoint);
    void handleLightSource(Vector3D& normal, Vector3D& intersectionPoint, Color& clr, double tmin, Vector3D& rd);
    bool isInShadow(Ray& lightRay, double tmin);
    void calculateLambertAndPhong(Vector3D& normal, Vector3D& lightDir, Color& clr, Light& l, double& lambert, double& phong, Vector3D& rd, Vector3D& intersectionPoint);
    void handleDiffuseAndSpecular(Color& clr, Light& l, double lambert, double phong, Vector3D& intersectionPoint);
    void handleRecursiveReflection(Vector3D& rd, Color& clr, Vector3D& intersectionPoint, Vector3D& normal, int level);
    Ray get_reflectedRay(Vector3D& intersectionPoint, Vector3D& normal, Vector3D& rd);
    void handleReflectedColor(Ray& rayReflected, Color& clr, int level);
    int getNearestIntersectingObject(Ray& rayReflected, Color& colorReflected);
};


class Sphere : public Object {
private:
    double radius;

public:
    Sphere(Vector3D center, double radius) : Object(), radius(radius) {
        reference_point = center;
        length = radius;
    }

    void generatePoints(Vector3D points[100][100], int stacks, int slices, double radius);
    void draw() override;
    double intersect(Ray r, Color clr, int level) override;
    Vector3D getNormalAt(Vector3D intersectionPoint) override;
    

    // Getters and setters
    double getRadius() const { return radius; }
    void setRadius(double r) { radius = r; }

    // Destructor
    ~Sphere() = default;
};

class Triangle : public Object {
    Vector3D v1, v2, v3;

public:
    Triangle() {}

    Triangle(Vector3D v1, Vector3D v2, Vector3D v3)
        : v1(v1), v2(v2), v3(v3){}

    Vector3D &getv1() { return v1; }
    Vector3D &getv2() { return v2; }
    Vector3D &getv3() { return v3; }

    void setv1(Vector3D &v1) { this->v1 = v1; }
    void setv2(Vector3D &v2) { this->v2 = v2; }
    void setv3(Vector3D &v3) { this->v3 = v3; }

    void draw() override;
    double intersect(Ray r, Color clr, int level) override;
    Vector3D getNormalAt(Vector3D intersectionPoint) override;
};

class GeneralQuadricSurface : public Object {
private:
    double A, B, C, D, E, F, G, H, I, J;

public:
    GeneralQuadricSurface(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j,
                      double length, double width, double height, Vector3D ref)
    : A(a), B(b), C(c), D(d), E(e), F(f), G(g), H(h), I(i), J(j){
        this->length = length;
        this->width = width;
        this->height = height;
        reference_point = ref;
    }
    
    double getA() { return A; }
    double getB() { return B; }
    double getC() { return C; }
    double getD() { return D; }
    double getE() { return E; }
    double getF() { return F; }
    double getG() { return G; }
    double getH() { return H; }
    double getI() { return I; }
    double getJ() { return J; }

    void setA(double A) { this->A = A; }
    void setB(double B) { this->B = B; }
    void setC(double C) { this->C = C; }
    void setD(double D) { this->D = D; }
    void setE(double E) { this->E = E; }
    void setF(double F) { this->F = F; }
    void setG(double G) { this->G = G; }
    void setH(double H) { this->H = H; }
    void setI(double I) { this->I = I; }
    void setJ(double J) { this->J = J; }

    bool withinReferenceCube(Vector3D p);
    double intersect(Ray r, Color clr, int level) override;
    Vector3D getNormalAt(Vector3D intersectionPoint) override;

};


class Floor : public Object {
public:
    Floor(double floorWidth, double tileWidth) {
        reference_point = {-floorWidth / 2, -floorWidth / 2, 0};
        length = tileWidth;
    }

    Color getColorAt(Vector3D intersectionPoint) override {
        return (( static_cast<int>((reference_point.getX() + intersectionPoint.getX()) / length )+
                static_cast<int>((reference_point.getY() + intersectionPoint.getY()) / length ) ) % 2 == 0) ? 
                Color(1, 1, 1) : Color(0, 0, 0);
    }

    double intersect(Ray r, Color clr, int level) override;
    void draw() override;
    bool isPointWithinBounds(Vector3D point) ;
    
    Vector3D getNormalAt(Vector3D intersectionPoint) override {
        return Vector3D(0, 0, 1);
    }

};

#endif // CLASSES_H
