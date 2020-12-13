#include "MainMenu.h"

void MainMenu::Initialize() {
    state.nextScene = -1;
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(7, -7, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 4.0f;
    state.player->textureID = Util::LoadTexture("PlayerSprites.png");
    state.player->width = 0.5f;
    state.player->height = 0.5f;

    state.player->animDown = new int[3]{ 2, 0, 1 };
    state.player->animLeft = new int[3]{ 5, 3, 4 };
    state.player->animRight = new int[3]{ 8, 7, 6 };
    state.player->animUp = new int[3]{ 11, 9, 10 };


    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 3;
    state.player->animIndex = 1;
    state.player->animCols = 3;
    state.player->animRows = 4;
}

void MainMenu::Update(float deltaTime) {
}

void MainMenu::Render(ShaderProgram* program) {
    if (state.player)
	Util::DrawText(program, Util::LoadTexture("font.png"), "CyberSlash:", 1.0f, -0.6f, glm::vec3(state.player->position.x - 1.75f, state.player->position.y + 0.55f, 0));
    Util::DrawText(program, Util::LoadTexture("font.png"), "2020", 1.0f, -0.6f, glm::vec3(state.player->position.x - 0.5f, state.player->position.y + 0.05f, 0));
}