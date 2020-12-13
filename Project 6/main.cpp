#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "MainMenu.h"
#include "Level1.h"

#include <vector>

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene* currentScene;
Scene* sceneList[4];
int currentSceneIndex = 0;
int lives = 3;

Mix_Music* music;
Mix_Chunk* swing;

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("CyberSlash 2020", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    #ifdef _WINDOWS
    glewInit();
    #endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("mainsong.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME);
    swing = Mix_LoadWAV("swing.wav");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.67f, 0.67f, 1.0f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sceneList[0] = new MainMenu();
    sceneList[1] = new Level1();
    //sceneList[2] = new Level2();
    //sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);    
}

void ProcessInput() {    
    currentScene->state.player->movement = glm::vec3(0);
    
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
                        if (currentScene->state.player->status == 0) {
                            Mix_PlayChannel(-1, swing, 0);
                            currentScene->state.player->swing = true;
                        }                        
                        break;
                    case SDLK_RETURN:
                        if (currentSceneIndex == 0) {
                            currentSceneIndex = 1;
                            SwitchToScene(sceneList[currentSceneIndex]);
                        }
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT] && (currentScene->state.player->status != 1 && currentScene->state.player->status != 2)) {

        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        currentScene->state.player->facing = 1;
    }
    else if (keys[SDL_SCANCODE_RIGHT] && (currentScene->state.player->status != 1 && currentScene->state.player->status != 2)) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
        currentScene->state.player->facing = 0;
    }
    else if (keys[SDL_SCANCODE_UP] && (currentScene->state.player->status != 1 && currentScene->state.player->status != 2)) {
        currentScene->state.player->movement.y = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animUp;
        currentScene->state.player->facing = 2;
    }
    else if (keys[SDL_SCANCODE_DOWN] && (currentScene->state.player->status != 1 && currentScene->state.player->status != 2)) {
        currentScene->state.player->movement.y = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animDown;
        currentScene->state.player->facing = 3;
    }
    else {
        currentScene->state.player->movement.x = 0.0f;
        currentScene->state.player->movement.y = 0.0f;
    }
    

    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {    
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
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->status == 2 || currentScene->state.player->status == 1) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-7, 7, 0));
    } else{
        if (currentScene->state.player->position.x > 5.25 && currentScene->state.player->position.x < 7.75 && -currentScene->state.player->position.y > 4 && -currentScene->state.player->position.y < 9) {
            viewMatrix = glm::translate(viewMatrix,
                glm::vec3(-currentScene->state.player->position.x, -currentScene->state.player->position.y, 0));
        }
        else if (currentScene->state.player->position.x > 7.75) {
            if (-currentScene->state.player->position.y > 9) {
                viewMatrix = glm::translate(viewMatrix, glm::vec3(-7.75, 9, 0));
            }
            else if (-currentScene->state.player->position.y < 4) {
                viewMatrix = glm::translate(viewMatrix, glm::vec3(-7.75, 4, 0));
            }
            else {
                viewMatrix = glm::translate(viewMatrix, glm::vec3(-7.75, -currentScene->state.player->position.y, 0));
            }
        }
        else if (currentScene->state.player->position.x < 5.25) {
            if (-currentScene->state.player->position.y > 9) {
                viewMatrix = glm::translate(viewMatrix, glm::vec3(-5.25, 9, 0));
            }
            else if (-currentScene->state.player->position.y < 4) {
                viewMatrix = glm::translate(viewMatrix, glm::vec3(-5.25, 4, 0));
            }
            else {
                viewMatrix = glm::translate(viewMatrix, glm::vec3(-5.25, -currentScene->state.player->position.y, 0));
            }
        }
        else if (-currentScene->state.player->position.y > 9) {
            if (currentScene->state.player->position.x > 4 && currentScene->state.player->position.x < 8) {
                viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 9, 0));
            }
        }
        else if (-currentScene->state.player->position.y < 4) {
            if (currentScene->state.player->position.x > 4 && currentScene->state.player->position.x < 8) {
                viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 4, 0));
            }
        }
    }    

    if (lives <= 0) {
        currentScene->state.player->status = 1;
    }    
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);

    if (currentScene->state.player->status == 1) {
        Util::DrawText(&program, Util::LoadTexture("font.png"), "You lose!", 1.0f, -0.5f, glm::vec3(5, -6.75, 0));
        
    }
    else if (currentScene->state.player->status == 2) {
        Util::DrawText(&program, Util::LoadTexture("font.png"), "You win!", 1.0f, -0.5f, glm::vec3(5.15, -6.75, 0));
    }
    else {
        currentScene->Render(&program);
    }
    /*
    if (currentSceneIndex != 0 && currentScene->state.player->status != 2) {
        if (currentScene->state.player->position.x > 5 && currentScene->state.player->position.x < 8) {
            switch (lives) {
                case 1:
                    Util::DrawText(&program, Util::LoadTexture("font.png"), "Lives: 1", 1.0f, -0.5f, glm::vec3(currentScene->state.player->position.x - 4, -0.5f, 0));
                    break;
                case 2:
                    Util::DrawText(&program, Util::LoadTexture("font.png"), "Lives: 2", 1.0f, -0.5f, glm::vec3(currentScene->state.player->position.x - 4, -0.5f, 0));
                    break;
                case 3:
                    Util::DrawText(&program, Util::LoadTexture("font.png"), "Lives: 3", 1.0f, -0.5f, glm::vec3(currentScene->state.player->position.x - 4, -0.5f, 0));
                    break;
            }            
        }
        else if (currentScene->state.player->position.x > 8) {
            switch (lives) {
                case 1:
                    Util::DrawText(&program, Util::LoadTexture("font.png"), "Lives: 1", 1.0f, -0.5f, glm::vec3(4, -0.5f, 0));
                    break;
                case 2:
                    Util::DrawText(&program, Util::LoadTexture("font.png"), "Lives: 2", 1.0f, -0.5f, glm::vec3(4, -0.5f, 0));
                    break;
                case 3:
                    Util::DrawText(&program, Util::LoadTexture("font.png"), "Lives: 3", 1.0f, -0.5f, glm::vec3(4, -0.5f, 0));
                    break;
            }
        }
        else {
            switch (lives) {
                case 1:
                    Util::DrawText(&program, Util::LoadTexture("font.png"), "Lives: 1", 1.0f, -0.5f, glm::vec3(1, -0.5f, 0));
                    break;
                case 2:
                    Util::DrawText(&program, Util::LoadTexture("font.png"), "Lives: 2", 1.0f, -0.5f, glm::vec3(1, -0.5f, 0));
                    break;
                case 3:
                    Util::DrawText(&program, Util::LoadTexture("font.png"), "Lives: 3", 1.0f, -0.5f, glm::vec3(1, -0.5f, 0));
                    break;
            }
        }
    }
    */
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
        if (currentScene->state.nextScene >= 0) {
            if (currentScene->state.nextScene == currentSceneIndex) {
                lives--;
            }
            currentSceneIndex = currentScene->state.nextScene;
            if (lives != 0) {
                SwitchToScene(sceneList[currentScene->state.nextScene]);
            }                    
        }
        Render();
    }
    
    Shutdown();
    return 0;
}
