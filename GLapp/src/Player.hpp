// data and functions to draw a player
#pragma once

#include "Object.hpp"

// player object
class Player : public Object {
public:

    int xPos, yPos, zPos;
    int xRate, yRate, zRate;

    // create sphere given latitude and longitude sizes and color texture
    Player(int width, int height, glm::vec3 size, const char* texturePPM);

    void updatePos(GLapp* app);

    // draw this sphere object
    virtual void draw(GLapp* app, double now) override;
};
