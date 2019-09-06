#include <scene/materials/bxdfs/bxdf.h>

glm::vec3 BxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{
    //TODO
    wi_ret = glm::vec3(0);
    pdf_ret = 0.0f;
    return glm::vec3(0);
}

glm::vec3 BxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2* samples) const
{
    //TODO
    return glm::vec3(0);
}

float BxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO
    return 0.0f;
}
