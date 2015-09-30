
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
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST = clearer
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    SDL_FreeSurface(surface);
    
    return textureID;
}

bool ClassDemoApp::isDone() {
    return done;
}

ClassDemoApp::ClassDemoApp() :
lastFrameTicks(0.0f), angle(0.0f), p1Wins(0), p2Wins(0), done(false)
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
    
    program = new ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    topWall = new Entity(program, modelMatrix, 7.108f, 0.025f);
    botWall = new Entity(program, modelMatrix, 7.108f, 0.025f);
    midLine = new Entity(program, modelMatrix, 0.025f, 4.0f);
    
    topWall->y = 2.0f - topWall->height / 2;
    botWall->y = -2.0f + topWall->height / 2;
    
    paddle1 = new Entity(program, modelMatrix, 0.2f, 1.25f);
    paddle2 = new Entity(program, modelMatrix, 0.2f, 1.25f);
    ball = new Entity(program, modelMatrix, 0.2f, 0.2f);
    
    paddle1->x = -3.554f + paddle1->width / 2;
    paddle2->x = 3.554f - paddle1->width / 2;
    
    paddle1->speed = 4.4f;
    paddle2->speed = 4.4f;
    
    ball->speed = 3.0f;
    
    ball->direction_x = 1.0f;
    ball->direction_y = 1.0f;
}

ClassDemoApp::~ClassDemoApp() {
    delete program;
    
    SDL_Quit();
}

void ClassDemoApp::ProcessEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
    }
}

void ClassDemoApp::Render() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    modelMatrix.identity();
    
    program->setModelMatrix(modelMatrix);
    program->setViewMatrix(viewMatrix);
    program->setProjectionMatrix(projectionMatrix);
    
    glUseProgram(program->programID);
    
    topWall->Draw();
    botWall->Draw();
    midLine->Draw();
    
    paddle1->Draw();
    paddle2->Draw();
    ball->Draw();
    
    SDL_GL_SwapWindow(displayWindow);
}

void ClassDemoApp::Update(float elapsed) {
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_UP]) {
        paddle2->y += elapsed * paddle2->speed;
    }
    if (keys[SDL_SCANCODE_DOWN]) {
        paddle2->y -= elapsed * paddle2->speed;
    }
    if (keys[SDL_SCANCODE_A]) {
        paddle1->y += elapsed * paddle2->speed;
    }
    if (keys[SDL_SCANCODE_Z]) {
        paddle1->y -= elapsed * paddle2->speed;
    }
    
    ball->x += elapsed * ball->direction_x * ball->speed;
    ball->y += elapsed * ball->direction_y * ball->speed;
    
    // Check for collision between paddle and screen edge
    if (paddle1->y > 2.0f - paddle1->height / 2) {
        paddle1->y = 2.0f - paddle1->height / 2;
    }
    else if (paddle1->y < -2.0f + paddle1->height / 2) {
        paddle1->y = -2.0f + paddle1->height / 2;
    }
    if (paddle2->y > 2.0f - paddle2->height / 2) {
        paddle2->y = 2.0f - paddle2->height / 2;
    }
    else if (paddle2->y < -2.0f + paddle2->height / 2) {
        paddle2->y = -2.0f + paddle2->height / 2;
    }
    
    // Check for collision between ball and screen edge
    if (ball->x > 3.55f - ball->width / 2) {
        ball->direction_x = -1;
        ball->x = 0.0f;
        ball->y = 0.0f;
        p1Wins += 1;
    }
    else if (ball->x < -3.55f + ball->width / 2) {
        ball->direction_x = 1;
        ball->x = 0.0f;
        ball->y = 0.0f;
        p2Wins += 1;
    }
    
    if (ball->y > 2.0f - ball->height / 2) {
        ball->direction_y = -1.0f;
    }
    else if (ball->y < -2.0f + ball->height / 2) {
        ball->direction_y = 1.0f;
    }
    
    // Check for collision between paddles and ball
    if (!(paddle1->y - paddle1->height / 2 > ball->y + ball->height / 2 ||
          paddle1->y + paddle1->height / 2 < ball->y - ball->height / 2 ||
          paddle1->x - paddle1->width / 2 > ball->x + ball->width / 2 ||
          paddle1->x + paddle1->width / 2 < ball->x - ball->width / 2)) {
        ball->direction_x = 1.0f;
    }
    if (!(paddle2->y - paddle2->height / 2 > ball->y + ball->height / 2 ||
          paddle2->y + paddle2->height / 2 < ball->y - ball->height / 2 ||
          paddle2->x - paddle2->width / 2 > ball->x + ball->width / 2 ||
          paddle2->x + paddle2->width / 2 < ball->x - ball->width / 2)) {
        ball->direction_x = -1.0f;
    }
}

bool ClassDemoApp::UpdateAndRender() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float elapsed = ticks - lastFrameTicks;
    lastFrameTicks = ticks;
    
    ProcessEvents();
    Update(elapsed);
    Render();
    
    return done;
}