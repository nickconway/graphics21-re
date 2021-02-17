// virtual class for any object
#ifndef OBJECT_HPP
#define OBJECT_HPP

// other classes we use DIRECTLY in our interface
#include "Intersection.hpp"
#include "Vec3.hpp"

// classes we only use by pointer or reference
class World;
class Ray;

class Object {
public: // public data
    Vec3 color;         // this object's color

public: // constructor & destructor
    Object();
    Object(const Vec3 &_color);
    virtual ~Object();


public: // computational members
    // return t for closest intersection with ray
    virtual const Intersection intersect(const Ray &ray) const = 0;
};

#endif
