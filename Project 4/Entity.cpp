#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity* other) {
    if (isActive == false || other->isActive == false) return false;

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0) return true;
    else return false;
}

void Entity::CheckCollisionsY(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];

        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                collidedEntity = object;
            }
            if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                collidedEntity = object;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];

        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                collidedEntity = object;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                collidedEntity = object;
            }
        }
    }
}

void Entity::AIWalker(Entity* player) {
    if (collidedRight) {
        movement.x = -1.0f;
        status = 2;
    }
    if (collidedLeft) {
        movement.x = 1.0f;
        status = 0;
    }
    
}

void Entity::AISneaky(Entity* player) {
    if (abs(player->position.x - position.x) < 2.0f) {
        if (player->position.x > position.x) {
            movement.x = 1.0f;
        } else if (player->position.x < position.x) {
            movement.x = -1.0f;
        }        
    }
}

void Entity::AIJumper(Entity* player) {
    if (collidedRight) {
        movement.x = -1.0f;
        status = 0;
    }
    if (collidedLeft) {
        movement.x = 1.0f;
        status = 2;
    }
    if (collidedBottom && position.x < -1.5f) {
        movement.x = 1.0f;
        status = 2;
        jump = true;
    }
    else if (collidedBottom) {
        jump = true;
    }
    
}

void Entity::AI(Entity* player) {
    switch (aiType) {
        case WALKER:
            AIWalker(player);
            break;
        case SNEAKY:
            AISneaky(player);
            break;
        case JUMPER:
            AIJumper(player);
            break;
    }
}

void Entity::Update(float deltaTime, Entity* playerEnemy, int enemyCount, Entity *platforms, int platformCount)
{
    if (isActive == false) return;   

    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

    if (animIndices != NULL) {
        if (glm::length(velocity.x) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.1f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 1;
                }
            }
        }
        else animIndex = 0;
    }

    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;

    velocity += acceleration * deltaTime;    
    
    position.y += velocity.y * deltaTime;       // Move on Y
    CheckCollisionsY(platforms, platformCount); // Fix if needed

    position.x += velocity.x * deltaTime;       // Move on X
    CheckCollisionsX(platforms, platformCount); // Fix if needed    

    if (entityType == ENEMY) {
        CheckCollisionsY(playerEnemy, enemyCount);
        CheckCollisionsX(playerEnemy, enemyCount);

        if ((collidedLeft || collidedRight || collidedBottom) && collidedEntity->entityType == PLAYER) {
            playerEnemy->status = 1;
        }
        AI(playerEnemy);
    }

    if (entityType == PLAYER) {
        CheckCollisionsY(playerEnemy, enemyCount);
        CheckCollisionsX(playerEnemy, enemyCount);

        if ((collidedLeft || collidedRight || collidedTop) && collidedEntity->entityType == ENEMY) {
            status = 1;
        }
        else if ((collidedBottom) && collidedEntity->entityType == ENEMY) {
            collidedEntity->isActive = false;
        }
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);    
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    if (isActive == false) return;

    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
