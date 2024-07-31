#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "1905073_classes.hpp"

extern vector<Object*> objects;
extern vector<Light> lights;
extern int recursion_level;

Light::Light()
    : light_pos(Vector3D(0.0, 0.0, 0.0)), color(Color(0.0, 0.0, 0.0)), radius(0.0), segments(0), stacks(0),
      is_SpotLight(false), spotDirection(Vector3D(0.0, 0.0, 0.0)), spotCutoff(360.0) {}

Light::Light(Vector3D l_pos, Color c, double r, int seg, int stck)
    : light_pos(l_pos), color(c), radius(r), segments(seg), stacks(stck), is_SpotLight(false),
      spotDirection(Vector3D(0.0, 0.0, 0.0)), spotCutoff(360.0) {}

void Light::setSpotLight(Vector3D dir, double cutoff) {
    is_SpotLight = true;
    spotDirection = dir;
    spotCutoff = cutoff;
}

bool Light::isSpotLight() const {
    return is_SpotLight;
}

Vector3D Light::getSpotDirection() const {
    return spotDirection;
}

double Light::getSpotCutoff() const {
    return spotCutoff;
}

Color Light::getColor() const {
    return color;
}

Vector3D Light::getLightPos() const {
    return light_pos;
}

void Light::draw() {
    stacks += 1;
    segments += 1;
    Vector3D points[stacks][segments];

    double height, _radius;

    /* generating points: segments = segments in the plane; stacks = segments in hemisphere */
    for (int i = 0; i < stacks; i++) {
        height = radius * std::sin(static_cast<double>(i) / (stacks - 1) * (PI / 2));
        _radius = radius * std::cos(static_cast<double>(i) / (stacks - 1) * (PI / 2));

        for (int j = 0; j < segments; j++) {
            points[i][j] = Vector3D(_radius * std::cos(static_cast<double>(j) / (segments - 1) * 2 * PI),
                                    _radius * std::sin(static_cast<double>(j) / (segments - 1) * 2 * PI), height);
        }
    }

    /* drawing quads using generated points */
    glColor3f(color.getR(), color.getG(), color.getB());

    for (int i = 0; i < (stacks - 1); i++) {
        for (int j = 0; j < (segments - 1); j++) {
            glBegin(GL_QUADS);
            {
                /* upper hemisphere */
                glVertex3f((light_pos + points[i][j]).getX(), (light_pos + points[i][j]).getY(),
                            (light_pos + points[i][j]).getZ());

                glVertex3f((light_pos + points[i][j + 1]).getX(), (light_pos + points[i][j + 1]).getY(),
                            (light_pos + points[i][j + 1]).getZ());

                glVertex3f((light_pos + points[i + 1][j + 1]).getX(), (light_pos + points[i + 1][j + 1]).getY(),
                            (light_pos + points[i + 1][j + 1]).getZ());

                glVertex3f((light_pos + points[i + 1][j]).getX(), (light_pos + points[i + 1][j]).getY(),
                            (light_pos + points[i + 1][j]).getZ());

                /* lower hemisphere */
                glVertex3f((light_pos + points[i][j]).getX(), (light_pos + points[i][j]).getY(),
                            (light_pos - points[i][j]).getZ());

                glVertex3f((light_pos + points[i][j + 1]).getX(), (light_pos + points[i][j + 1]).getY(),
                            (light_pos - points[i][j + 1]).getZ());

                glVertex3f((light_pos + points[i + 1][j + 1]).getX(), (light_pos + points[i + 1][j + 1]).getY(),
                            (light_pos - points[i + 1][j + 1]).getZ());

                glVertex3f((light_pos + points[i + 1][j]).getX(), (light_pos + points[i + 1][j]).getY(),
                            (light_pos - points[i + 1][j]).getZ());
            }
            glEnd();
        }
    }
}


double Object::getLength() {
    return length;
}

void Object::draw() {}

double Object::intersect(Ray r, Color clr, int level) {
    return -1.0;
}

Vector3D Object::getNormalAt(Vector3D intersectionPoint) {
    return Vector3D(0.0, 0.0, 0.0);
}

Color Object::getColorAt(Vector3D intersectionPoint) {
    return color;
}

void Object::setColor(Color c) {
    color = c;
}

void Object::setColor(double c1, double c2, double c3) {
    color.setColor(c1, c2, c3);
}

void Object::setShine(int s) {
    shine = s;
}

void Object::setCoefficients(double c1, double c2, double c3, double c4) {
    coefficients = ReflectionCoefficients(c1, c2, c3, c4);
}

void Object::setCoefficients(ReflectionCoefficients c) {
    coefficients = c;
}

