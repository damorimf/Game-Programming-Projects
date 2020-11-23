#include "MainMenu.h"

void MainMenu::Initialize() {
    state.nextScene = -1;
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -25.0f, 0);
    state.player->speed = 4.0f;
    state.player->textureID = Util::LoadTexture("10thDoctorSprites2.png");
    state.player->width = 0.75f;
    state.player->height = 1.0f;
    state.player->jumpPower = 10.0f;


    state.player->animRight = new int[4]{ 0, 9, 10, 8 };
    state.player->animLeft = new int[4]{ 0, 5, 4, 6 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 3;
    state.player->animIndex = 1;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
}

void MainMenu::Update(float deltaTime) {
}

void MainMenu::Render(ShaderProgram* program) {
	Util::DrawText(program, Util::LoadTexture("font.png"), "Doctor Who:", 1.0f, -0.6f, glm::vec3(3.0f, -3.0f, 0));
    Util::DrawText(program, Util::LoadTexture("font.png"), "Sidescroll Advendture", 1.0f, -0.6f, glm::vec3(1.0f, -3.75f, 0));    
}