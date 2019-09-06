#include <scene/materials/bxdfs/lambertBxDF.h>

glm::vec3 LambertBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO
    return diffuse_color/PI;
}
glm::vec3 LambertBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return glm::vec3(0);
}
