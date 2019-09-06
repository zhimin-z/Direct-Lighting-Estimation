#include <renderthread.h>
#include <raytracing/samplers/stratifiedpixelsampler.h>

RenderThread::RenderThread(unsigned int xstart, unsigned int xend, unsigned int ystart, unsigned int yend, unsigned int samplesSqrt, unsigned int depth, Film *f, Camera *c, Integrator *i)
    : x_start(xstart), x_end(xend), y_start(ystart), y_end(yend), samples_sqrt(samplesSqrt), max_depth(depth), film(f), camera(c), integrator(i)
{}

void RenderThread::run()
{
    unsigned int seed = (((x_start << 16 | x_end) ^ x_start) * ((y_start << 16 | y_end) ^ y_start));
    StratifiedPixelSampler pixel_sampler(samples_sqrt, seed);

    for(unsigned int Y = y_start; Y < y_end; Y++)
    {
        for(unsigned int X = x_start; X < x_end; X++)
        {
            glm::vec3 pixel_color;
            QList<glm::vec2> samples = pixel_sampler.GetSamples(X, Y);
            for(int i = 0; i < samples.size(); i++)
            {
                Ray ray = camera->Raycast(samples[i]);
                pixel_color += integrator->TraceRay(ray, 0);
            }
            pixel_color /= samples.size();
            film->pixels[X][Y] = pixel_color;
        }
    }
}
