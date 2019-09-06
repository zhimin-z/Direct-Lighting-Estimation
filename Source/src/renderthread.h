#pragma once
#include <QThread>
#include <raytracing/film.h>
#include <scene/scene.h>
#include <raytracing/integrator.h>

glm::vec3 ComponentMult(const glm::vec3 &a, const glm::vec3 &b);

class RenderThread : public QThread
{
public:
    RenderThread(unsigned int xstart, unsigned int xend,
            unsigned int ystart, unsigned int yend,
            unsigned int samplesSqrt, unsigned int depth,
            Film* f, Camera* c, Integrator* i);

protected:
    //This overrides the functionality of QThread::run
    virtual void run();
    glm::vec3 TraceRay(Ray r, unsigned int depth);// IntersectionEngine* intersection_engine, Scene* scene);

    unsigned int x_start, x_end, y_start, y_end;
    unsigned int samples_sqrt;//The square root of the number of rays to cast per pixel
    unsigned int max_depth;
    Film* film;
    Camera* camera;
    Integrator* integrator;
};