double Object::intersectWithIllumination(Ray& r, Color& clr, int level) {
    double tmin = this->intersect(r, clr, level);

    if(level == 0) return tmin;
    
    Vector3D ro = r.getOrigin(); 
    Vector3D rd = r.getDirection(); 
    Vector3D intersectionPoint = ro + (rd*tmin);

    clr = calculateAmbientColor(intersectionPoint);

    Vector3D normal = calculateAndNormalizeNormal(intersectionPoint);

    handleLightSource(normal, intersectionPoint, clr, tmin, rd);

    if (level >= recursion_level) return tmin;
    
    handleRecursiveReflection(rd, clr, intersectionPoint, normal, level);
    
    return tmin;
}

Color Object::calculateAmbientColor(Vector3D& intersectionPoint) {
    double ambientColorCoefficient = coefficients.getKa();
    Color ambientColor = getColorAt(intersectionPoint)*ambientColorCoefficient;
    ambientColor.fix();
    return ambientColor;
}

Vector3D Object::calculateAndNormalizeNormal(Vector3D& intersectionPoint) {
    Vector3D normal = getNormalAt(intersectionPoint);
    normal.normalize();
    return normal;
}

void Object::handleLightSource(Vector3D& normal, Vector3D& intersectionPoint, Color& clr, double tmin, Vector3D& rd) {
    for (Light& l : lights) {
        Vector3D lightDir = l.getLightPos() - intersectionPoint;
        lightDir.normalize();
        Vector3D lightPos = intersectionPoint + lightDir*0.0000000001;
        Ray lightRay(lightPos, lightDir);

        if (l.isSpotLight()) {
            Vector3D t = intersectionPoint - l.getLightPos();
            Vector3D s = l.getSpotDirection();
            
            double t_length = t.getDistanceVector(Vector3D(0, 0, 0));
            double s_length = s.getDistanceVector(Vector3D(0, 0, 0));

            double angle = (acos(t.dot(s)/(t_length*s_length))) * 180/PI;
            
            if (angle > l.getSpotCutoff()) continue;
        }

        if (!isInShadow(lightRay, tmin)) {
            double lambert, phong;
            calculateLambertAndPhong(normal, lightDir, clr, l, lambert, phong, rd, intersectionPoint);
        }
    }
}

bool Object::isInShadow(Ray& lightRay, double tmin) {
    double t, tMinActual = INFINITY;
    Color tempClr;

    for (const auto& obj : objects) {
        t = obj->intersectWithIllumination(lightRay, tempClr, 0);
        if (t > 0 && t < tMinActual) {
            tMinActual = t;
        }
    }
    if (tmin <= tMinActual) return false;
    else return true;
}

void Object::calculateLambertAndPhong(Vector3D& normal, Vector3D& lightDir, Color& clr, Light& l, double& lambert, double& phong, Vector3D& rd, Vector3D& intersectionPoint) {
    lambert = std::max(normal.dot(lightDir), 0.0);
    Vector3D R = (((normal*2.0)*(normal.dot(lightDir)))-lightDir);
    R.normalize();
    phong = std::max(std::pow(rd.dot(R), shine), 0.0);
    handleDiffuseAndSpecular(clr, l, lambert, phong, intersectionPoint);
}

void Object::handleDiffuseAndSpecular(Color& clr, Light& l, double lambert, double phong, Vector3D& intersectionPoint) {
    Color diffuse = l.getColor()*(coefficients.getKd() * lambert) * (getColorAt(intersectionPoint));
    clr = clr+diffuse;
    clr.fix();
    Color specular = l.getColor() * (coefficients.getKs() * phong);
    clr = clr+specular;
    clr.fix();
}

void Object::handleRecursiveReflection(Vector3D& rd, Color& clr, Vector3D& intersectionPoint, Vector3D& normal, int level) {
    Ray reflectedRay = get_reflectedRay(intersectionPoint, normal, rd);
    handleReflectedColor(reflectedRay, clr, level);
}

Ray Object::get_reflectedRay(Vector3D& intersectionPoint, Vector3D& normal, Vector3D& rd) {
    Vector3D temp_v = (normal*2.0) * (normal.dot(rd));

    Vector3D reflectedRayDir = rd - temp_v;
    reflectedRayDir.normalize();
    Vector3D reflectedRayOrigin = intersectionPoint + (reflectedRayDir * 0.0000000001);
    return Ray(reflectedRayOrigin, reflectedRayDir);
}

