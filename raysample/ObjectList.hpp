// lists of objects in scene
#ifndef OBJECTLIST_HPP
#define OBJECTLIST_HPP

// other classes we use DIRECTLY in our interface
#include "Intersection.hpp"
#include "Ray.hpp"

// system includes
#include <vector>

// classes we only use by pointer or reference
class Object;

class ObjectList {
public: // data
    // list of objects
    typedef std::vector<Object*> ObjList;
    ObjList objects;

public: // constructor & destructor
    ObjectList() {}
    ~ObjectList();

public:
    // Add an object to the list. Objects should be allocated with
    // new. Objects will be deleted when this ObjectList is destroyed
    void addObject(Object *obj) { objects.push_back(obj); }

public: // computational members
    // trace ray r through all objects, returning first intersection
    const Intersection trace(Ray r) const;

    // trace ray r through all objects, returning true if there is an
    // interesction between r.near and r.far
    const bool probe(Ray r) const;
};

#endif
