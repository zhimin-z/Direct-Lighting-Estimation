#include <scene/geometry/geometry.h>

float Geometry::RayPDF(const Intersection &isx, const Ray &ray)
{
    //TODO
    //The isx passed in was tested ONLY against us (no other scene objects), so we test if NULL
    //rather than if != this.
    if(isx.object_hit == NULL)
    {
        return 0;
    }
    //Add more here
    float cosine = glm::dot(isx.normal, -ray.direction);
    float r2 = glm::distance2(isx.point, ray.origin);
    float pdf = r2/(cosine*area);
    return pdf;
}
