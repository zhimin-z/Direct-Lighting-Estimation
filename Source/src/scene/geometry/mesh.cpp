#include <scene/geometry/mesh.h>
#include <la.h>
#include <tinyobj/tiny_obj_loader.h>
#include <iostream>

float Area(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
{
    return glm::length(glm::cross(p1 - p2, p3 - p2)) * 0.5f;
}

glm::vec3 Triangle::SampleLight(glm::vec2 rand)
{
    glm::vec3 point = points[0]*float(fabs(rand[0])) + points[1]*float(fabs(rand[1])) + points[2]*float(1 - fabs(rand[1]) - fabs(rand[0]));
    return point;
}

glm::vec3 Mesh::SampleLight(glm::vec2 rand)
{
    int i = faces.size()*(0.5 + rand[0]);
    glm::vec3 point = faces[i]->SampleLight(rand);
    point = glm::vec3(transform.T()*glm::vec4(point,1));
    return point;
}

void Triangle::ComputeArea()
{
    //Extra credit to implement this
    area = Area(points[0], points[1], points[2]);
}

void Mesh::ComputeArea()
{
    //Extra credit to implement this
    for(int i = 0; i < faces.size(); i++){
        faces[i]->ComputeArea();
        area += faces[i]->area;
    }
}

Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3):
    Triangle(p1, p2, p3, glm::vec3(1), glm::vec3(1), glm::vec3(1), glm::vec2(0), glm::vec2(0), glm::vec2(0))
{
    for(int i = 0; i < 3; i++)
    {
        normals[i] = plane_normal;
    }
}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3):
    Triangle(p1, p2, p3, n1, n2, n3, glm::vec2(0), glm::vec2(0), glm::vec2(0))
{}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3, const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3)
{
    plane_normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    normals[0] = n1;
    normals[1] = n2;
    normals[2] = n3;
    uvs[0] = t1;
    uvs[1] = t2;
    uvs[2] = t3;
}

//Returns the interpolation of the triangle's three normals based on the point inside the triangle that is given.
glm::vec3 Triangle::GetNormal(const glm::vec3 &P)
{
    float A = Area(points[0], points[1], points[2]);
    float A0 = Area(points[1], points[2], P);
    float A1 = Area(points[0], points[2], P);
    float A2 = Area(points[0], points[1], P);
    return glm::normalize(normals[0] * A0/A + normals[1] * A1/A + normals[2] * A2/A);
}

glm::vec4 Triangle::GetNormal(const glm::vec4 &position)
{
    glm::vec3 result = GetNormal(glm::vec3(position));
    return glm::vec4(result, 0);
}

glm::vec3 Triangle::ComputeNormal(const glm::vec3 &P)
{}

glm::vec3 Mesh::ComputeNormal(const glm::vec3 &P)
{}

//HAVE THEM IMPLEMENT THIS
//The ray in this function is not transformed because it was *already* transformed in Mesh::GetIntersection
Intersection Triangle::GetIntersection(Ray r){
    //1. Ray-plane intersection
    Intersection result;
    float t =  glm::dot(plane_normal, (points[0] - r.origin)) / glm::dot(plane_normal, r.direction);

    glm::vec3 P = r.origin + t * r.direction;
    //2. Barycentric test
    float S = 0.5f * glm::length(glm::cross(points[0] - points[1], points[0] - points[2]));
    float s1 = 0.5f * glm::length(glm::cross(P - points[1], P - points[2]))/S;
    float s2 = 0.5f * glm::length(glm::cross(P - points[2], P - points[0]))/S;
    float s3 = 0.5f * glm::length(glm::cross(P - points[0], P - points[1]))/S;
    float sum = s1 + s2 + s3;

    if(s1 >= 0 && s1 <= 1 && s2 >= 0 && s2 <= 1 && s3 >= 0 && s3 <= 1 && fequal(sum, 1.0f)){
        result.t = t;
        result.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(P)), material->texture);
        result.object_hit = this;
        //TODO: Store the tangent and bitangent
        glm::vec3 dists[2] = {points[1] - points[0], points[2] - points[0]};
        glm::vec2 uv_points[3] = {GetUVCoordinates(points[1]), GetUVCoordinates(points[1]), GetUVCoordinates(points[2])};
        glm::vec2 uv_dists[2] = {uv_points[1] - uv_points[0], uv_points[2] - uv_points[0]};

        result.tangent = (uv_dists[1][1]*dists[0] - uv_dists[0][1]*dists[1])/(uv_dists[1][1]*uv_dists[0][0] - uv_dists[0][1]*uv_dists[1][0]);
        result.bitangent = (dists[1] - uv_dists[1][0]*result.tangent)/uv_dists[1][1];
    }
    return result;
}

Intersection Mesh::GetIntersection(Ray r){
    Ray r_loc = r.GetTransformedCopy(transform.invT());
    Intersection closest;
    for(int i = 0; i < faces.size(); i++){
        Intersection isx = faces[i]->GetIntersection(r_loc);
        if(isx.object_hit != NULL && isx.t > 0 && (isx.t < closest.t || closest.t < 0)){
            closest = isx;
        }
    }
    if(closest.object_hit != NULL)
    {
        Triangle* tri = (Triangle*)closest.object_hit;
        glm::vec4 P = glm::vec4(closest.t * r_loc.direction + r_loc.origin, 1);
        closest.point = glm::vec3(transform.T() * P);
        closest.normal = glm::normalize(glm::vec3(transform.invTransT() * tri->GetNormal(P)));
        closest.object_hit = this;
        closest.t = glm::distance(closest.point, r.origin);//The t used for the closest triangle test was in object space
        //TODO: Store the tangent and bitangent
        closest.tangent = glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(closest.tangent,0)));
        closest.bitangent = glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(closest.bitangent,0)));
    }
    return closest;
}

