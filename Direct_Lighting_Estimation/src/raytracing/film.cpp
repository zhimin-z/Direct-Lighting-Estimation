#include <raytracing/film.h>
#include <bmp/EasyBMP.h>

Film::Film() : Film(400, 400){}

Film::Film(unsigned int width, unsigned int height)
{
    SetDimensions(width, height);
}

void Film::SetDimensions(unsigned int w, unsigned int h)
{
    this->width = w;
    this->height = h;
    pixels.clear();
    pixels = std::vector<std::vector<glm::vec3>>(width);
    for(unsigned int i = 0; i < width; i++){
        pixels[i] = std::vector<glm::vec3>(height);
    }
}

void Film::WriteImage(QString path){
    if(QString::compare(path.right(4), QString(".bmp"), Qt::CaseInsensitive) != 0)
    {
        path.append(QString(".bmp"));
    }
    WriteImage(path.toStdString());
}

void Film::WriteImage(const std::string &path){
    BMP output;
    output.SetSize(width, height);
    output.SetBitDepth(24);

    for(unsigned int i = 0; i < width; i++) {
        for(unsigned int j = 0; j < height; j++) {
            glm::vec3 color = pixels[i][j];
            output(i, j)->Red   = glm::clamp(color.r, 0.0f, 1.0f)*255;
            output(i, j)->Green = glm::clamp(color.g, 0.0f, 1.0f)*255;
            output(i, j)->Blue  = glm::clamp(color.b, 0.0f, 1.0f)*255;
        }
    }
    output.WriteToFile(path.c_str());
}
