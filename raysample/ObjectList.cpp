// implementation code for ObjectList class
// list of objects in scene

// include this class include file FIRST to ensure that it has
// everything it needs for internal self-consistency
#include "ObjectList.hpp"
#include "Object.hpp"
#include <iostream>
#include <atomic>

static std::atomic<int> RayCount(0), ShadowCount(0);

// delete list and objects it contains
ObjectList::~ObjectList() {
    std::cout << RayCount << " Photon Ray" << (RayCount == 1 ? "" : "s") << "; "
        << ShadowCount << " Shadow Ray" << (ShadowCount == 1 ? "" : "s") << '\n';
    for(auto obj : objects)
        delete obj;
}

// trace ray r through all objects, returning first intersection
const Intersection
ObjectList::trace(Ray r) const
{
    ++RayCount;
    Intersection closest;       // no object, t = infinity
    for(auto obj : objects) {
        Intersection current = obj->intersect(r);
        if (current < closest)
            closest = current;
    }
    return closest;
}

// trace ray r through all objects, returning true if there is any
// intersection between r.near and r.far
const bool
ObjectList::probe(Ray r) const
{
    ++ShadowCount;
    for(auto obj : objects) {
        if (obj->intersect(r).t < r.far)
            return true;
    }
    return false;
}