void Object::handleReflectedColor(Ray& reflectedRay, Color& clr, int level) {
    Color reflectedColor;
    int nearest = getNearestIntersectingObject(reflectedRay, reflectedColor);

    if (nearest != -1) {
        double reflected_min = objects[nearest]->intersectWithIllumination(reflectedRay, reflectedColor, level + 1);
        clr = clr + reflectedColor*coefficients.getKr();
        clr.fix();
    }
}

int Object::getNearestIntersectingObject(Ray& reflectedRay, Color& reflectedColor) {
        
        int nearest = -1;
        double reflected_min = INFINITY;

        for (int i = 0; i < objects.size(); i++) {
            double t_reflected = objects[i]->intersectWithIllumination(reflectedRay, reflectedColor, 0);
            if(t_reflected>0 && t_reflected < reflected_min){
                nearest = i;
                reflected_min = t_reflected;
            } 
        }

    return nearest;
}

void Floor::draw() {
    double limit = -(reference_point.getX()) / length;

    glBegin(GL_QUADS);
    {
        for (int i = -limit; i < limit; i++) {
            for (int j = -limit; j < limit; j++) {
                double clr=0;
                if((i + j) % 2 == 0)
                    clr=1;

                glColor3f(clr,clr,clr);

                glVertex3f(i * length, j * length, 0);
                glVertex3f((i+1) * length, j * length, 0);
                glVertex3f((i+1) * length, (j+1) * length, 0);
                glVertex3f(i * length, (j+1) * length , 0);
            }
        }
    }
    glEnd();
}


bool Floor::isPointWithinBounds(Vector3D point) {
    return (point.getX() >= reference_point.getX() &&
            point.getX() <= -reference_point.getX() &&
            point.getY() >= reference_point.getY() &&
            point.getY() <= -reference_point.getY());
}


double Floor::intersect(Ray r, Color clr, int level) {
    Vector3D n(0, 0, 1);
    Vector3D ro = r.getOrigin();
    Vector3D rd = r.getDirection();

    double t = -(n.dot(ro) / n.dot(rd));

    Vector3D intersectionPoint = ro + (rd * t);

    if (!isPointWithinBounds(intersectionPoint)) {
        return -1;
    }

    return t;
}


void Sphere::generatePoints(Vector3D points[100][100], int stacks, int slices, double rad) {
    for (int i = 0; i <= stacks; ++i) {
        double h = radius * sin((static_cast<double>(i) / stacks) * (PI / 2));
        double r = radius * cos((static_cast<double>(i) / stacks) * (PI / 2));

        for (int j = 0; j <= slices; ++j) {
            double angle = (static_cast<double>(j) / slices) * 2 * PI;
            points[i][j] = Vector3D(r*cos(angle), r*sin(angle), h);
        }
    }
}

double Sphere::intersect(Ray r, Color clr, int level) {
    Vector3D ro = r.getOrigin()-reference_point;
    Vector3D rd = r.getDirection();

    double a = 1, b = 2 * rd.dot(ro), c = ro.dot(ro) - radius * radius;
    double discr = b * b - 4 * a * c;

    if (discr < 0) return -1;

    double sqrtDiscr = sqrt(discr), t1 = (-b - sqrtDiscr) / (2 * a), t2 = (-b + sqrtDiscr) / (2 * a);

    return (t1 < 0 && t2 < 0) ? -1 : (t1 < 0) ? t2 : t1;

}

void Sphere::draw() {
    glTranslatef(reference_point.getX(), reference_point.getY(), reference_point.getZ());

    const int stacks = 50;
    const int slices = 24;
    Vector3D points[100][100];

    double radius = length;

    generatePoints(points, stacks, slices, radius);

    // draw quads using generated points
    glColor3f(color.getR(), color.getG(), color.getB());
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            glBegin(GL_QUADS); {                
                glVertex3f(points[i][j].getX(), points[i][j].getY(), points[i][j].getZ());    
                glVertex3f(points[i][j + 1].getX(), points[i][j + 1].getY(), points[i][j + 1].getZ());
                glVertex3f(points[i + 1][j + 1].getX(), points[i + 1][j + 1].getY(), points[i + 1][j + 1].getZ());                
                glVertex3f(points[i + 1][j].getX(), points[i + 1][j].getY(), points[i + 1][j].getZ());              
                glVertex3f(points[i][j].getX(), points[i][j].getY(), -points[i][j].getZ());
                glVertex3f(points[i][j + 1].getX(), points[i][j + 1].getY(), -points[i][j + 1].getZ());
                glVertex3f(points[i + 1][j + 1].getX(), points[i + 1][j + 1].getY(), -points[i + 1][j + 1].getZ());
                glVertex3f(points[i + 1][j].getX(), points[i + 1][j].getY(), -points[i + 1][j].getZ());
            } glEnd();
        }
    }
    glTranslatef(-reference_point.getX(), -reference_point.getY(), -reference_point.getZ());
}

