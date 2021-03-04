// implementation code for Sphere object class

// include this class include file FIRST to ensure that it has
// everything it needs for internal self-consistency
#include "Sphere.hpp"

// other classes used directly in the implementation
#include "Ray.hpp"

Sphere::Sphere(const Vec3 &_color, const Vec3 &_center, float _radius)
    : Object(_color) 
{
    C = _center;
    R = _radius;
    Rsquared = R*R;
}

// sphere-ray intersection
const Intersection
Sphere::intersect(const Ray &r) const
{
    // solve p=r.start-center + t*r.direction; p*p -radius^2=0
    float a = r.D_dot_D;
    Vec3 g = r.E - C;
    float b = dot(r.D, g);
    float c = dot(g,g) - Rsquared;

    float discriminant = b*b - a*c;
    if (discriminant < 0)       // no intersection
        return Intersection();

    // solve quadratic equation for desired surface
    float dsq = sqrtf(discriminant);
    float t = (-b - dsq) / a;       // first intersection within ray extent?
    if (t > 0) 
        return Intersection(this,t);

    t = (-b + dsq) / a;             // second intersection within ray extent?
    if (t > 0) 
        return Intersection(this,t);

    return Intersection();              // sphere entirely behind start point
}
