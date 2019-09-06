#include <raytracing/samplers/stratifiedpixelsampler.h>
#include <iostream>
#include <functional>

StratifiedPixelSampler::StratifiedPixelSampler():PixelSampler(){}

StratifiedPixelSampler::StratifiedPixelSampler(unsigned int samples, unsigned int seed) : PixelSampler(samples), mersenne_generator(seed), unif_distribution(0,1){}

QList<glm::vec2> StratifiedPixelSampler::GetSamples(int x, int y)
{
    QList<glm::vec2> result;
    //1. Divide the given pixel into subsections
    for(float i = 0; i < samples_sqrt; i++)
    {
        for(float j = 0; j < samples_sqrt; j++)
        {
            //Get a random sample within the sub-pixel square
            float dx = x + i/samples_sqrt;
            float dy = y + j/samples_sqrt;

            float randX = unif_distribution(mersenne_generator);
            float randY = unif_distribution(mersenne_generator);

            float offset_x = randX/samples_sqrt;
            float offset_y = randY/samples_sqrt;

            result.append(glm::vec2(dx + offset_x, dy + offset_y));
        }
    }
    return result;
}
