
#include "ClassDemoApp.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_OVER };

int state;

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
lastFrameTicks(0.0f), done(false)
{
    Setup();
}

void ClassDemoApp::Setup() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 360);
    
    projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0, 2.0, -1.0, 1.0);
    
    program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    spritesTexture = LoadTexture(RESOURCE_FOLDER"complete.png");
    fontTexture = LoadTexture(RESOURCE_FOLDER"pixel_font.png");
        
    RenderMainMenu();
}

ClassDemoApp::~ClassDemoApp() {
    delete program;
    
    SDL_Quit();
}

bool ClassDemoApp::UpdateAndRender() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float elapsed = ticks - lastFrameTicks;
    lastFrameTicks = ticks;
    
    ProcessEvents();
    Update(elapsed);
    
    return done;
}

void ClassDemoApp::ProcessEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && state == STATE_GAME_LEVEL) {
                shoot();
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                if (state == STATE_GAME_OVER) {
                    RenderMainMenu();
                }
                else if (state == STATE_MAIN_MENU) {
                    RenderGameLevel();
                }
            }
        }
    }
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    program->setModelMatrix(modelMatrix);
    program->setViewMatrix(viewMatrix);
    program->setProjectionMatrix(projectionMatrix);
    
    glUseProgram(program->programID);
    
    switch (state) {
        case STATE_MAIN_MENU:
            DrawText(fontTexture, "SPACE INVADERS", 0.3, 0, -1.9f, 0.7f);
            DrawText(fontTexture, "PRESS ENTER TO BEGIN", 0.1, 0, -1.0f, -0.5f);
            break;
            
        case STATE_GAME_LEVEL:
            player->Draw(program);
            bullet.Draw(program);
            DrawText(fontTexture, "SCORE: " + std::to_string(score), 0.1, 0, -0.37f, 1.8f);
            for (Entity& enemy : enemies) {
                enemy.Draw(program);
            }
            break;
            
        case STATE_GAME_OVER:
            DrawText(fontTexture, "SCORE: " + std::to_string(score), 0.1, 0, -0.37f, 1.8f);
            DrawText(fontTexture, "GAMEOVER", 0.1, 0, -0.37f, 0.9f);
            DrawText(fontTexture, "PRESS ENTER TO CONTINUE", 0.1, 0, -1.1f, 0.0f);
            for (Entity& enemy : enemies) {
                enemy.Draw(program);
            }
            break;
    }
    
    SDL_GL_SwapWindow(displayWindow);
}

void ClassDemoApp::RenderMainMenu() {
    state = STATE_MAIN_MENU;
}

void ClassDemoApp::RenderGameLevel() {
    state = STATE_GAME_LEVEL;
    
    score = 0;
    
    timeCount = 0;
    enemyDirection = 1.0f;
    moveEnemyDown = false;
    enemyHitWall = false;
    
    player = new Entity();
    player->x = -0.1f;
    player->y = -2.0f + (player->height * 0.3f) / 2.0f;
    player->sprite = SheetSprite(spritesTexture, 426.0f/1024.0f, 132.0f/512.0f, 32.0f/1024.0f, 66.0f/512.0f, 0.3f);
    player->hitboxHeight = player->height * 0.3f;
    player->hitboxWidth = player->width * 0.3f;
    
    enemies.clear();
    
    for (int row = 0; row < 3; row++) {
        for (int column = 0; column < 6; column++) {
            Entity enemy;
            enemy.x = -3.0f + (column * 0.75f);
            enemy.y = 1.4f - (row * 0.5f);
            enemy.sprite = SheetSprite(spritesTexture, 420.0f/1024.0f, 66.0f/512.0f, 40.0f/1024.0f, 66.0f/512.0f, 0.3f);
            enemy.hitboxHeight = enemy.height * 0.3f;
            enemy.hitboxWidth = enemy.width * 0.3f;
            enemies.push_back(enemy);
            numOfEnemies++;
        }
    }
    
    bullet = Entity();
    bullet.active = false;
    bullet.sprite = SheetSprite(spritesTexture, 183.0f/1024.0f, 467.0f/512.0f, 12.0f/1024.0f, 11.0f/512.0f, 0.3f);
    bullet.hitboxHeight = bullet.height * 0.3f;
    bullet.hitboxWidth = bullet.width * 0.3f;
}

void ClassDemoApp::RenderGameOver() {
    state = STATE_GAME_OVER;
}

void ClassDemoApp::Update(float elapsed) {
    switch (state) {
        case STATE_MAIN_MENU:
            break;
            
        case STATE_GAME_LEVEL:
            UpdateGameLevel(elapsed);
            break;
            
        case STATE_GAME_OVER:
            break;
    }
}

void ClassDemoApp::UpdateGameLevel(float elapsed) {
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT]) {
        player->x -= elapsed * 6.0f;
        if( player->x < -3.55f + player->width / 2.0f) {
            player->x = -3.55f + player->width / 2.0f;
        }
    }
    if (keys[SDL_SCANCODE_RIGHT]) {
        player->x += elapsed * 6.0f;
        if (player->x > 3.55f - player->width / 2.0f) {
            player->x = 3.55f - player->width / 2.0f;
        }
    }
    
    timeCount += elapsed;
    
    if (timeCount > 0.15f) {
        for (Entity& enemy : enemies) {
            if (enemy.active) {
                if (player->collidesWith(enemy)) {
                    RenderGameOver();
                }
                if (moveEnemyDown) {
                    enemy.y -= 0.3f;
                }
                else {
                    enemy.x += 0.1f * enemyDirection;
                    if (enemy.x < -3.0f || enemy.x > 3.0f) {
                        enemyHitWall = true;
                    }
                }
            }
        }
        moveEnemyDown = false;
        if (enemyHitWall) {
            moveEnemyDown = true;
            enemyDirection *= -1.0f;
            enemyHitWall = false;
        }
        timeCount -= 0.15f;
    }
    
    if (bullet.active) {
        bullet.y += 10.0f * elapsed;
        if (bullet.y > 2.0f) {
            bullet.active = false;
        }
        else {
            for (Entity& enemy : enemies) {
                if (bullet.collidesWith(enemy)) {
                    enemy.active = false;
                    bullet.active = false;
                    score++;
                    numOfEnemies--;
                    if (numOfEnemies <= 0) {
                        RenderMainMenu();
                    }
                    break;
                }
            }
        }
    }
    
}

void ClassDemoApp::shoot() {
    if (!bullet.active) {
        bullet.active = true;
        bullet.x = player->x;
        bullet.y = player->y;
    }
}