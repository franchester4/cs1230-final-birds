#ifndef SETUP_H
#define SETUP_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "shapes/cone.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include "utils/scenedata.h"

class Setup
{
public:
    Setup();

    void setupSphere(int param1, int param2);
    void setupCube(int param1, int param2);
    void setupCylinder(int param1, int param2);
    void setupCone(int param1, int param2);
    void setupShapes(int param1, int param2);

    void bindVAO(PrimitiveType type);
    void drawArrays(PrimitiveType type);

    void deleteBuffers();
    void deleteVertexArrays();

    GLuint m_sphere_vbo; // Stores id of sphere vbo
    GLuint m_sphere_vao; // Stores id of sphere vao
    std::vector<float> m_sphereData;

    GLuint m_cube_vbo; // Stores id of cube vbo
    GLuint m_cube_vao; // Stores id of cube vao
    std::vector<float> m_cubeData;

    GLuint m_cone_vbo; // Stores id of cone vbo
    GLuint m_cone_vao; // Stores id of cone vao
    std::vector<float> m_coneData;

    GLuint m_cylinder_vbo; // Stores id of cylinder vbo
    GLuint m_cylinder_vao; // Stores id of cylinder vao
    std::vector<float> m_cylinderData;

    Sphere *sphere;
    Cube *cube;
    Cone *cone;
    Cylinder *cylinder;

    GLuint getSphereVAO();
};

#endif // SETUP_H
