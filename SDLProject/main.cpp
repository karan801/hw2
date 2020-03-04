#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <cstdlib>

#include <iostream>
SDL_Window* displayWindow;
bool gameIsRunning = true;

// ========== player X
ShaderProgram programx;
glm::mat4 viewxMatrix, modelxMatrix, projectionxMatrix;
float player_x = 0;

// Start at 0, 0, 0
glm::vec3 player_x_position = glm::vec3(4,0,0);

// Don't go anywhere (yet).
glm::vec3 player_x_movement = glm::vec3(0,0,0);

float player_x_speed = 1.0f;

GLuint playerxTextureID;

// player X end ==========
// ========== player Y
ShaderProgram programy;
glm::mat4 viewyMatrix, modelyMatrix, projectionyMatrix;
float player_y = 0;

// Start at 0, 0, 0
glm::vec3 player_y_position = glm::vec3(-4,0,0);

// Don't go anywhere (yet).
glm::vec3 player_y_movement = glm::vec3(0,0,0);

float player_y_speed = 1.0f;

GLuint playeryTextureID;
// player Y end ==========
// ========== ball
ShaderProgram programb;
glm::mat4 viewbMatrix, modelbMatrix, projectionbMatrix;
float ball = 0;

// Start at 0, 0, 0
glm::vec3 ball_position = glm::vec3(0,0,0);

// Don't go anywhere (yet).
glm::vec3 ball_movement = glm::vec3(0,0,0);

float ball_speed = (rand() % 5 + 1);
bool ballstart = false;
float loss = false;

GLuint ballTextureID;

void updateBall(const float deltaTime) {
    
    //renaming of variables for ease of typing
    //ball coords
    float b_x = ball_position.x;
    float b_y = ball_position.y;
    //player x coords
    float px_x = player_x_position.x;
    float px_y = player_x_position.y;
    //player y coords
    float py_x = player_y_position.x;
    float py_y = player_y_position.y;
    
    //heights & widths
    float ph = 2.0f; //player height
    float pw = 0.4f; //player width
    
    float bd = 0.9f; //ball diameter
    
    float pxxd = (fabs(b_x-px_x)) - ((bd + pw)/2.0f); // player x's and ball's x distance
    float pxyd = (fabs(b_y-px_y)) - ((bd + ph)/2.0f); // player x's and ball's y distance
    
    float pyxd = (fabs(b_x-py_x)) - ((bd + pw)/2.0f); // player x's and ball's x distance
    float pyyd = (fabs(b_y-py_y)) - ((bd + ph)/2.0f); // player x's and ball's y distance
    
    
    // hit by the left paddle
    if (pxxd < 0.0f && pxyd < 0.0f) {
        ball_speed = -(ball_speed);
        ball_movement.y = -(ball_movement.y);
    }
    // hit by the right paddle
    if (pyxd < 0.0f && pyyd < 0.0f) {
        ball_speed = -(ball_speed);
        ball_movement.y = -(ball_movement.y);
    }
    // hit the left wall
    if (b_x < -5.0f) {
        ball_speed = 0;
        ball_movement.x = 0;
        ball_movement.y = 0;
        loss = true;
    }
    // hit the right wall
    if (b_x > 5.0f) {
        ball_speed = 0;
        ball_movement.x = 0;
        ball_movement.y = 0;
        loss = true;
    }
    // hit the top wall
    if (b_y > 3.75f) {
        ball_movement.y = -(ball_movement.y);
    }
    // hit the bottom wall
    if (b_y < -3.75f) {
        ball_movement.y = -(ball_movement.y);
    }
    ball_position += ball_movement * ball_speed * deltaTime;
    //std::cout << ball_position.x << " " << ball_position.y << " " << ball_movement.x << " " << ball_movement.y << " " << ball_speed;
}
// ball ==========

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

