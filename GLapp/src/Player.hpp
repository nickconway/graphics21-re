// data and functions to draw a player
#pragma once

#include "Object.hpp"
#include "Island.hpp"

// player object
class Player : public Object {
public:

    float xPos, yPos, zPos;
    float forwardSpeed, strafeSpeed;
    Island* island;

    // create sphere given latitude and longitude sizes and color texture
    Player(int width, int height, glm::vec3 size, std::vector<const char*> textures, Island* i);

    void move(GLapp* app);

    std::vector<float> barycentric(std::vector<glm::vec3> triangle);

    // draw this sphere object
    virtual void draw(GLapp* app, double now) override;
};
