
#include "Entity.h"

float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

Entity::Entity()
: active(true), position_x(0.0f), position_y(0.0f), width(1.0f), height(1.0f), hitboxHeight(1.0f), hitboxWidth(1.0f), velocity_x(0.0f), velocity_y(0.0f), acceleration_x(0.0f), acceleration_y(0.0f), friction_x(0.0f), friction_y(0.0f), isStatic(false), collidedTop(false), collidedBottom(false), collidedLeft(false), collidedRight(false) {}

Entity::Entity(float x, float y, float height, float width, bool static_entity) :
position_x(x), position_y(y), height(height), width(width), isStatic(static_entity), velocity_x(0.0f), velocity_y(0.0f), acceleration_x(0.0f), acceleration_y(0.0f), friction_x(0.0f), friction_y(0.0f), collidedTop(false), collidedBottom(false), collidedLeft(false), collidedRight(false) {}

void Entity::Update(float elapsed) {
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    position_x += velocity_x * elapsed;
    position_y += velocity_y * elapsed;
    
    velocity_x += acceleration_x * elapsed;
    velocity_y += acceleration_y * elapsed;
    
    velocity_x = lerp(velocity_x, 0.0f, elapsed * friction_x);
    velocity_y = lerp(velocity_y, 0.0f, elapsed * friction_y);
    
    velocity_y += -10.0f * elapsed;
}

void Entity::fixPenetration(float width2, float height2) {
    float xPenetration = fabs(position_x - width/2 - width2/2);
    float yPenetration = fabs(position_y - height/2 - height2/2);
    
    if (collidedLeft) {
        position_x += xPenetration;
    }
    else if (collidedRight) {
        position_x -= xPenetration;
    }
    
    if (collidedTop) {
        position_y -= yPenetration;
    }
    else if (collidedBottom) {
        position_y += yPenetration;
    }
}

void Entity::Draw(ShaderProgram* program) {
    if (active) {
        Matrix modelMatrix;
        modelMatrix.Translate(position_x, position_y, 0.0f);
        program->setModelMatrix(modelMatrix);
        
        sprite.Draw(program);
    }
}

bool Entity::collidesWith(Entity& entity) {
    if (active && entity.active) {
        if (!(position_x - hitboxWidth / 2 > entity.position_x + entity.hitboxWidth / 2 ||
              position_x + hitboxWidth / 2 < entity.position_x - entity.hitboxWidth / 2 ||
              position_y - hitboxHeight / 2 > entity.position_y + entity.hitboxHeight / 2 ||
              position_y + hitboxHeight / 2 < entity.position_y - entity.hitboxHeight / 2 )) {
            return true;
        }
        return false;
    }
    return false;
}