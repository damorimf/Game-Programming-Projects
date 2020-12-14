#include "Level2.h"

#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 14

#define LEVEL2_ENEMY_COUNT 7

unsigned int level2_data[] =
{
 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 3, 1, 1, 1, 1, 1, 1, 3, 1, 1, 2,
 2, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 3, 3, 1, 1, 1, 1, 1, 3, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 3, 1, 1, 1, 1, 3, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 2,
 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level2::Initialize() {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(7, -7, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 4.0f;
    state.player->textureID = Util::LoadTexture("PlayerSprites.png");
    state.player->width = 0.4f;
    state.player->height = 0.4f;

    state.player->animDown = new int[3]{ 2, 0, 1 };
    state.player->animLeft = new int[3]{ 5, 3, 4 };
    state.player->animRight = new int[3]{ 8, 7, 6 };
    state.player->animUp = new int[3]{ 11, 9, 10 };


    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 3;
    state.player->animIndex = 1;
    state.player->animCols = 3;
    state.player->animRows = 4;

    //Initialize Weapon
    state.sword = new Entity();
    state.sword->entityType = SWORD;
    state.sword->position = glm::vec3(state.player->position.x + 0.5, state.player->position.y + 0.5, 0);
    state.sword->width = 0.4f;
    state.sword->height = 0.4f;
    state.sword->textureID = Util::LoadTexture("boundary.png");

    //Initialize Enemies
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];

    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiType = CRAZY;
        state.enemies[i].textureID = Util::LoadTexture("potato.png");
        state.enemies[i].speed = 2.5f;
        state.enemies[i].status = 1;
        state.enemies[i].width = 0.5f;
        state.enemies[i].height = 0.5f;
    }

    state.enemies[1].position = glm::vec3(2, -2, 0);
    state.enemies[1].movement = glm::vec3(-1, 1, 0);
    state.enemies[2].position = glm::vec3(2, -4, 0);
    state.enemies[2].movement = glm::vec3(1, -1, 0);
    state.enemies[3].position = glm::vec3(2, -6, 0);
    state.enemies[3].movement = glm::vec3(-1, -1, 0);
    state.enemies[4].position = glm::vec3(2, -8, 0);
    state.enemies[4].movement = glm::vec3(1, 1, 0);
    state.enemies[5].position = glm::vec3(2, -10, 0);
    state.enemies[5].movement = glm::vec3(-1, -1, 0);
    state.enemies[6].position = glm::vec3(5, -11, 0);
    state.enemies[6].movement = glm::vec3(-1, 1, 0);
    state.enemies[0].position = glm::vec3(13, -6, 0);
    state.enemies[0].movement = glm::vec3(1, 1, 0);
}

void Level2::Update(float deltaTime) {
    if (state.player->status != 1 || state.player->status != 2) {
        state.player->Update(deltaTime, state.player, state.sword, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

        state.sword->Update(deltaTime, state.player, state.sword, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

        for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
            state.enemies[i].Update(deltaTime, state.player, state.sword, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
        }
        if (state.player->status == -1) {
            state.player->status = 0;
            state.nextScene = 2;
        }
        if (state.player->kills == LEVEL2_ENEMY_COUNT) {
            state.nextScene = 3;
        }
    }
}

void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);

    state.player->Render(program);

    state.sword->Render(program);

    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}