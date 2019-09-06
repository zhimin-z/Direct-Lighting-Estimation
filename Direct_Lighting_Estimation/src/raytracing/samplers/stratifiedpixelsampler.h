#pragma once
#include <raytracing/samplers/pixelsampler.h>
#include <random>

class StratifiedPixelSampler : public PixelSampler
{
public:
    StratifiedPixelSampler();
    StratifiedPixelSampler(unsigned int samples, unsigned int seed);
    virtual QList<glm::vec2> GetSamples(int x, int y);

protected:
    std::mt19937 mersenne_generator;
    std::uniform_real_distribution<float> unif_distribution;
};
