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

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, playerMatrix, enemyMatrix, projectionMatrix;

float lastTicks = 0.0f;

float player_x = 0.0f;
float player_y = 0.0f;
float player_rotate = 0.0f;

float enemy_x = 0.0f;
float enemy_y = 0.0f;
float enemy_rotate = 0.0f;

GLuint playerTextureID;
GLuint enemyTextureID;

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

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Will zombie Toad eat Mario?", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    playerMatrix = glm::mat4(1.0f);
    enemyMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.55f, 0.94f, 1.0f, 1.0f);

    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    playerTextureID = LoadTexture("Mario.png");
    enemyTextureID = LoadTexture("ZombieToad.png");

    //Inital Positions
    enemy_x = -4.0f;
    enemyMatrix = glm::translate(enemyMatrix, glm::vec3(enemy_x, 0.0f, 0.0f));
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

int decentFlag = 0;
int leftFlag = 0;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    //modelMatrix = glm::translate(modelMatrix, glm::vec3(player_x, 0.01f, 0.0f));
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));

    if (enemy_x <= 4.0f && !leftFlag) {
        enemyMatrix = glm::mat4(1.0f);
        enemy_x += deltaTime * 2.5f;
        enemyMatrix = glm::translate(enemyMatrix, glm::vec3(enemy_x, 0.0f, 0.0f));
    }
    else if (!leftFlag) {
        leftFlag = 1;
    }
    else if (enemy_x > -4.0f && leftFlag) {
        enemyMatrix = glm::mat4(1.0f);
        enemy_x += deltaTime * -2.5f;
        enemyMatrix = glm::translate(enemyMatrix, glm::vec3(enemy_x, 0.0f, 0.0f));
    }
    else {
        leftFlag = 0;
    }
    
    if (player_y <= 0.10f && !decentFlag) {
        playerMatrix = glm::mat4(1.0f);
        player_y += deltaTime * 1.5f;
        playerMatrix = glm::translate(playerMatrix, glm::vec3(0.0f, player_y, 0.0f));
    }
    else if (player_rotate <= 350.0f) {
        playerMatrix = glm::mat4(1.0f);
        player_y += deltaTime * 1.5f;
        playerMatrix = glm::translate(playerMatrix, glm::vec3(0.0f, player_y, 0.0f));
        player_rotate += deltaTime * 300.0f;
        playerMatrix = glm::rotate(playerMatrix, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));        
        decentFlag = 1;
    }
    else if (decentFlag && player_y > 0.0f) {    
        playerMatrix = glm::mat4(1.0f);
        player_y += deltaTime * -5.0f;
        playerMatrix = glm::translate(playerMatrix, glm::vec3(0.0f, player_y, 0.0f));        
    }
    else {
        decentFlag = 0;
        player_rotate = 0.0f;
    }
}

void Render() {
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    program.SetModelMatrix(playerMatrix);
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(enemyMatrix);
    glBindTexture(GL_TEXTURE_2D, enemyTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}