#include <scene/transform.h>

Transform::Transform():
    Transform(glm::vec3(0), glm::vec3(0), glm::vec3(1))
{}

Transform::Transform(const glm::vec3 &t, const glm::vec3 &r, const glm::vec3 &s):
    translation(t),
    rotation(r),
    scale(s)
{
    SetMatrices();
}

void Transform::SetMatrices()
{
    worldTransform = glm::translate(glm::mat4(1.0f), translation)
            * glm::rotate(glm::mat4(1.0f), rotation.x * DEG2RAD, glm::vec3(1,0,0))
            * glm::rotate(glm::mat4(1.0f), rotation.y * DEG2RAD, glm::vec3(0,1,0))
            * glm::rotate(glm::mat4(1.0f), rotation.z * DEG2RAD, glm::vec3(0,0,1))
            * glm::scale(glm::mat4(1.0f), scale);
    inverse_worldTransform = glm::inverse(worldTransform);
    inverse_transpose_worldTransform = glm::inverse(glm::transpose(worldTransform));
}

const glm::mat4& Transform::T()
{
    return worldTransform;
}

const glm::mat4& Transform::invT()
{
    return inverse_worldTransform;
}
const glm::mat4& Transform::invTransT()
{
    return inverse_transpose_worldTransform;
}

const glm::vec3& Transform::position()
{
    return translation;
}
