#include <scene/xmlreader.h>
#include <scene/geometry/mesh.h>
#include <scene/geometry/cube.h>
#include <scene/geometry/sphere.h>
#include <scene/geometry/square.h>
#include <scene/geometry/disc.h>
#include <iostream>
#include <scene/materials/material.h>
#include <scene/materials/lightmaterial.h>
#include <raytracing/samplers/uniformpixelsampler.h>
#include <raytracing/samplers/stratifiedpixelsampler.h>
#include <scene/materials/bxdfs/lambertBxDF.h>
#include <QImage>

void XMLReader::LoadSceneFromFile(QFile &file, const QStringRef &local_path, Scene &scene, Integrator &integrator)
{
    if(file.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader xml_reader;
        xml_reader.setDevice(&file);
        QMap<QString, QList<Geometry*>> material_to_geometry_map;
        QMap<QString, QList<Material*>> bxdf_to_material_map;//Key is the bxdf's name
        while(!xml_reader.isEndDocument())
        {
            xml_reader.readNext();
            if(xml_reader.isStartElement())
            {
                //Get the tag name
                QString tag(xml_reader.name().toString());
                if(QString::compare(tag, QString("camera")) == 0)
                {
                    scene.SetCamera(LoadCamera(xml_reader));
                }
                else if(QString::compare(tag, QString("geometry")) == 0)
                {
                    Geometry* geometry = LoadGeometry(xml_reader, material_to_geometry_map, local_path);
                    if(geometry == NULL)
                    {
                        return;
                    }
                    scene.objects.append(geometry);
                }
                else if(QString::compare(tag, QString("material")) == 0)
                {
                    Material* material = LoadMaterial(xml_reader, local_path, bxdf_to_material_map);
                    if(material == NULL)
                    {
                        return;
                    }
                    scene.materials.append(material);
                }
                else if(QString::compare(tag, QString("bxdf")) == 0)
                {
                    BxDF* bxdf = LoadBxDF(xml_reader);
                    if(bxdf == NULL)
                    {
                        return;
                    }
                    scene.bxdfs.append(bxdf);
                }
                else if(QString::compare(tag, QString("integrator")) == 0)
                {
                    integrator = LoadIntegrator(xml_reader);
                }
                else if(QString::compare(tag, QString("pixelSampleLength"), Qt::CaseInsensitive) == 0)
                {
                    scene.sqrt_samples = LoadPixelSamples(xml_reader);
                }
            }
        }
        //Associate the materials in the XML file with the geometries that use those materials.
        for(int i = 0; i < scene.materials.size(); i++)
        {
            QList<Geometry*> l = material_to_geometry_map.value(scene.materials[i]->name);
            for(int j = 0; j < l.size(); j++)
            {
                l[j]->SetMaterial(scene.materials[i]);
            }
        }

        for(int i = 0; i < scene.bxdfs.size(); i++)
        {
            QList<Material*> l = bxdf_to_material_map.value(scene.bxdfs[i]->name);
            for(int j = 0; j < l.size(); j++)
            {
                l[j]->bxdfs.append(scene.bxdfs[i]);
            }
        }

        //Copy emissive geometry from the list of objects to the list of lights
        QList<Geometry*> to_lights;
        for(Geometry *g : scene.objects)
        {
            g->create();
            if(g->material->is_light_source)
            {
                to_lights.append(g);
            }
            g->ComputeArea();
        }
        for(Geometry *g : to_lights)
        {
            scene.lights.append(g);
        }
        file.close();
    }
}

