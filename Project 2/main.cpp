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

#include "time.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, player1Matrix, player2Matrix, ballMatrix, projectionMatrix;

float verticesPaddle[] = { -0.25, -1.00, 0.25, -1.00, 0.25, 1.00,   -0.25, -1.00, 0.25, 1.00, -0.25, 1.00 };

// Start at 0, 0, 0
glm::vec3 player1_position = glm::vec3(-4.75f, 0.0f, 0.0f);
// Don’t go anywhere (yet).
glm::vec3 player1_movement = glm::vec3(0.0f, 0.0f, 0.0f);
float player1_speed = 7.0f;

// Start at 0, 0, 0
glm::vec3 player2_position = glm::vec3(4.75f, 0.0f, 0.0f);
// Don’t go anywhere (yet).
glm::vec3 player2_movement = glm::vec3(0.0f, 0.0f, 0.0f);
float player2_speed = 7.0f;

float verticesBall[] = { -0.25, -0.25, 0.25, -0.25, 0.25, 0.25,   -0.25, -0.25, 0.25, 0.25, -0.25, 0.25 };
bool ball_on = false;

// Start at 0, 0, 0
glm::vec3 ball_position = glm::vec3(0.0f, 0.0f, 0.0f);
// Don’t go anywhere (yet).
glm::vec3 ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);
float ball_speed = 5.0f;

//GLuint player1TextureID;
//GLuint player2TextureID;

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
    displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    //program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    player1Matrix = glm::mat4(1.0f);
    player2Matrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    /*
    * Textured Code
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    player1TextureID = LoadTexture("PongBoard.png");
    player2TextureID = LoadTexture("PongBoard.png");
    */

    srand(time(0));
    int direction = rand() % 4;
    switch (direction) {
        case 0:
            ball_movement = glm::vec3(1.0f, 1.0f, 0.0f);
            break;
        case 1:
            ball_movement = glm::vec3(1.0f, -1.0f, 0.0f);
            break;
        case 2:
            ball_movement = glm::vec3(-1.0f, 1.0f, 0.0f);
            break;
        case 3:
            ball_movement = glm::vec3(-1.0f, -1.0f, 0.0f);
            break;
    }
}

void ProcessInput() {
    player1_movement = glm::vec3(0);
    player2_movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_SPACE:
                ball_on = true;
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (ball_on) {
        if (keys[SDL_SCANCODE_W]) {
            if (player1_position.y <= 2.75f) {
                player1_movement.y = 1.0f;
            }
            else {
                player1_movement = glm::vec3(0);
            }
        }
        else if (keys[SDL_SCANCODE_S]) {
            if (player1_position.y >= -2.75f) {
                player1_movement.y = -1.0f;
            }
            else {
                player1_movement = glm::vec3(0);
            }
        }
        if (keys[SDL_SCANCODE_UP]) {
            if (player2_position.y <= 2.75f) {
                player2_movement.y = 1.0f;
            }
            else {
                player2_movement = glm::vec3(0);
            }
        }
        else if (keys[SDL_SCANCODE_DOWN]) {
            if (player2_position.y >= -2.75f) {
                player2_movement.y = -1.0f;
            }
            else {
                player2_movement = glm::vec3(0);
            }
        }
        if (glm::length(player1_movement) > 1.0f) {
            player1_movement = glm::normalize(player1_movement);
        }
        if (glm::length(player2_movement) > 1.0f) {
            player2_movement = glm::normalize(player2_movement);
        }
    }  
}

float lastTicks = 0.0f;

int decentFlag = 0;
int leftFlag = 0;

bool CollisionDetect(glm::vec3 box1, float verticesBox1[], glm::vec3 box2, float verticesBox2[]) {
    float box1_height = verticesBox1[5] * 2;
    float box1_width = verticesBox1[4] * 2;

    float box2_height = verticesBox2[5] * 2;
    float box2_width = verticesBox2[4] * 2;

    float x_diff = fabs(box2.x - box1.x);
    float y_diff = fabs(box2.y - box1.y);

    float x_dist = x_diff - (box1_width + box2_width) / 2;
    float y_dist = y_diff - (box1_height + box2_height) / 2;
    if (x_dist < 0 && y_dist < 0) return true;
    else return false;
}

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    // Add (direction * units per second * elapsed time)
    if (ball_position.x <= 4.75f && ball_position.x >= -4.75f) {
        player1_position += player1_movement * player1_speed * deltaTime;
        player1Matrix = glm::mat4(1.0f);
        player1Matrix = glm::translate(player1Matrix, player1_position);

        player2_position += player2_movement * player2_speed * deltaTime;
        player2Matrix = glm::mat4(1.0f);
        player2Matrix = glm::translate(player2Matrix, player2_position);
        if (ball_on) {
            if (!(ball_position.y < 3.5f) && ball_movement.y > 0) {
                ball_movement = glm::vec3(ball_movement.x, -ball_movement.y, 0.0f);
            }
            if (!(ball_position.y > -3.5f) && ball_movement.y < 0) {
                ball_movement = glm::vec3(ball_movement.x, -ball_movement.y, 0.0f);
            }
            if (CollisionDetect(player1_position, verticesPaddle, ball_position, verticesBall) && ball_movement.x < 0) {
                ball_movement = glm::vec3(-ball_movement.x, ball_movement.y, 0.0f);
            }
            if (CollisionDetect(player2_position, verticesPaddle, ball_position, verticesBall) && ball_movement.x > 0) {
                ball_movement = glm::vec3(-ball_movement.x, ball_movement.y, 0.0f);
            }
            ball_position += ball_movement * ball_speed * deltaTime;
            ballMatrix = glm::mat4(1.0f);
            ballMatrix = glm::translate(ballMatrix, ball_position);
        }
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableVertexAttribArray(program.positionAttribute);

            
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesBall);

    program.SetModelMatrix(ballMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);

      
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesPaddle);
    
    program.SetModelMatrix(player1Matrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(player2Matrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    /*
    * Textured Code
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    program.SetModelMatrix(player1Matrix);
    glBindTexture(GL_TEXTURE_2D, player1TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(player2Matrix);
    glBindTexture(GL_TEXTURE_2D, player2TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    */
    glDisableVertexAttribArray(program.positionAttribute);
    //glDisableVertexAttribArray(program.texCoordAttribute);

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