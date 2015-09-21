#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include "Matrix.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    SDL_FreeSurface(surface);
    
    return textureID;
}

void DrawSprite(GLint textureID, float x, float y, float xscale, float yscale, float rotation) {
    float vertices[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f};
    float texCoords[] = {0.0f, 0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.5f};
 
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    
    glTranslatef(x, y, 0.0f);
    glScalef(xscale, yscale, 1.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    
    glVertexPointer(2, GL_FLOAT, 0, texCoords);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glTexCoordPointer(2, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisable(GL_TEXTURE_2D);
}


int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 360);
    
    SDL_Event event;
    bool done = false;
    
    GLuint medal7 = LoadTexture(RESOURCE_FOLDER"flat_medal7.png");
    GLuint medal8 = LoadTexture(RESOURCE_FOLDER"flat_medal8.png");
    GLuint medal9 = LoadTexture(RESOURCE_FOLDER"flat_medal9.png");
    
    ShaderProgram program = ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

    glClearColor(0.4f, 0.2f, 0.4f, 1.0f);
    
    float lastFrameTicks = 0.0f;
    float angle = 0.0f;
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        angle += elapsed;
        DrawSprite(medal7, -0.75f, 0.0f, 1.0f, 1.0f, angle);
        DrawSprite(medal8, -0.25f, 0.0f, 1.0f, 1.0f, angle);
        DrawSprite(medal9, 0.25f, 0.0f, 1.0f, 1.0f, angle);
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
