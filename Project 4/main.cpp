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

#include "Entity.h"

#include <vector>

#define PLATFORM_COUNT 42
#define ENEMY_COUNT 3

struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* enemies;
};

GameState state;
int activeCounter = 0;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

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

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {

        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
         offset + (-0.5f * size), 0.5f * size,
         offset + (-0.5f * size), -0.5f * size,
         offset + (0.5f * size), 0.5f * size,
         offset + (0.5f * size), -0.5f * size,
         offset + (0.5f * size), 0.5f * size,
         offset + (-0.5f * size), -0.5f * size,
            });
        texCoords.insert(texCoords.end(), {
        u, v,
        u, v + height,
        u + width, v,
        u + width, v + height,
        u + width, v,
        u, v + height,
            });

    } // end of for loop
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Doctor Who: Attack of the AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    #ifdef _WINDOWS
    glewInit();
    #endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.898f, 0.320f, 0.0f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
       
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-3.5f, -3.25f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -25.0f, 0);
    state.player->speed = 4.0f;
    state.player->textureID = LoadTexture("10thDoctorSprites2.png");
    state.player->width = 0.75f;
    state.player->height = 1.0f;
    state.player->jumpPower = 10.0f;


    state.player->animRight = new int[4] {0, 9, 10, 8};
    state.player->animLeft = new int[4] {0, 5, 4, 6};
    
    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 3;
    state.player->animIndex = 1;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4; 
    
    //Initialize Enemies

    state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].acceleration = glm::vec3(0, -25.0f, 0);
    }
    
    state.enemies[0].aiType = WALKER;
    state.enemies[0].textureID = LoadTexture("dalek.png");
    state.enemies[0].position = glm::vec3(2.5f, -1.25f, 0.0f);
    state.enemies[0].speed = 1.0f;
    state.enemies[0].movement.x = 1.0f;
    
    state.enemies[1].aiType = SNEAKY;
    state.enemies[1].textureID = LoadTexture("potato.png");
    state.enemies[1].position = glm::vec3(0.5f, -2.25f, 0.0f);
    state.enemies[1].height = 0.5f;
    state.enemies[1].width = 0.5f;
    state.enemies[1].speed = 1.0f;

    state.enemies[2].aiType = JUMPER;
    state.enemies[2].textureID = LoadTexture("cyberman2.png");
    state.enemies[2].position = glm::vec3(3.5f, 2.5f, 0);
    state.enemies[2].speed = 2.25f;
    state.enemies[2].movement.x = -1.0f;
    state.enemies[2].jumpPower = 6.0f;

    //Initialize Platforms

    state.platforms = new Entity[PLATFORM_COUNT];   

    GLuint platformTextureID = LoadTexture("magma.png");

    int leftWall[6] = { 0, 1, 2, 3, 4, 5};
    
    for (int i = 0; i < 6; i++) {
        state.platforms[leftWall[i]].position = glm::vec3(-5.0f, 4.25f - i, 0);
    }

    int rightWall[9] = { 6, 7, 8, 9, 10, 11, 12 , 13 };
    for (int i = 0; i < 9; i++) {
        state.platforms[rightWall[i]].position = glm::vec3(5.0f, 4.25f - i, 0);
    }  

    int floor[9] = { 14, 15, 16, 17, 18, 19, 20, 21, 22 }; 
    for (int i = 0; i < 9; i++) {
        state.platforms[floor[i]].position = glm::vec3(-3.5f + i, -3.25f, 0);
    }

    int plat1[5] = { 23, 24, 25, 26, 27 };
    for (int i = 0; i < 5; i++) {
        state.platforms[plat1[i]].position = glm::vec3(1.25f + i, -2.25f, 0);
    }

    int plat2[4] = { 28, 29, 30, 31 };
    for (int i = 0; i < 4; i++) {
        state.platforms[plat2[i]].position = glm::vec3(-5.0f + i, -1.0f, 0);
    }

    int plat3 = 32;
    state.platforms[plat3].position = glm::vec3(-4.0f, 0.75f, 0);

    int plat4[7] = { 33, 34, 35, 36, 37, 38, 39 };
    for (int i = 0; i < 7; i++) {
        state.platforms[plat4[i]].position = glm::vec3(-1.5f + i, 1.5f, 0);
    }

    for (int i = 0; i < 40; i++) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].Update(0, NULL, 0, NULL, 0);
    }

    GLuint landingPadTextureID = LoadTexture("stone.png");
    int landingPad = 40;    
    state.platforms[landingPad].position = glm::vec3(-4.5f, -3.25f, 0);
    state.platforms[landingPad].entityType = PLATFORM;
    state.platforms[landingPad].textureID = landingPadTextureID;
    state.platforms[landingPad].Update(0, NULL, 0, NULL, 0);

    GLuint shipTextureID = LoadTexture("ship.png");
    int ship = 41;    
    state.platforms[ship].position = glm::vec3(-4.5f, -2.25f, 0);
    state.platforms[ship].width = 0.75f;
    state.platforms[ship].entityType = PLATFORM;
    state.platforms[ship].textureID = shipTextureID;
    state.platforms[ship].Update(0, NULL, 0, NULL, 0);
}

void ProcessInput() {    
    state.player->movement = glm::vec3(0);
    
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
                        if (state.player->collidedBottom && (state.player->status != 1 && state.player->status != 2)) {
                            state.player->jump = true;
                        }                        
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT] && (state.player->status != 1 && state.player->status != 2)) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT] && (state.player->status != 1 && state.player->status != 2)) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }
    else {
        state.player->movement.x = 0.0f;
    }
    

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {    
    if (state.player->status != 1 && state.player->status != 2) {
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float deltaTime = ticks - lastTicks;
        lastTicks = ticks;

        deltaTime += accumulator;
        if (deltaTime < FIXED_TIMESTEP) {
            accumulator = deltaTime;
            return;
        }

        while (deltaTime >= FIXED_TIMESTEP) {
            // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
            state.player->Update(FIXED_TIMESTEP, state.enemies, ENEMY_COUNT, state.platforms, PLATFORM_COUNT);
            for (int i = 0; i < ENEMY_COUNT; i++) {
                state.enemies[i].Update(FIXED_TIMESTEP, state.player, 1, state.platforms, PLATFORM_COUNT);
                if (!state.enemies[i].isActive) {
                    activeCounter++;
                }
            }
            if (activeCounter == 3) {
                state.player->status = 2;
            }
            activeCounter = 0;

            deltaTime -= FIXED_TIMESTEP;
        }

        accumulator = deltaTime;
    }    
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }

    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (i == 0 && state.enemies[i].status == 2) {
            state.enemies[i].textureID = LoadTexture("dalek2.png");
        }
        else if (i == 0 && state.enemies[i].status == 0){
            state.enemies[i].textureID = LoadTexture("dalek.png");
        }
        if (i == 2 && state.enemies[i].status == 0) {
            state.enemies[i].textureID = LoadTexture("cyberman2.png");
        }
        else if (i == 2 && state.enemies[i].status == 2) {
            state.enemies[i].textureID = LoadTexture("cyberman.png");
        }
        state.enemies[i].Render(&program);

    }

    state.player->Render(&program);
    
    
    if (state.player->status == 2) {
        DrawText(&program, LoadTexture("font.png"), "You win!", 1.0f, -0.5f, glm::vec3(-2.0f, 0, 0));
    }
    else if (state.player->status == 1) {
        DrawText(&program, LoadTexture("font.png"), "You lose!", 1.0f, -0.5f, glm::vec3(-2.5f, 0, 0));
    }
    

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
