#pragma once
#include <raytracing/samplers/pixelsampler.h>

class UniformPixelSampler : public PixelSampler
{
public:
    UniformPixelSampler();
    UniformPixelSampler(int samples);

    virtual QList<glm::vec2> GetSamples(int x, int y);
};
