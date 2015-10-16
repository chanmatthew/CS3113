#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include "Matrix.h"

#include "SheetSprite.h"

class Entity {
public:
    
    Entity();
//    ~Entity();
    
    void Draw(ShaderProgram* program);
    
    float x;
    float y;
    float rotation;
    
    int textureID;
    
    float width;
    float height;
    
    float speed;
    float direction_x;
    float direction_y;
    
    bool active;
    
    bool collidesWith(Entity& object);
    
    float velocityX;
    float velocityY;
    
    float hitboxWidth;
    float hitboxHeight;
    
    SheetSprite sprite;
};