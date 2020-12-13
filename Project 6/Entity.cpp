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

bool Entity::SwordCollision(Entity* enemy) {
    if (isActive == false || enemy->isActive == false) return false;

    float xdist = fabs(position.x - enemy->position.x) - ((width + enemy->width) / 2.0f);
    float ydist = fabs(position.y - enemy->position.y) - ((height + enemy->height) / 2.0f);

    if (xdist < 0 && ydist < 0) {
        collidedEntity = enemy;
        return true;
    }    
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

void Entity::CheckCollisionsY(Map* map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);

    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        mapTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        mapTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        mapTop = true;
    }


    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        mapBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        mapBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        mapBottom = true;
    }
}

void Entity::CheckCollisionsX(Map* map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        mapLeft = true;
    }

    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        mapRight = true;
    }
}

void Entity::AIChaser(Entity* player, Entity* objects, int objectCount) {    
    if (position.x > player->position.x) {
        movement.x = -1.0f;
    }
    if (position.x < player->position.x) {
        movement.x = 1.0f;
    }
    if (position.y > player->position.y) {
        movement.y = -1.0f;
    }
    if (position.y < player->position.y) {
        movement.y = 1.0f;
    }
}

void Entity::AICrazy(Entity* player, Entity* objects, int objectCount) {
    if (mapTop) {
        movement.y = -1.0f;
    }
    if (mapBottom) {
        movement.y = 1.0f;
    }
    if (mapLeft) {
        movement.x = 1.0f;
    }
    if (mapRight) {
        movement.x = -1.0f;
    }

}

void Entity::AI(Entity* player, Entity* objects, int objectCount) {
    switch (aiType) {
        case CHASER:
            AIChaser(player, objects, objectCount);
            break;
        case CRAZY:
            AICrazy(player, objects, objectCount);
            break;
    }
}

void Entity::Update(float deltaTime, Entity* player, Entity* sword, Entity* objects, int objectCount, Map* map)
{
    if (isActive == false || player->status == 1) return;

    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

    mapTop = false;
    mapBottom = false;
    mapLeft = false;
    mapRight = false;

    if (animIndices != NULL) {
        if (glm::length(velocity.x) != 0 || glm::length(velocity.y) != 0) {
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
        else animIndex = 1;
        if (swing){
            animIndex = 0;
        }
    }    

    if (swing) {
        swingTime += deltaTime;
        if (swingTime >= 0.5f) {
            swing = false;
            swingTime = 0;
        }
    }

    velocity.x = movement.x * speed;
    velocity.y = movement.y * speed;

    velocity += acceleration * deltaTime;    
    
    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);

    if (entityType == ENEMY) {
        SwordCollision(sword);

        if (collidedEntity == sword && player->swing) {
            isActive = false;
            player->kills++;
        }

        CheckCollisionsY(player, 1);
        CheckCollisionsX(player, 1);
        
        if (collidedEntity == player) {
            player->status = 1;
            return;
        }

        //CheckCollisionsY(objects, objectCount);
        //CheckCollisionsX(objects, objectCount);
        AI(player, objects, objectCount);
    }
    
    if (entityType == PLAYER) {
        CheckCollisionsY(objects, objectCount);
        CheckCollisionsX(objects, objectCount);        
        if (collidedEntity) {
            status = 1;
            return;
        }        
    }

    if (entityType == SWORD) {
        switch (player->facing) {
            case 0:
                position.x = player->position.x + 0.5;
                position.y = player->position.y;
                break;
            case 1:
                position.x = player->position.x - 0.5;
                position.y = player->position.y;
                break;
            case 2:
                position.x = player->position.x;
                position.y = player->position.y + 0.5;
                break;
            case 3:
                position.x = player->position.x;
                position.y = player->position.y - 0.5;
                break;
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
