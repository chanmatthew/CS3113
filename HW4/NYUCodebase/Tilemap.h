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

#include <vector>

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#define LEVEL_HEIGHT 32
#define LEVEL_WIDTH 128
#define TILE_SIZE 1.0f

using namespace std;

class Tilemap {
public:
    Tilemap();
    Tilemap( string& path, GLuint textureID, ShaderProgram* program );
    unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH];
    vector<char> airTiles;
    
    int SPRITE_COUNT_X = 16;
    int SPRITE_COUNT_Y = 8;
    
    float playerX = -6;
    float playerY = 15;
    
    void ReadFromFile( const std::string& path );
    void PrepareLevel();
    void Render();
    void placeEntity(std::string& type, float x, float y);

private:
    GLuint textureID;
    ShaderProgram* program;
    bool readLayerData(ifstream &stream);
    bool readEntityData(ifstream &stream);
    vector<float> vertexData;
    vector<float> texCoordData;
    int numVerts;
    Entity* player;
    
};