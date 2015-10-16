
#include "Entity.h"

Entity::Entity()
: active(true), x(0.0f), y(0.0f), width(1.0f), height(1.0f), hitboxHeight(1.0f), hitboxWidth(1.0f), velocityX(0.0f), velocityY(0.0f)
{
    
}

void Entity::Draw(ShaderProgram* program) {
    if (active) {
        Matrix modelMatrix;
        modelMatrix.Translate(x, y, 0.0f);
        program->setModelMatrix(modelMatrix);
        
        sprite.Draw(program);
    }
}

bool Entity::collidesWith(Entity& object) {
    if (active && object.active) {
        if (!(x - hitboxWidth / 2 > object.x + object.hitboxWidth / 2 ||
              x + hitboxWidth / 2 < object.x - object.hitboxWidth / 2 ||
              y - hitboxHeight / 2 > object.y + object.hitboxHeight / 2 ||
              y + hitboxHeight / 2 < object.y - object.hitboxHeight / 2 )) {
            return true;
        }
        return false;
    }
    return false;
}