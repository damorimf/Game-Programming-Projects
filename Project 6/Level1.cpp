#include "Level1.h"

#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 14

#define LEVEL1_ENEMY_COUNT 3

unsigned int level1_data[] =
{
 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 3, 1, 1, 1, 1, 3, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level1::Initialize() {
    state.nextScene = -1;

	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);

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
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = CHASER;
    state.enemies[0].textureID = Util::LoadTexture("ZombieToad.png");
    state.enemies[0].speed = 1.5f;
    state.enemies[0].status = 1;
    state.enemies[0].position = glm::vec3(2, -2, 0);
    state.enemies[0].width = 0.36f;
    state.enemies[0].height = 0.5f;

    for (int i = 1; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiType = CRAZY;
        state.enemies[i].textureID = Util::LoadTexture("potato.png");
        state.enemies[i].speed = 3.0f;
        state.enemies[i].status = 1;
        state.enemies[i].width = 0.5f;
        state.enemies[i].height = 0.5f;
    }
    
    state.enemies[1].position = glm::vec3(3, -3, 0);
    state.enemies[1].movement = glm::vec3(-1, 1, 0);
    state.enemies[2].position = glm::vec3(9, -9, 0);
    state.enemies[2].movement = glm::vec3(-1, 1, 0);
}

void Level1::Update(float deltaTime) {
    if (state.player->status != 1 || state.player->status != 2) {
        state.player->Update(deltaTime, state.player, state.sword, state.enemies, LEVEL1_ENEMY_COUNT, state.map);

        state.sword->Update(deltaTime, state.player, state.sword, state.enemies, LEVEL1_ENEMY_COUNT, state.map);

        for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
            state.enemies[i].Update(deltaTime, state.player, state.sword, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
        }
        if (state.player->status == -1) {
            state.player->status = 0;
            state.nextScene = 1;
        }
        if (state.player->kills == LEVEL1_ENEMY_COUNT) {
            //state.player->status = 2;
            state.nextScene = 2;
        }
    }
}

void Level1::Render(ShaderProgram* program) {
	state.map->Render(program);

	state.player->Render(program);
    
    state.sword->Render(program);

    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}