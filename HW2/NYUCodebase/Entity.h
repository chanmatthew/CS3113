#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include "Matrix.h"

class Entity {
public:
    
    Entity(ShaderProgram *program, Matrix &modelMatrix, float width, float height);
    ~Entity();
    
    void Draw();
    
    ShaderProgram *program;
    Matrix& modelMatrix;
    
    float x;
    float y;
    float rotation;
    
    int textureID;
    
    float width;
    float height;
    
    float speed;
    float direction_x;
    float direction_y;
};