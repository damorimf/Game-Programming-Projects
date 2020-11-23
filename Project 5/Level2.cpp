#include "Level2.h"

#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMY_COUNT 15

unsigned int level2_data[] =
{
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 3,
 3, 3, 0, 0, 0, 3, 0, 1, 2, 2, 2, 2, 2, 3
};

void Level2::Initialize() {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, -2, 0);
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

    //Initialize Enemies
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];


    for (int i = 0; i < 1; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].acceleration = glm::vec3(0, -25.0f, 0);
    }
    GLuint enemyTexture = Util::LoadTexture("dalek.png");

    state.enemies[0].aiType = SNEAKY;
    state.enemies[0].textureID = enemyTexture;
    state.enemies[0].position = glm::vec3(11.0f, -4.0f, 0.0f);
    state.enemies[0].speed = 1.5f;
    state.enemies[0].isActive = true;

    for (int i = 1; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].acceleration = glm::vec3(0, 0.0f, 0);
        state.enemies[i].aiType = STILL;
        state.enemies[i].textureID = Util::LoadTexture("boundary.png");
        state.enemies[i].position = glm::vec3(i - 1, -9, 0.0f);
        state.enemies[i].isActive = true;
    }
}

void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    }
    if (state.player->status == -1) {
        state.player->status = 0;
        state.nextScene = 2;
    }
    if (state.player->position.x >= 12) {
        state.nextScene = 3;
    }
}

void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);

    state.player->Render(program);

    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        if (i == 0 && state.enemies[i].status == 2) {
            state.enemies[i].textureID = Util::LoadTexture("dalek2.png");
        }
        else if (i == 0 && state.enemies[i].status == 0) {
            state.enemies[i].textureID = Util::LoadTexture("dalek.png");
        }
        if (i == 2 && state.enemies[i].status == 0) {
            state.enemies[i].textureID = Util::LoadTexture("cyberman2.png");
        }
        else if (i == 2 && state.enemies[i].status == 2) {
            state.enemies[i].textureID = Util::LoadTexture("cyberman.png");
        }
        state.enemies[i].Render(program);
    }
}