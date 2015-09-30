
#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include "Matrix.h"

#include "Entity.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class ClassDemoApp {
public:
    
    ClassDemoApp();
    ~ClassDemoApp();
    
    void Setup();
    
    void ProcessEvents();
    void Render();
    void Update(float elapsed);
    
    bool UpdateAndRender();
    
    bool isDone();
        
private:
    
    ShaderProgram *program;
    
    SDL_Event event;
    
    SDL_Window* displayWindow;
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    float lastFrameTicks;
    float angle;

    int p1Wins;
    int p2Wins;
    
    Entity *topWall;
    Entity *botWall;
    Entity *midLine;
    
    Entity *paddle1;
    Entity *paddle2;
    Entity *ball;
    
    bool done = false;
    
};