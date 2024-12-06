#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "sceneparser.h"


#include "shapes/cone.h"
#include "shapes/cube.h"
#include "shapes/sphere.h"
#include "shapes/cylinder.h"

// Contains all shape data (aka scene primitives)

class ShapeMeta
{
public:
    ShapeMeta(RenderShapeData shape_);
    RenderShapeData shape;
    glm::mat4 ctm;
    glm::mat4 inv_ctm;
    glm::mat3 tinv_ctm;

    int p1;
    int p2;

    int num_v;

    // VBO/VAO; initialized in glInitialize()
    void setVBO_VAO();
    GLuint vbo; // Stores id of vbo
    GLuint vao; // Stores id of vao

    std::vector<float>getVBOData();

    Cone cone;
    Cube cube;
    Cylinder cyl;
    Sphere sphere;

private:

};
