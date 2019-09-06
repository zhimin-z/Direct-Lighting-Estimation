#pragma once
#include <la.h>

class Transform
{
public:
    Transform();
    Transform(const glm::vec3 &t, const glm::vec3 &r, const glm::vec3 &s);

    void SetMatrices();

    const glm::mat4 &T();
    const glm::mat4 &invT();
    const glm::mat4 &invTransT();

    const glm::vec3 &position();
    glm::vec3 GetScale() const{return scale;}

private:
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 worldTransform;
    glm::mat4 inverse_worldTransform;
    glm::mat4 inverse_transpose_worldTransform;
};
