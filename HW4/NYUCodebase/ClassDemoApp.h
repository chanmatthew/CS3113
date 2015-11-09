
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

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

#define LEVEL_HEIGHT 16
#define LEVEL_WIDTH 22

#define TILE_SIZE 1.0f

#define SPRITE_COUNT_X 16
#define SPRITE_COUNT_Y 8

class ClassDemoApp {
public:
    
    ClassDemoApp();
    ~ClassDemoApp();
    
    void DrawText(int fontTexture, std::string text, float size, float spacing, float x, float y);
    
    void Setup();
    
    bool UpdateAndRender();
    void ProcessEvents();
    
    void Render();
    
    void Update(float elapsed);
    
    void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);
    bool checkIfSolid(int gridX, int gridY, unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH]);
    void buildLevel();
    void renderTilemap();
    
    bool isDone();
    
private:
    
    ShaderProgram *program;
    
    SDL_Event event;
    
    SDL_Window* displayWindow;
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    float lastFrameTicks;
    float timeLeftOver;
    
    Entity *player;
    std::vector<Entity*> entities;
    
    GLuint blocksTexture;
    GLuint playerTexture;
    
    float timeCount;
    
    unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH];
    
    unsigned char level1Data[LEVEL_HEIGHT][LEVEL_WIDTH] =
    {
        {90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90},
        {90,4,4,4,4,4,4,4,90,90,90,90,90,90,4,4,4,4,4,4,4,90},
        {90,4,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,4,90},
        {90,4,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,4,90},
        {90,4,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,4,90},
        {90,4,90,90,90,90,90,6,6,6,6,6,6,6,6,90,90,90,90,90,4,90},
        {90,4,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,4,90},
        {90,4,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,4,90},
        {90,4,6,6,6,6,6,90,90,90,90,90,90,90,90,6,6,6,6,6,4,90},
        {90,4,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,4,90},
        {90,4,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,4,90},
        {90,4,90,90,90,90,90,6,6,6,6,6,6,6,6,90,90,90,90,90,4,90},
        {90,4,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,4,90},
        {90,4,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,4,90},
        {2,2,2,2,2,2,2,2,2,3,90,90,3,2,2,2,2,2,2,2,2,2},
        {32,33,33,34,32,33,33,34,33,35,100,101,35,32,33,32,34,32,33,32,33,33},
    };
    
//    int mapHeight;
//    int mapWidth;
    
    bool done = false;
    
};