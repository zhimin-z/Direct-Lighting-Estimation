#pragma once
#include <scene/materials/material.h>

class LightMaterial : public Material
{
public:
    //Already implemented. Just returns the emitted light color * intensity
    virtual glm::vec3 EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDFType flags = BSDF_ALL) const;

    //Given an intersection with some geometry, generate a point on the geometry to which this material is applied and
    glm::vec3 SampleLight(const Intersection &geom_isx, glm::vec3 &wi_ret, float &light_pdf_ret, bool &visibility_ret) const;
};
