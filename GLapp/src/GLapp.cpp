//
// Simple GL example
//


#include "GLapp.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Island.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <assert.h>

#ifndef F_PI
#define F_PI 3.1415926f
#endif
#include <iostream>

using namespace glm;  // avoid glm:: for all glm types and functions

int subdivisions = 0;
Island* island;

///////
// GLFW callbacks must use extern "C"
extern "C" {
    // called for GLFW error
    void error(int error, const char *description) {
        fprintf(stderr, "GLFW error %d: %s\n", error, description);
    }

    // called whenever the window size changes
    void reshape(GLFWwindow *win, int width, int height) {
        // save window dimensions
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        glfwGetFramebufferSize(win, &app->width, &app->height);

        // viewport size matches window size
        glViewport(0, 0, app->width, app->height);
    }

    // called when mouse button is pressed
    void mousePress(GLFWwindow *win, int button, int action, int mods) {
        if (button != GLFW_MOUSE_BUTTON_LEFT) return;

        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        app->button = (action == GLFW_PRESS);
        glfwSetInputMode(win, GLFW_CURSOR, app->button ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        glfwGetCursorPos(win, &app->mouseX, &app->mouseY);
    }

    // called when mouse is moved
    void mouseMove(GLFWwindow *win, double x, double y) {
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        if (!app->button) return;

        // rotation angle, scaled so across the window = one rotation
        app->pan += float(F_PI * float(x - app->mouseX) / app->width);
        app->tilt += float(0.5f*F_PI * float(y - app->mouseY) / app->height);

        // remember location so next update will be relative to this one
        app->mouseX = x;
        app->mouseY = y;
    }

    // called on any keypress
    void keyPress(GLFWwindow *win, int key, int scancode, int action, int mods) {
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        int level = island->level;

        if (action == GLFW_PRESS) {
            switch (key) {
            case 'A':                   // rotate left
                app->panRate = -F_PI;  // half a rotation/sec
                return;

            case 'D':                   // rotate right
                app->panRate = F_PI;   // half a rotation/sec
                return;

            case 'W':                   // rotate up
                app->tiltRate = 0.5f * F_PI; // 1/4 rotation/sec
                return;

            case 'S':                   // rotate down
                app->tiltRate = -0.5f * F_PI; // 1/4 rotation/sec
                return;

            case 'R':                   // reload shaders
                for (auto object : app->objects)
                    object->updateShaders();
                return;

            case 'I':                   // cycle through ambient intensity
                app->scene.LightDir.a += 0.2f;
                if (app->scene.LightDir.a > 1.f)
                    app->scene.LightDir.a = 0.f;
                return;

            case 'L':                   // toggle lines or solid
                app->wireframe = !app->wireframe;
                glPolygonMode(GL_FRONT_AND_BACK, app->wireframe ? GL_LINE : GL_FILL);
                return;

            case '=':
                if (mods == 1) {
                    island->addSubdivision();
                }
                return;

            case '-':
                if (island->level != 0) {
                    island->removeSubdivision();
                }
                return;

            case 'N':
                island->reset();
                for (int i = 0; i < level; i++) {
                    island->addSubdivision();
                }
                return;

            case GLFW_KEY_ESCAPE:                    // Escape: exit
                glfwSetWindowShouldClose(win, true);
                return;
            }
        }

        if (action == GLFW_RELEASE) {
            switch (key) {
            case 'A': case 'D':         // stop panning
                app->panRate = 0;
                return;
            case 'W': case 'S':         // stop tilting
                app->tiltRate = 0;
                return;
            }
        }
    }
}

// initialize GLFW - windows and interaction
GLapp::GLapp()
{
    // member data initialization
    width = 843; height = 480;                  // window size
    distance = 500.f; pan = 0.f; tilt = -1.4f;  // view
    panRate = tiltRate = 0.f;                   // keyboard view control
    button = false; mouseX = mouseY = 0.f;      // mouse view controls
    wireframe = false;                          // solid drawing

    // set error callback before init
    glfwSetErrorCallback(error);
    int ok = glfwInit();
    assert(ok);

    // OpenGL version: YOU MAY NEED TO ADJUST VERSION OR OPTIONS!
    // When figuring out the settings that will work for you, make
    // sure you can see error messages on console output.
    //
    // My driver needs FORWARD_COMPAT, but others will need to comment that out.
    // Likely changes for other versions:
    //   All versions: change VERSION_MAJOR and VERSION_MINOR
    //   OpenGL 3.0 (2008): does not support features we need
    //   OpenGL 3.1 (2009):
    //     comment out GLFW_OPENGL_PROFILE line
    //     Use "140" for the "#version" line in the .vert and .frag files
    //   OpenGL 3.2 (2009): Use "150 core" for the "#version" line in the .vert and .frag files
    //   OpenGL 3.3 (2010): Use "330 core" for the "#version" line in the .vert and .frag files
    //   Any of 4.0-4.6:
    //     Similar to 3.3: #version line in shaders uses <MAJOR><MINOR>0
    //     For example, 4.6 is "#version 460 core" 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // ask for a window with dimensions 843 x 480 (HD 480p)
    win = glfwCreateWindow(width, height, "Simple OpenGL Application", 0, 0);
    assert(win);

    glfwMakeContextCurrent(win);

    // GLEW handles OpenGL shared library access
    glewExperimental = true;
    glewInit();

    // set callback functions to be called by GLFW
    glfwSetWindowUserPointer(win, this);
    glfwSetFramebufferSizeCallback(win, reshape);
    glfwSetKeyCallback(win, keyPress);
    glfwSetMouseButtonCallback(win, mousePress);
    glfwSetCursorPosCallback(win, mouseMove);

    // tell OpenGL to enable z-buffer for overlapping surfaces
    glEnable(GL_DEPTH_TEST);

    // initialize buffer for scene shader data
    glGenBuffers(1, &sceneUniformsID);
    glBindBuffer(GL_UNIFORM_BUFFER, sceneUniformsID);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneData), 0, GL_STREAM_DRAW);

    // initialize scene data
    scene.LightDir = vec4(-1,-2,2,0);
}

