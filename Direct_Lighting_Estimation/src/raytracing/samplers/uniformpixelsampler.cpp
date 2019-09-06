#include <raytracing/samplers/uniformpixelsampler.h>

UniformPixelSampler::UniformPixelSampler():PixelSampler()
{}

UniformPixelSampler::UniformPixelSampler(int samples):PixelSampler(samples)
{}

QList<glm::vec2> UniformPixelSampler::GetSamples(int x, int y)
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
            result.append(glm::vec2(dx, dy));
        }
    }
    return result;
}
