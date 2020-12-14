#include "Level3.h"

#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 14

#define LEVEL3_ENEMY_COUNT 10

unsigned int level3_data[] =
{
 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 3, 1, 1, 1, 1, 1, 1, 3, 1, 1, 2,
 2, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 2,
 2, 1, 1, 3, 3, 1, 1, 1, 1, 3, 3, 1, 1, 2,
 2, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 2,
 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level3::Initialize() {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);

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
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];

    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        if (i <= 8) {
            state.enemies[i].entityType = ENEMY;
            state.enemies[i].aiType = CRAZY;
            state.enemies[i].textureID = Util::LoadTexture("potato.png");
            state.enemies[i].speed = 2.5f;
            state.enemies[i].status = 1;
            state.enemies[i].width = 0.5f;
            state.enemies[i].height = 0.5f;
        }
        else {
            state.enemies[i].entityType = ENEMY;
            state.enemies[i].aiType = CHASER;
            state.enemies[i].textureID = Util::LoadTexture("potato.png");
            state.enemies[i].speed = 3.0f;
            state.enemies[i].status = 1;
            state.enemies[i].width = 0.5f;
            state.enemies[i].height = 0.5f;
        }
    }

    state.enemies[1].position = glm::vec3(6, -7, 0);
    state.enemies[1].movement = glm::vec3(-1, 1, 0);
    state.enemies[2].position = glm::vec3(6, -8, 0);
    state.enemies[2].movement = glm::vec3(-1, -1, 0);
    state.enemies[3].position = glm::vec3(6, -6, 0);
    state.enemies[3].movement = glm::vec3(-1, 1, 0);
    state.enemies[4].position = glm::vec3(7, -8, 0);
    state.enemies[4].movement = glm::vec3(1, -1, 0);
    state.enemies[5].position = glm::vec3(7, -6, 0);
    state.enemies[5].movement = glm::vec3(-1, 1, 0);
    state.enemies[6].position = glm::vec3(8, -8, 0);
    state.enemies[6].movement = glm::vec3(1, -1, 0);
    state.enemies[0].position = glm::vec3(8, -6, 0);
    state.enemies[0].movement = glm::vec3(1, 1, 0);

    state.enemies[7].position = glm::vec3(5, -3, 0);
    state.enemies[7].movement = glm::vec3(-1, 1, 0);
    state.enemies[8].position = glm::vec3(2, -3, 0);
    state.enemies[8].movement = glm::vec3(1, 1, 0);
    state.enemies[9].position = glm::vec3(2, -12, 0);
}

void Level3::Update(float deltaTime) {
    if (state.player->status != 1 || state.player->status != 2) {
        state.player->Update(deltaTime, state.player, state.sword, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

        state.sword->Update(deltaTime, state.player, state.sword, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

        for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
            state.enemies[i].Update(deltaTime, state.player, state.sword, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
        }
        if (state.player->status == -1) {
            state.player->status = 0;
            state.nextScene = 3;
        }
        if (state.player->kills == LEVEL3_ENEMY_COUNT) {
            state.nextScene = 4;
        }
    }
}

void Level3::Render(ShaderProgram* program) {
    state.map->Render(program);

    state.player->Render(program);

    state.sword->Render(program);

    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}