Geometry* XMLReader::LoadGeometry(QXmlStreamReader &xml_reader, QMap<QString, QList<Geometry*>> &map, const QStringRef &local_path)
{
    Geometry* result = NULL;

    //First check what type of geometry we're supposed to load
    QXmlStreamAttributes attribs(xml_reader.attributes());
    QStringRef type = attribs.value(QString(), QString("type"));
    bool is_mesh = false;
    if(QStringRef::compare(type, QString("obj")) == 0)
    {
        result = new Mesh();
        is_mesh = true;
    }
    else if(QStringRef::compare(type, QString("sphere")) == 0)
    {
        result = new Sphere();
    }
    else if(QStringRef::compare(type, QString("square")) == 0)
    {
        result = new SquarePlane();
    }
    else if(QStringRef::compare(type, QString("cube")) == 0)
    {
        result = new Cube();
    }
    else if(QStringRef::compare(type, QString("disc")) == 0)
    {
        result = new Disc();
    }
    else
    {
        std::cout << "Could not parse the geometry!" << std::endl;
        return NULL;
    }
    QStringRef name = attribs.value(QString(), QString("name"));
    result->name = name.toString();

    while(!xml_reader.isEndElement() || QStringRef::compare(xml_reader.name(), QString("geometry")) != 0)
    {
        xml_reader.readNext();// xml_reader.readNext();

        QString tag(xml_reader.name().toString());
        if(is_mesh && QString::compare(tag, QString("filename")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                ((Mesh*)result)->LoadOBJ(xml_reader.text(), local_path);
            }
            xml_reader.readNext();
        }
        else if(QString::compare(tag, QString("transform")) == 0)
        {
            result->transform = LoadTransform(xml_reader);
            xml_reader.readNext();
        }
        else if(QString::compare(tag, QString("material")) == 0)
        {
            //Add the geometry to the map of material names to geometries so that we can assign it a material later
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                QString material_name = xml_reader.text().toString();
                QList<Geometry*> list = map.value(material_name);
                list.append(result);
                map.insert(material_name, list);
                xml_reader.readNext();

            }
//            attribs = QXmlStreamAttributes(xml_reader.attributes());
//            QString name = attribs.value(QString(), QString("value")).toString();
//            QList<Geometry*> list = map.value(name);
//            list.append(result);
//            map.insert(name, list);
//            xml_reader.readNext();
        }
    }
    return result;
}

Material* XMLReader::LoadMaterial(QXmlStreamReader &xml_reader, const QStringRef &local_path, QMap<QString, QList<Material *> > &map)
{
    Material* result;
    //First check what type of material we're supposed to load
    QXmlStreamAttributes attribs(xml_reader.attributes());
    QStringRef type = attribs.value(QString(), QString("type"));
    if(QStringRef::compare(type, QString("default")) == 0)
    {
        result = new Material();
    }
    else if(QStringRef::compare(type, QString("light")) == 0)
    {
        result = new LightMaterial();
        result->is_light_source = true;
        QStringRef intensity = attribs.value(QString(), QString("intensity"));
        if(QStringRef::compare(intensity, QString("")) != 0)
        {
            result->intensity = intensity.toFloat();
        }
    }
    else
    {
        std::cout << "Could not parse the material!" << std::endl;
        return NULL;
    }

    result->name = attribs.value(QString(), QString("name")).toString();

    while(!xml_reader.isEndElement() || QStringRef::compare(xml_reader.name(), QString("material")) != 0)
    {
        xml_reader.readNext();
        QString tag(xml_reader.name().toString());
        if(QString::compare(tag, QString("baseColor")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                result->base_color = ToVec3(xml_reader.text());
            }
            xml_reader.readNext();
        }
        else if(QString::compare(tag, QString("bxdf")) == 0)
        {
            //Add the Material to the map of BxDF names to Materials so that we can assign it a BxDF later
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                QString bxdf_name = xml_reader.text().toString();
                QList<Material*> list = map.value(bxdf_name);
                list.append(result);
                map.insert(bxdf_name, list);
                xml_reader.readNext();

            }
        }
        else if(QString::compare(tag, QString("texture")) == 0)
        {
            result->texture = LoadTextureFile(xml_reader, local_path);
        }
        else if(QString::compare(tag, QString("normalMap")) == 0)
        {
            result->normal_map = LoadTextureFile(xml_reader, local_path);
        }
    }
    return result;
}

Camera XMLReader::LoadCamera(QXmlStreamReader &xml_reader)
{
    Camera result;
    while(!xml_reader.isEndElement() || QStringRef::compare(xml_reader.name(), QString("camera")) != 0)
    {
        xml_reader.readNext();
        QString tag(xml_reader.name().toString());


        if(QString::compare(tag, QString("target")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                result.ref = ToVec3(xml_reader.text());
            }
            xml_reader.readNext();
        }
        else if(QString::compare(tag, QString("eye")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                result.eye = ToVec3(xml_reader.text());
            }
            xml_reader.readNext();
        }
        else if(QString::compare(tag, QString("worldUp")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                result.world_up = ToVec3(xml_reader.text());
            }
            xml_reader.readNext();
        }
        else if(QString::compare(tag, QString("width")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                result.width = xml_reader.text().toFloat();
            }
            xml_reader.readNext();
        }
        else if(QString::compare(tag, QString("height")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                result.height = xml_reader.text().toFloat();
            }
            xml_reader.readNext();
        }
        else if(QString::compare(tag, QString("fov")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                result.fovy = xml_reader.text().toFloat();
            }
            xml_reader.readNext();
        }
        else if(QString::compare(tag, QString("nearClip")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                result.near_clip = xml_reader.text().toFloat();
            }
            xml_reader.readNext();
        }
        else if(QString::compare(tag, QString("farClip")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                result.far_clip = xml_reader.text().toFloat();
            }
            xml_reader.readNext();
        }
    }
    result.RecomputeAttributes();
    return result;
}

