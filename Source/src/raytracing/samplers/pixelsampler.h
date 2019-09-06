#pragma once
#include <la.h>
#include <QList>
#include <random>

class PixelSampler
{
protected:
    int samples_sqrt;//The square root of how many samples to take within the given pixel.
                    //In other words, when samples_sqrt = 5, 25 samples will be taken in the pixel
public:
    PixelSampler():samples_sqrt(1){}
    PixelSampler(int samples):samples_sqrt(samples){}
    void SetSampleCount(int samples){samples_sqrt = samples;}
    virtual ~PixelSampler(){}
    virtual QList<glm::vec2> GetSamples(int x, int y) = 0;
};