void Mesh::SetMaterial(Material *m)
{
    this->material = m;
    for(Triangle *t : faces)
    {
        t->SetMaterial(m);
    }
}

glm::vec2 Mesh::GetUVCoordinates(const glm::vec3 &point)
{
    return glm::vec2();
}


glm::vec2 Triangle::GetUVCoordinates(const glm::vec3 &point)
{
    float A = Area(points[0], points[1], points[2]);
    float A0 = Area(points[1], points[2], point);
    float A1 = Area(points[0], points[2], point);
    float A2 = Area(points[0], points[1], point);
    return uvs[0] * A0/A + uvs[1] * A1/A + uvs[2] * A2/A;
}

void Mesh::LoadOBJ(const QStringRef &filename, const QStringRef &local_path)
{
    QString filepath = local_path.toString(); filepath.append(filename);
    std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
    std::string errors = tinyobj::LoadObj(shapes, materials, filepath.toStdString().c_str());
    std::cout << errors << std::endl;
    if(errors.size() == 0)
    {
        //Read the information from the vector of shape_ts
        for(unsigned int i = 0; i < shapes.size(); i++)
        {
            std::vector<float> &positions = shapes[i].mesh.positions;
            std::vector<float> &normals = shapes[i].mesh.normals;
            std::vector<float> &uvs = shapes[i].mesh.texcoords;
            std::vector<unsigned int> &indices = shapes[i].mesh.indices;
            for(unsigned int j = 0; j < indices.size(); j += 3)
            {
                glm::vec3 p1(positions[indices[j]*3], positions[indices[j]*3+1], positions[indices[j]*3+2]);
                glm::vec3 p2(positions[indices[j+1]*3], positions[indices[j+1]*3+1], positions[indices[j+1]*3+2]);
                glm::vec3 p3(positions[indices[j+2]*3], positions[indices[j+2]*3+1], positions[indices[j+2]*3+2]);

                Triangle* t = new Triangle(p1, p2, p3);
                if(normals.size() > 0)
                {
                    glm::vec3 n1(normals[indices[j]*3], normals[indices[j]*3+1], normals[indices[j]*3+2]);
                    glm::vec3 n2(normals[indices[j+1]*3], normals[indices[j+1]*3+1], normals[indices[j+1]*3+2]);
                    glm::vec3 n3(normals[indices[j+2]*3], normals[indices[j+2]*3+1], normals[indices[j+2]*3+2]);
                    t->normals[0] = n1;
                    t->normals[1] = n2;
                    t->normals[2] = n3;
                }
                if(uvs.size() > 0)
                {
                    glm::vec2 t1(uvs[indices[j]*2], uvs[indices[j]*2+1]);
                    glm::vec2 t2(uvs[indices[j+1]*2], uvs[indices[j+1]*2+1]);
                    glm::vec2 t3(uvs[indices[j+2]*2], uvs[indices[j+2]*2+1]);
                    t->uvs[0] = t1;
                    t->uvs[1] = t2;
                    t->uvs[2] = t3;
                }
                this->faces.append(t);
            }
        }
        std::cout << "" << std::endl;
        //TODO: .mtl file loading
    }
    else
    {
        //An error loading the OBJ occurred!
        std::cout << errors << std::endl;
    }
}

void Mesh::create(){
    //Count the number of vertices for each face so we can get a count for the entire mesh
        std::vector<glm::vec3> vert_pos;
        std::vector<glm::vec3> vert_nor;
        std::vector<glm::vec3> vert_col;
        std::vector<GLuint> vert_idx;

        unsigned int index = 0;

        for(int i = 0; i < faces.size(); i++){
            Triangle* tri = faces[i];
            vert_pos.push_back(tri->points[0]); vert_nor.push_back(tri->normals[0]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[1]); vert_nor.push_back(tri->normals[1]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[2]); vert_nor.push_back(tri->normals[2]); vert_col.push_back(tri->material->base_color);
            vert_idx.push_back(index++);vert_idx.push_back(index++);vert_idx.push_back(index++);
        }

        count = vert_idx.size();
        int vert_count = vert_pos.size();

        bufIdx.create();
        bufIdx.bind();
        bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufIdx.allocate(vert_idx.data(), count * sizeof(GLuint));

        bufPos.create();
        bufPos.bind();
        bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufPos.allocate(vert_pos.data(), vert_count * sizeof(glm::vec3));

        bufCol.create();
        bufCol.bind();
        bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufCol.allocate(vert_col.data(), vert_count * sizeof(glm::vec3));

        bufNor.create();
        bufNor.bind();
        bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufNor.allocate(vert_nor.data(), vert_count * sizeof(glm::vec3));
}

//This does nothing because individual triangles are not rendered with OpenGL; they are rendered all together in their Mesh.
void Triangle::create(){}
