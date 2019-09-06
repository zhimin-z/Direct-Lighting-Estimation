#pragma once
#include <la.h>

enum BxDFType {
    BSDF_REFLECTION   = 1<<0,
    BSDF_TRANSMISSION = 1<<1,
    BSDF_DIFFUSE      = 1<<2,
    BSDF_GLOSSY       = 1<<3,
    BSDF_SPECULAR     = 1<<4,
    BSDF_ALL_TYPES        = BSDF_DIFFUSE |
                            BSDF_GLOSSY |
                            BSDF_SPECULAR,
    BSDF_ALL_REFLECTION   = BSDF_REFLECTION |
                            BSDF_ALL_TYPES,
    BSDF_ALL_TRANSMISSION = BSDF_TRANSMISSION |
                            BSDF_ALL_TYPES,
    BSDF_ALL              = BSDF_ALL_REFLECTION |
                            BSDF_ALL_TRANSMISSION
};

//An abstract class from which specific BRDF and BTDF types inherit
//Contains functions necessary for the evaluation of reflected/transmitted light energy
//All functions using wo and wi are assumed to operate around a surface normal of <0 0 1>
class BxDF
{
public:
//Constructors/Destructors
    BxDF(BxDFType t) : type(t), name("BxDF"){}
    virtual ~BxDF(){}
//Functions
    //This evaluates the BxDF given the outgoing and incoming light directions. In its most basic form,
    //it is a base color multiplied by the contribution factor of wi given wo.
    //It MUST be implemented by subclasses
    virtual glm::vec3 EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const = 0;

    //This generates an incoming light direction wi based on rand1 and rand2 and returns the result of EvaluateScatteredEnergy based on wi.
    //It "returns" wi by storing it in the supplied reference to wi. Likewise, it "returns" the value of its PDF given wi and wo in the reference to pdf.
    virtual glm::vec3 SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const;

    //Given a set of pairs of random numbers (samples), create N sample points on the hemisphere and evaluate the BxDF given wo and each of these sampled wi.
    //Equivalent to the RHO term in Monte Carlo path tracing
    //The default implementation generates wi based on cosine-weighted hemisphere sampling
    virtual glm::vec3 EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2* samples) const;

    //A shorthand way of calling EvaluateHemisphereScatteredEnergy
    glm::vec3 Rho(const glm::vec3 &wo, int num_samples, const glm::vec2* samples) const {return EvaluateHemisphereScatteredEnergy(wo, num_samples, samples);}

    //Returns how likely wo is to be reflected in direction wi. If a BxDF subclass overrides the functionality of SampleAndEvaluateBxDF,
    //it MUST also override the functionality of PDF in order to keep the two paired together and consistent.
    //The default implementation returns the cosine weight of wi relative to the "default" surface normal
    virtual float PDF(const glm::vec3 &wo, const glm::vec3 &wi) const;
//Member variables
    BxDFType type;
    QString name;
};
