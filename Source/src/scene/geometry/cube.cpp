#include "cube.h"
#include <la.h>
#include <iostream>

static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;

glm::vec3 Cube::SampleLight(glm::vec2 rand)
{
    int i = (rand[0] + 0.5)*12;
    glm::vec3 cube[12] = {
        glm::vec3(rand[0], rand[1], -0.5),
        glm::vec3(0.5, rand[0], rand[1]),
        glm::vec3(rand[1], rand[0], -0.5),
        glm::vec3(rand[0], 0.5, rand[1]),
        glm::vec3(0.5, rand[1], rand[0]),
        glm::vec3(rand[1], 0.5, rand[0]),
        glm::vec3(rand[0], -0.5, rand[1]),
        glm::vec3(-0.5, rand[1], rand[0]),
        glm::vec3(rand[1], rand[0], 0.5),
        glm::vec3(rand[0], rand[1], 0.5),
        glm::vec3(-0.5, rand[0], rand[1]),
        glm::vec3(rand[1], -0.5, rand[0]),
    };
    glm::vec3 point = glm::vec3(transform.T()*glm::vec4(cube[i],1));
    return point;
}

void Cube::ComputeArea()
{
    //Extra credit to implement this
    glm::vec3 scale = transform.GetScale();
    area = 2*(scale[0]*scale[1] + scale[1]*scale[2] +scale[2]*scale[0]);
}

glm::vec4 GetCubeNormal(const glm::vec4& P)
{
    int idx = 0;
    float val = -1;
    for(int i = 0; i < 3; i++){
        if(glm::abs(P[i]) > val){
            idx = i;
            val = glm::abs(P[i]);
        }
    }
    glm::vec4 N(0,0,0,0);
    N[idx] = glm::sign(P[idx]);
    return N;
}

glm::vec3 Cube::ComputeNormal(const glm::vec3 &P)
{return glm::vec3(0);}

Intersection Cube::GetIntersection(Ray r)
{
    //Transform the ray
    Ray r_loc = r.GetTransformedCopy(transform.invT());
    Intersection result;

    float t_n = -1000000;
    float t_f = 1000000;
    for(int i = 0; i < 3; i++){
        //Ray parallel to slab check
        if(r_loc.direction[i] == 0){
            if(r_loc.origin[i] < -0.5f || r_loc.origin[i] > 0.5f){
                return result;
            }
        }
        //If not parallel, do slab intersect check
        float t0 = (-0.5f - r_loc.origin[i])/r_loc.direction[i];
        float t1 = (0.5f - r_loc.origin[i])/r_loc.direction[i];
        if(t0 > t1){
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }
        if(t0 > t_n){
            t_n = t0;
        }
        if(t1 < t_f){
            t_f = t1;
        }
    }
    float t_final = -1;
    if(t_n < t_f)
    {
        if(t_n >= 0)
        {
            t_final = t_n;
        }
        else if(t_f >= 0)
        {
            t_final = t_f;
        }
    }
    if(t_final >= 0)
    {
        //Lastly, transform the point found in object space by T
        glm::vec4 P = glm::vec4(r_loc.origin + t_final*r_loc.direction, 1);
        result.point = glm::vec3(transform.T() * P);
        result.normal = glm::normalize(glm::vec3(transform.invTransT() * GetCubeNormal(P)));
        result.object_hit = this;
        result.t = glm::distance(result.point, r.origin);
        result.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(P)), material->texture);
        //TODO: Store the tangent and bitangent
        glm::vec3 normal = glm::vec3(GetCubeNormal(P));

        if((normal == glm::vec3(0,0,1)) || (normal == glm::vec3(0,0,-1))){
            result.tangent = glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(1,0,0,0)));
            result.bitangent = glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(0,1,0,0)));
        }
        else if((normal == glm::vec3(0,1,0)) || (normal == glm::vec3(0,-1,0))){
            result.tangent = glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(0,0,1,0)));
            result.bitangent = glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(1,0,0,0)));
        }
        else{
            result.tangent = glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(0,1,0,0)));
            result.bitangent = glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(0,0,1,0)));
        }
    }
    return result;
}

glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point)
{
    glm::vec3 abs = glm::min(glm::abs(point), 0.5f);
    glm::vec2 UV;//Always offset lower-left corner
    if(abs.x > abs.y && abs.x > abs.z)
    {
        UV = glm::vec2(point.z + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.x < 0)
        {
            UV += glm::vec2(0, 0.333f);
        }
        else
        {
            UV += glm::vec2(0, 0.667f);
        }
    }
    else if(abs.y > abs.x && abs.y > abs.z)
    {
        UV = glm::vec2(point.x + 0.5f, point.z + 0.5f)/3.0f;
        //Left face
        if(point.y < 0)
        {
            UV += glm::vec2(0.333f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.333f, 0.667f);
        }
    }
    else
    {
        UV = glm::vec2(point.x + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.z < 0)
        {
            UV += glm::vec2(0.667f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.667f, 0.667f);
        }
    }
    return UV;
}

//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createCubeVertexPositions(glm::vec3 (&cub_vert_pos)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);

    //Right face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);

    //Left face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Back face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);

    //Top face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Bottom face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
}


void createCubeVertexNormals(glm::vec3 (&cub_vert_nor)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,1);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(1,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(-1,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,-1);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,1,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,-1,0);
    }
}

void createCubeIndices(GLuint (&cub_idx)[CUB_IDX_COUNT])
{
    int idx = 0;
    for(int i = 0; i < 6; i++){
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+1;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4+3;
    }
}

void Cube::create()
{
    GLuint cub_idx[CUB_IDX_COUNT];
    glm::vec3 cub_vert_pos[CUB_VERT_COUNT];
    glm::vec3 cub_vert_nor[CUB_VERT_COUNT];
    glm::vec3 cub_vert_col[CUB_VERT_COUNT];

    createCubeVertexPositions(cub_vert_pos);
    createCubeVertexNormals(cub_vert_nor);
    createCubeIndices(cub_idx);

    for(int i = 0; i < CUB_VERT_COUNT; i++){
        cub_vert_col[i] = material->base_color;
    }

    count = CUB_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, CUB_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos,CUB_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, CUB_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, CUB_VERT_COUNT * sizeof(glm::vec3));

}
