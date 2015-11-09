
#include "ClassDemoApp.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = clearer
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    SDL_FreeSurface(surface);
    
    return textureID;
}

void ClassDemoApp::DrawText(int fontTexture, std::string text, float size, float spacing, float x, float y) {
    float texture_size = 1.0 / 16.0f;
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    
    modelMatrix.identity();
    
    modelMatrix.Translate(x, y, 0.0f);
    program->setModelMatrix(modelMatrix);
    
    for (int i = 0; i < text.size(); i++) {
        float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
        float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
        vertexData.insert(vertexData.end(), {
            ((size + spacing) * i) + (-0.5f * size), 0.5f * size,
            ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
            ((size + spacing) * i) + (0.5f * size), 0.5f * size,
            ((size + spacing) * i) + (0.5f * size), -0.5f * size,
            ((size + spacing) * i) + (0.5f * size), 0.5f * size,
            ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
        });
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        });
    }
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

bool ClassDemoApp::isDone() {
    return done;
}

ClassDemoApp::ClassDemoApp() :
lastFrameTicks(0.0f), timeLeftOver(0.0f), done(false)
{
    Setup();
}

void ClassDemoApp::Setup() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 400, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 800, 400);
    
    projectionMatrix.setOrthoProjection(-8.0f, 8.0f, -4.0f, 4.0f, -1.0f, 1.0f);
    
    program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    blocksTexture = LoadTexture(RESOURCE_FOLDER"arne_sprites.png");
    playerTexture = LoadTexture(RESOURCE_FOLDER"complete.png");
    
    player = new Entity();
    player->position_x = 11.0f;
    player->position_y = -5.0f;
    player->sprite = SheetSprite(playerTexture, 426.0f/1024.0f, 132.0f/512.0f, 32.0f/1024.0f, 66.0f/512.0f, 1.0f);
//    player->hitboxHeight = player->height * 0.3f;
//    player->hitboxWidth = player->width * 0.3f;
    player->friction_x = 2.0f;
    player->friction_y = 2.0f;
    player->isStatic = false;
    entities.push_back(player);
}

ClassDemoApp::~ClassDemoApp() {
    delete program;
    
    SDL_Quit();
}

bool ClassDemoApp::UpdateAndRender() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float elapsed = ticks - lastFrameTicks;
    lastFrameTicks = ticks;
    
    float fixedElapsed = elapsed + timeLeftOver;
    if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
        fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
    }
    while (fixedElapsed >=FIXED_TIMESTEP) {
        fixedElapsed -= FIXED_TIMESTEP;
        Update(FIXED_TIMESTEP);
    }
    timeLeftOver = fixedElapsed;
    
    ProcessEvents();
//    Update(elapsed);
    Render();
    
    return done;
}

void ClassDemoApp::ProcessEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
    }
    
    program->setModelMatrix(modelMatrix);
    program->setViewMatrix(viewMatrix);
    program->setProjectionMatrix(projectionMatrix);
    
    glUseProgram(program->programID);
    
    SDL_GL_SwapWindow(displayWindow);
}


void ClassDemoApp::Render() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    viewMatrix.identity();
    
    buildLevel();
    renderTilemap();
    
    viewMatrix.Translate(-player->position_x, -player->position_y, 0);
    player->Draw(program);
}


void ClassDemoApp::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
    *gridX = (int)(worldX / TILE_SIZE);
    *gridY = (int)(-worldY / TILE_SIZE);
}

bool ClassDemoApp::checkIfSolid(int gridX, int gridY, unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
    if (gridX >= 0 && gridY >= 0 && (levelData[gridY][gridX] == 4 ||
                                     levelData[gridY][gridX] == 6 ||
                                     levelData[gridY][gridX] == 2 ||
                                     levelData[gridY][gridX] == 3 ||
                                     levelData[gridY][gridX] == 32 ||
                                     levelData[gridY][gridX] == 33 ||
                                     levelData[gridY][gridX] == 34 ||
                                     levelData[gridY][gridX] == 35 ||
                                     levelData[gridY][gridX] == 100 ||
                                     levelData[gridY][gridX] == 101)) {
        return true;
    }
    return false;
}

void ClassDemoApp::Update(float elapsed) {
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        player->acceleration_x = -10.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        player->acceleration_x = 10.0f;
    }
    else {
        player->acceleration_x = 0.0f;
    }
    if (keys[SDL_SCANCODE_UP] && player->collidedBottom) {
        player->velocity_y = 10.0f;
        player->collidedBottom = false;
    }
    
    for (size_t i = 0; i < entities.size(); i++)
        if (!entities[i]->isStatic) {
            
            entities[i]->Update(elapsed);
            
            int gridX = 0, gridY = 0;
            
            worldToTileCoordinates(entities[i]->position_x - entities[i]->width / 2, entities[i]->position_y, &gridX, &gridY);
            if (checkIfSolid(gridX, gridY, levelData)) {
                entities[i]->position_x = (gridX + 1) * TILE_SIZE + entities[i]->width / 2 + 0.005f;
                entities[i]->velocity_x = 0;
                entities[i]->collidedLeft = true;
            }
            
            worldToTileCoordinates(entities[i]->position_x + entities[i]->width / 2, entities[i]->position_y, &gridX, &gridY);
            if (checkIfSolid(gridX, gridY, levelData)) {
                entities[i]->position_x = gridX * TILE_SIZE - entities[i]->width / 2 - 0.005f;
                entities[i]->velocity_x = 0;
                entities[i]->collidedRight = true;
            }
            
            worldToTileCoordinates(entities[i]->position_x, entities[i]->position_y + entities[i]->height / 2, &gridX, &gridY);
            if (checkIfSolid(gridX, gridY, levelData)) {
                entities[i]->position_y = (-gridY - 1) * TILE_SIZE - entities[i]->height / 2 - 0.005f;
                entities[i]->velocity_y = 0;
                entities[i]->collidedTop = true;
            }
            
            worldToTileCoordinates(entities[i]->position_x, entities[i]->position_y - entities[i]->height / 2, &gridX, &gridY);
            if (checkIfSolid(gridX, gridY, levelData)) {
                entities[i]->position_y = -gridY * TILE_SIZE + entities[i]->height / 2 + 0.005f;
                entities[i]->velocity_y = 0;
                entities[i]->collidedBottom = true;
            }
        }
}

void ClassDemoApp::buildLevel() {
    memcpy(levelData, level1Data, LEVEL_HEIGHT*LEVEL_WIDTH);
}

void ClassDemoApp::renderTilemap() {
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    
    for (int y = 0; y < LEVEL_HEIGHT; y++) {
        for (int x = 0; x < LEVEL_WIDTH; x++) {
            
            float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X;
            float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y;
            
            float spriteWidth = 1.0f / (float)SPRITE_COUNT_X;
            float spriteHeight = 1.0f / (float)SPRITE_COUNT_Y;
            
            vertexData.insert(vertexData.end(), {
                
                TILE_SIZE * x, -TILE_SIZE * y,
                TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
                (TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
                
                TILE_SIZE * x, -TILE_SIZE * y,
                (TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
                (TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
            });
            
            texCoordData.insert(texCoordData.end(), {
                u, v,
                u, v + spriteHeight,
                u + spriteWidth, v + spriteHeight,
                
                u, v,
                u + spriteWidth, v + spriteHeight,
                u + spriteWidth, v
            });
        }
    }
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
//    modelMatrix.identity();
//    program->setModelMatrix(modelMatrix);
    
    glBindTexture(GL_TEXTURE_2D, blocksTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6 * LEVEL_HEIGHT * LEVEL_WIDTH);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}