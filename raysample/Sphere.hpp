// sphere objects
#ifndef SPHERE_HPP
#define SPHERE_HPP

// other classes we use DIRECTLY in the interface
#include "Object.hpp"
#include "Vec3.hpp"

// classes we only use by pointer or reference
class World;
class Ray;

// sphere objects
class Sphere : public Object {
    Vec3 C;
    float R;

    // derived, for intersection testing
    float Rsquared;

public: // constructors
    Sphere(const Vec3 &_color, const Vec3 &_center, float _radius);

public: // object functions
    const Intersection intersect(const Ray &ray) const;
};

#endif