float lastTicks = 0.0f;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong by Karan Ganta", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    
    // player X
    programx.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewxMatrix = glm::mat4(1.0f);
    modelxMatrix = glm::mat4(1.0f);
    projectionxMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    programx.SetProjectionMatrix(projectionxMatrix);
    programx.SetViewMatrix(viewxMatrix);
    programx.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    glUseProgram(programx.programID);
    
    playerxTextureID = LoadTexture("pong.png");
    // player X end
    // player Y
    programy.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewyMatrix = glm::mat4(1.0f);
    modelyMatrix = glm::mat4(1.0f);
    projectionyMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    programy.SetProjectionMatrix(projectionyMatrix);
    programy.SetViewMatrix(viewyMatrix);
    programy.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    glUseProgram(programy.programID);
    
    playeryTextureID = LoadTexture("pong.png");
    // player Y end
    // ball
    programb.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewbMatrix = glm::mat4(1.0f);
    modelbMatrix = glm::mat4(1.0f);
    projectionbMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    programb.SetProjectionMatrix(projectionbMatrix);
    programb.SetViewMatrix(viewbMatrix);
    programb.SetColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glUseProgram(programb.programID);
    
    ballTextureID = LoadTexture("pongball.png");
    // ball end
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glEnable(GL_BLEND);
    
    // Good setting for transparency
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

void ProcessInput() {
     
    player_x_movement = glm::vec3(0);
    player_y_movement = glm::vec3(0);
    ball_movement = glm::vec3(0);
     
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        //Start Game
                        ballstart = true;
                        //ball_movement.x += -3.0f;
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    // if the game has started, then the ball direction will be randomized
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_UP] && !loss) {
        player_x_movement.y = 5.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN] && !loss) {
        player_x_movement.y = -5.0f;
    }
    
    if (keys[SDL_SCANCODE_W] && !loss) {
        player_y_movement.y = 5.0f;
    }
    else if (keys[SDL_SCANCODE_S] && !loss) {
        player_y_movement.y = -5.0f;
    }
    
    // normalizing movement
    if (glm::length(player_x_movement) > 5.0f) {
        player_x_movement = glm::normalize(player_x_movement);
    }
    if (glm::length(player_y_movement) > 5.0f) {
        player_y_movement = glm::normalize(player_y_movement);
    }
    if (glm::length(ball_movement) > 5.0f) {
        ball_movement = glm::normalize(ball_movement);
    }
 }

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    // player X
    // Add (direction * units per second * elapsed time)
    player_x_position += player_x_movement * player_x_speed * deltaTime;
    modelxMatrix = glm::mat4(1.0f);
    modelxMatrix = glm::translate(modelxMatrix, player_x_position);
    // player Y
    // Add (direction * units per second * elapsed time)
    player_y_position += player_y_movement * player_y_speed * deltaTime;
    modelyMatrix = glm::mat4(1.0f);
    modelyMatrix = glm::translate(modelyMatrix, player_y_position);
    // ball
    // Add (direction * units per second * elapsed time)
    if (ballstart){
        ball_movement.x += 3.0f;
        //ball_movement.y += 0.0f;
    }
    updateBall(deltaTime);
    modelbMatrix = glm::mat4(1.0f);
    modelbMatrix = glm::translate(modelbMatrix, ball_position);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    //float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float vertices[] = { -0.2, -1.0, 0.2, -1.0, 0.2, 1.0, -0.2, -1.0, 0.2, 1.0, -0.2, 1.0 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    // x
    programx.SetModelMatrix(modelxMatrix);
    glVertexAttribPointer(programx.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(programx.positionAttribute);
    glVertexAttribPointer(programx.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(programx.texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, playerxTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(programx.positionAttribute);
    glDisableVertexAttribArray(programx.texCoordAttribute);
    // y
    programy.SetModelMatrix(modelyMatrix);
    glVertexAttribPointer(programy.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(programy.positionAttribute);
    glVertexAttribPointer(programy.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(programy.texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, playeryTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(programy.positionAttribute);
    glDisableVertexAttribArray(programy.texCoordAttribute);
    // ball
    
    float bvertices[] = {-0.3, -0.3, 0.3, -0.3, 0.3, 0.3, -0.3, -0.3, 0.3, 0.3, -0.3, 0.3 };
    programb.SetModelMatrix(modelbMatrix);
    glVertexAttribPointer(programb.positionAttribute, 2, GL_FLOAT, false, 0, bvertices);
    glEnableVertexAttribArray(programb.positionAttribute);
    glVertexAttribPointer(programb.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(programb.texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, ballTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(programb.positionAttribute);
    glDisableVertexAttribArray(programb.texCoordAttribute);
    
    SDL_GL_SwapWindow(displayWindow);
    
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
