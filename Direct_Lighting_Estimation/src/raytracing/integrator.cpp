#include <ctime>
#include <chrono>
#include <raytracing/integrator.h>

Integrator::Integrator():
    max_depth(5)
{
    scene = NULL;
    intersection_engine = NULL;
}

glm::vec3 ComponentMult(const glm::vec3 &a, const glm::vec3 &b)
{
    return glm::vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}

glm::vec3 Integrator::TraceRay(Ray r, unsigned int depth)
{
    glm::vec3 color(0);
    if(depth > max_depth)
        return color;

    Intersection isx = intersection_engine->GetIntersection(r);//get intersection with ray

    //hit out of range
    if((isx.t >= 0)){
        glm::vec3 base_color = isx.object_hit->material->base_color;

        //hit the light source
        if(isx.object_hit->material->is_light_source)
            return base_color;

        //generate random numbers
        std::mt19937 mg(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> u(-0.5, 0.5);

        for(int i = 0;i < scene->lights.size();i++)//for each light source
            for(int j = 0;j < sample;j++){//for each sample of specific light source
                Geometry* light = scene->lights[i];

                glm::vec2 rand(u(mg), u(mg));
                glm::vec3 randP = light->SampleLight(rand);//pick a random point on light source

                Ray wj(isx.point + epsilon*isx.normal, randP - isx.point);//generate a ray towards light source
                Intersection light_isx = intersection_engine->GetIntersection(wj);//get intersection with light source

                if(light_isx.object_hit == light){//hit the light source
                    float pdf =light->RayPDF(light_isx, wj);//get the light PDF

                    if(pdf){
                        glm::vec3 radiance = light->material->EvaluateScatteredEnergy(light_isx, r.direction, -wj.direction);
                        glm::vec3 BRDF = isx.object_hit->material->EvaluateScatteredEnergy(isx, r.direction, wj.direction);
                        color += BRDF*isx.texture_color*base_color*radiance*glm::dot(wj.direction, isx.normal)/pdf;
                    }
            }
        }
    }
    return color/float(sample);
}