Transform XMLReader::LoadTransform(QXmlStreamReader &xml_reader)
{
    glm::vec3 t, r, s;
    s = glm::vec3(1,1,1);
    while(!xml_reader.isEndElement() || QStringRef::compare(xml_reader.name(), QString("transform")) != 0)
    {
        xml_reader.readNext();
        QString tag(xml_reader.name().toString());
        if(xml_reader.isStartElement() && QString::compare(tag, QString("translate")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                QStringRef vec = xml_reader.text();
                t = ToVec3(vec);
            }
            xml_reader.readNext();
        }
        else if(xml_reader.isStartElement() && QString::compare(tag, QString("rotate")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                QStringRef vec = xml_reader.text();
                r = ToVec3(vec);
            }
            xml_reader.readNext();
        }
        else if(xml_reader.isStartElement() && QString::compare(tag, QString("scale")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                QStringRef vec = xml_reader.text();
                s = ToVec3(vec);
            }
            xml_reader.readNext();
        }
    }
    return Transform(t, r, s);
}

Integrator XMLReader::LoadIntegrator(QXmlStreamReader &xml_reader)
{
    Integrator result;

    //First check what type of integrator we're supposed to load
    QXmlStreamAttributes attribs(xml_reader.attributes());
    QStringRef type = attribs.value(QString(), QString("type"));
    bool is_mesh = false;

    while(!xml_reader.isEndElement() || QStringRef::compare(xml_reader.name(), QString("integrator")) != 0)
    {
        xml_reader.readNext();

        QString tag(xml_reader.name().toString());
        if(is_mesh && QString::compare(tag, QString("maxDepth")) == 0)
        {
            xml_reader.readNext();
            if(xml_reader.isCharacters())
            {
                result.SetDepth(xml_reader.text().toInt());
            }
            xml_reader.readNext();
        }
    }
    return result;
}

unsigned int XMLReader::LoadPixelSamples(QXmlStreamReader &xml_reader)
{
    while(!xml_reader.isEndElement() || QStringRef::compare(xml_reader.name(), QString("pixelSampleLength")) != 0)
    {
        xml_reader.readNext();
        if(xml_reader.isCharacters())
        {
            return xml_reader.text().toInt();
        }
        xml_reader.readNext();
    }
}


QImage* XMLReader::LoadTextureFile(QXmlStreamReader &xml_reader, const QStringRef &local_path)
{
    xml_reader.readNext();
    QImage* texture = NULL;
    if(xml_reader.isCharacters())
    {
        QString img_filepath = local_path.toString().append(xml_reader.text().toString());
        texture = new QImage(img_filepath);
    }
    xml_reader.readNext();
    return texture;
}


BxDF* XMLReader::LoadBxDF(QXmlStreamReader &xml_reader)
{
    BxDF* result = NULL;
    //First check what type of material we're supposed to load
    QXmlStreamAttributes attribs(xml_reader.attributes());
    QStringRef type = attribs.value(QString(), QString("type"));
    if(QStringRef::compare(type, QString("lambert")) == 0)
    {
        glm::vec3 diffuseColor(0.5f);
        QStringRef color = attribs.value(QString(), QString("diffuseColor"));
        if(QStringRef::compare(color, QString("")) != 0)
        {
            diffuseColor = ToVec3(color);
        }
        result = new LambertBxDF(diffuseColor);
    }
    else
    {
        std::cout << "Could not parse the BxDF!" << std::endl;
        return NULL;
    }
    result->name = attribs.value(QString(), QString("name")).toString();
    return result;
}

glm::vec3 XMLReader::ToVec3(const QStringRef &s)
{
    glm::vec3 result;
    int start_idx;
    int end_idx = -1;
    for(int i = 0; i < 3; i++){
        start_idx = ++end_idx;
        while(end_idx < s.length() && s.at(end_idx) != QChar(' '))
        {
            end_idx++;
        }
        result[i] = s.mid(start_idx, end_idx - start_idx).toFloat();
    }
    return result;
}
