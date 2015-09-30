
#include "Entity.h"

Entity::Entity(ShaderProgram *program, Matrix& modelMatrix, float width, float height)
: program(program), modelMatrix(modelMatrix), width(width), height(height)
{
    x = 0.0f;
    y = 0.0f;
    rotation = 0.0f;
    speed = 0.0f;
    direction_x = 0.0f;
    direction_y = 0.0f;
}

void Entity::Draw() {
    modelMatrix.identity();
    
    modelMatrix.Translate(x, y, 0.0f);
    
    program->setModelMatrix(modelMatrix);
    
    float vertices[] = { -width / 2, -height / 2, width / 2, height / 2, -width / 2, height / 2,
        width / 2, height / 2, -width / 2, -height / 2, width / 2, -height / 2};
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
	float texCoords[] = { 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}