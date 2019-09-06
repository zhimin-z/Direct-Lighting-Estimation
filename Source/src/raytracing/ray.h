#pragma once
#include <la.h>

class Ray
{
public:
    Ray();
    Ray(const glm::vec3 &o, const glm::vec3 &d);
    Ray(const glm::vec4 &o, const glm::vec4 &d);
    Ray(const Ray &r);

    //Return a copy of this ray that has been transformed
    //by the input transformation matrix.
    Ray GetTransformedCopy(const glm::mat4& T) const;

    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 transmitted_color;
};