///////
// Clean up any context data
GLapp::~GLapp() 
{
    glfwDestroyWindow(win);
    glfwTerminate();
}

// call before drawing each frame to update per-frame scene state
void GLapp::sceneUpdate(double dTime)
{
    pan += float(panRate * dTime);
    tilt += float(tiltRate * dTime);

    scene.ProjFromWorld = 
        perspective(F_PI/4.f, (float)width/height, 1.f, 10000.f)
        * translate(mat4(1), vec3(0,0,-distance))
        * rotate(mat4(1), tilt, vec3(1,0,0))
        * rotate(mat4(1), pan, vec3(0,0,1));
    scene.WorldFromProj = inverse(scene.ProjFromWorld);

    glBindBuffer(GL_UNIFORM_BUFFER, sceneUniformsID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SceneData), &scene);
}

// render a frame
void GLapp::render()
{
    // consistent time for drawing this frame
    double currTime = glfwGetTime();
    double dTime = currTime - prevTime;

    // clear old screen contents to a sky blue
    glClearColor(0.5, 0.7, 0.9, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw all objects
    sceneUpdate(dTime);
    for (auto object : objects)
        object->draw(this, currTime);

    // show what we drew
    glfwSwapBuffers(win);
    prevTime = currTime;
}

int main(int argc, char *argv[])
{
    // initialize windows and OpenGL
    GLapp app;

    // add some objects to draw
    app.objects.push_back(new Plane(vec3(50000.f, 50000.f, 100.f), "water.ppm"));
    //app.objects.push_back(new Plane(vec3(500.f, 500.f, 100.f), "rocks-color.ppm"));

    island = new Island(vec3(500.f, 500.f, 100.f), "rocks-color.ppm");
    app.objects.push_back(island);
    app.objects.push_back(new Sphere(50, 25, vec3(50.f,50.f,50.f), "paving-color.ppm"));

    // set up initial viewport
    reshape(app.win, app.width, app.height);

    // each frame: render then check for events
    while (!glfwWindowShouldClose(app.win)) {
        app.render();
        glfwPollEvents();
    }

    return 0;
}