Vector3D Sphere::getNormalAt(Vector3D intersectionPoint) {
    Vector3D n = (intersectionPoint-reference_point);
    n.normalize();
    return n;
}

double Triangle::intersect(Ray r, Color clr, int level) {
    Vector3D ro = r.getOrigin();
    Vector3D rd = r.getDirection();

    Vector3D edge1 = v2 - v1;
    Vector3D edge2 = v3 - v1;

    Vector3D h = rd.cross(edge2);
    double a = edge1.dot(h);

    if (a > -EPSILON && a < EPSILON) {
        return -1; // ray is parallel to the triangle
    }

    double f = 1.0 / a;
    Vector3D s = ro - v1;
    double u = f * s.dot(h);

    if (u < 0.0 || u > 1.0) {
        return -1;
    }

    Vector3D q = s.cross(edge1);
    double v = f * rd.dot(q);

    if (v < 0.0 || (u + v) > 1.0) {
        return -1;
    }

    double t = f * edge2.dot(q);

    return (t > EPSILON) ? t : -1;
}

void Triangle::draw() {
    glBegin(GL_TRIANGLES);{
        glColor3f(color.getR(), color.getG(), color.getB());
        glVertex3f(v1.getX(), v1.getY(), v1.getZ());
        glVertex3f(v2.getX(), v2.getY(), v2.getZ());
        glVertex3f(v3.getX(), v3.getY(), v3.getZ());
    }glEnd();
}

Vector3D Triangle::getNormalAt(Vector3D intersectionPoint) {
    Vector3D edge1 = v2 - v1;
    Vector3D edge2 = v3 - v1;
    Vector3D n = edge1.cross(edge2);
    n.normalize();
    return n;
}


double GeneralQuadricSurface::intersect(Ray r, Color clr, int level) {
    Vector3D ro = r.getOrigin();
    Vector3D rd = r.getDirection();

    double a = A * rd.getX() * rd.getX() + B * rd.getY() * rd.getY() + C * rd.getZ() * rd.getZ() +
               D * rd.getX() * rd.getY() + E * rd.getX() * rd.getZ() + F * rd.getY() * rd.getZ();

    double b = 2 * (A * rd.getX() * ro.getX() + B * rd.getY() * ro.getY() + C * rd.getZ() * ro.getZ() +
                   D * (rd.getX() * ro.getY() + rd.getY() * ro.getX()) + E * (rd.getX() * ro.getZ() + rd.getZ() * ro.getX()) +
                   F * (rd.getY() * ro.getZ() + rd.getZ() * ro.getY()) + G * rd.getX() + H * rd.getY() + I * rd.getZ());

    double c = A * ro.getX() * ro.getX() + B * ro.getY() * ro.getY() + C * ro.getZ() * ro.getZ() +
               D * (ro.getX() * ro.getY()) + E * (ro.getX() * ro.getZ()) + F * (ro.getY() * ro.getZ()) +
               G * ro.getX() + H * ro.getY() + I * ro.getZ() + J;

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
        return -1;

    double sqrtDiscriminant = sqrt(discriminant);
    double t1 = (-b - sqrtDiscriminant) / (2 * a);
    double t2 = (-b + sqrtDiscriminant) / (2 * a);

    Vector3D p1 = ro + (rd*t1);
    Vector3D p2 = ro + (rd*t2);

    double t = -1;

    if (t1 > 0 && withinReferenceCube(p1))
        t = t1;
    else if (t2 > 0 && withinReferenceCube(p2))
        t = t2;

    return t;
}

Vector3D GeneralQuadricSurface::getNormalAt(Vector3D intersectionPoint) {
    double x = intersectionPoint.getX();
    double y = intersectionPoint.getY();
    double z = intersectionPoint.getZ();

    Vector3D n(2 * A * x + D * y + E * z + G,
               2 * B * y + D * x + F * z + H,
               2 * C * z + E * x + F * y + I);
    n.normalize();
    return n;
}

bool GeneralQuadricSurface::withinReferenceCube(Vector3D p) {
    if (height != 0 && (p.getZ() < reference_point.getZ() || p.getZ() > reference_point.getZ() + height))
        return false;

    if (length != 0 && (p.getX() < reference_point.getX() || p.getX() > reference_point.getX() + length))
        return false;

    if (width != 0 && (p.getY() < reference_point.getY() || p.getY() > reference_point.getY() + width))
        return false;

    return true;
}

#endif
