
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

#include "SheetSprite.h"

#include <vector>

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
    
    void Update(float elapsed);
    
    void ProcessEvents();
    
    void RenderMainMenu();
    void RenderGameLevel();
    void RenderGameOver();
    
    void UpdateGameLevel(float elapsed);
    
    void shoot();
    
    bool UpdateAndRender();
    
    void DrawText(int fontTexture, std::string text, float size, float spacing, float x, float y);
    
    bool isDone();
            
private:
    
    ShaderProgram *program;
    
    SDL_Event event;
    
    SDL_Window* displayWindow;
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    float lastFrameTicks;
    
    Entity *player;
    
    std::vector<Entity> enemies;
    Entity bullet;
    
    GLuint spritesTexture;
    GLuint fontTexture;
    
    int state;
    
    int score;
    
    int enemyDirection;
    int numOfEnemies;
    
    float timeCount;
    
    bool enemyHitWall;
    bool moveEnemyDown;
    
    bool done = false;
    
};