#pragma once
#include <QList>
#include <raytracing/ray.h>
#include <scene/scene.h>

class Scene;
class Ray;


class IntersectionEngine
{
public:
    IntersectionEngine();
    Scene *scene;

    Intersection GetIntersection(Ray r);
    QList<Intersection> GetAllIntersections(Ray r);
};
