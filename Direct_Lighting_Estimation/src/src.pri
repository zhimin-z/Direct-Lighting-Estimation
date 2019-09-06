INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
#LIBS += -L$$PWD/lib -ltbb

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/mygl.cpp \
    $$PWD/scene/camera.cpp \
    $$PWD/scene/scene.cpp \
    $$PWD/bmp/EasyBMP.cpp \
    $$PWD/scene/geometry/cube.cpp \
    $$PWD/scene/geometry/mesh.cpp \
    $$PWD/scene/geometry/sphere.cpp \
    $$PWD/openGL/drawable.cpp \
    $$PWD/openGL/glwidget277.cpp \
    $$PWD/openGL/shaderprogram.cpp \
    $$PWD/raytracing/intersection.cpp \
    $$PWD/raytracing/ray.cpp \
    $$PWD/scene/transform.cpp \
    $$PWD/scene/geometry/square.cpp \
    $$PWD/tinyobj/tiny_obj_loader.cc \
    $$PWD/scene/materials/material.cpp \
    $$PWD/raytracing/film.cpp \
    $$PWD/scene/xmlreader.cpp \
    $$PWD/raytracing/integrator.cpp \
    $$PWD/cameracontrolshelp.cpp \
    $$PWD/raytracing/samplers/stratifiedpixelsampler.cpp \
    $$PWD/raytracing/samplers/uniformpixelsampler.cpp \
    $$PWD/scene/geometry/disc.cpp \
    $$PWD/scene/materials/bxdfs/lambertBxDF.cpp \
    $$PWD/scene/materials/bxdfs/bxdf.cpp \
    $$PWD/scene/materials/lightmaterial.cpp \
    $$PWD/renderthread.cpp \
    $$PWD/scene/geometry/geometry.cpp

HEADERS += \
    $$PWD/mainwindow.h \
    $$PWD/mygl.h \
    $$PWD/scene/camera.h \
    $$PWD/la.h \
    $$PWD/drawable.h \
    $$PWD/scene/scene.h \
    $$PWD/bmp/EasyBMP.h \
    $$PWD/bmp/EasyBMP_BMP.h \
    $$PWD/bmp/EasyBMP_DataStructures.h \
    $$PWD/bmp/EasyBMP_VariousBMPutilities.h \
    $$PWD/scene/geometry/cube.h \
    $$PWD/scene/geometry/geometry.h \
    $$PWD/scene/geometry/mesh.h \
    $$PWD/scene/geometry/sphere.h \
    $$PWD/openGL/drawable.h \
    $$PWD/openGL/glwidget277.h \
    $$PWD/openGL/shaderprogram.h \
    $$PWD/raytracing/intersection.h \
    $$PWD/raytracing/ray.h \
    $$PWD/scene/transform.h \
    $$PWD/scene/geometry/square.h \
    $$PWD/tinyobj/tiny_obj_loader.h \
    $$PWD/scene/materials/material.h \
    $$PWD/raytracing/film.h \
    $$PWD/scene/xmlreader.h \
    $$PWD/raytracing/integrator.h \
    $$PWD/cameracontrolshelp.h \
    $$PWD/raytracing/samplers/pixelsampler.h \
    $$PWD/raytracing/samplers/stratifiedpixelsampler.h \
    $$PWD/raytracing/samplers/uniformpixelsampler.h \
    $$PWD/scene/geometry/disc.h \
    $$PWD/scene/materials/bxdfs/lambertBxDF.h \
    $$PWD/scene/materials/bxdfs/bxdf.h \
    $$PWD/scene/materials/lightmaterial.h \
    $$PWD/renderthread.h \
    $$PWD/raytracing/intersectionengine.h
