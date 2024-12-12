#include "setup.h"
#include "settings.h"

Setup::Setup()
{
    sphere = new Sphere;
    cube = new Cube;
    cylinder = new Cylinder;
    cone = new Cone;
}

/**
 * @brief set up and bind vbo and vao for sphere
 * @param param1
 * @param param2
 */
void Setup::setupSphere(int param1, int param2) {

    // generate and bind vbo
    glGenBuffers(1, &m_sphere_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);

    // updating vertices based on parameters and generating sphere data
    if (param1 < 2 || param2 < 2) {
        sphere->updateParams(2, 2);
    } else {
        sphere->updateParams(param1, param2);
    }
    m_sphereData = sphere->generateShape();

    // send data to vbo
    glBufferData(GL_ARRAY_BUFFER,m_sphereData.size() * sizeof(GLfloat),m_sphereData.data(), GL_STATIC_DRAW);

    // generate and bind vao
    glGenVertexArrays(1, &m_sphere_vao);
    glBindVertexArray(m_sphere_vao);

    // attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0 * sizeof(GLfloat)));

    // attribute 1 to store normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

/**
 * @brief set up and bind vbo and vao for cube
 * @param param1
 * @param param2
 */
void Setup::setupCube(int param1, int param2) {

    // generate and bind vbo
    glGenBuffers(1, &m_cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);

    // updating vertices based on parameters and generating cube data
    cube->updateParams(param1);
    m_cubeData = cube->generateShape();

    // send data to vbo
    glBufferData(GL_ARRAY_BUFFER,m_cubeData.size() * sizeof(GLfloat),m_cubeData.data(), GL_STATIC_DRAW);

    // generate and bind vao
    glGenVertexArrays(1, &m_cube_vao);
    glBindVertexArray(m_cube_vao);

    // attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0 * sizeof(GLfloat)));

    // attribute 1 to store normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

/**
 * @brief set up and bind vbo and vao for cylinder
 * @param param1
 * @param param2
 */
void Setup::setupCylinder(int param1, int param2) {

    // generate and bind vbo
    glGenBuffers(1, &m_cylinder_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cylinder_vbo);

    // updating vertices based on parameters and generating cylinder data
    if (param2 < 2) {
        cylinder->updateParams(param1, 2);
    } else {
        cylinder->updateParams(param1, param2);
    }
    m_cylinderData = cylinder->generateShape();

    // send data to vbo
    glBufferData(GL_ARRAY_BUFFER,m_cylinderData.size() * sizeof(GLfloat),m_cylinderData.data(), GL_STATIC_DRAW);

    // generate and bind vao
    glGenVertexArrays(1, &m_cylinder_vao);
    glBindVertexArray(m_cylinder_vao);

    // attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0 * sizeof(GLfloat)));

    // attribute 1 to store normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

/**
 * @brief set up and bind vbo and vao for cone
 * @param param1
 * @param param2
 */
void Setup::setupCone(int param1, int param2) {

    // generate and bind vbo
    glGenBuffers(1, &m_cone_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cone_vbo);

    // updating vertices based on parameters and generating cone data
    if (param2 < 2) {
        cone->updateParams(param1, 2);
    } else {
        cone->updateParams(param1, param2);
    }
    m_coneData = cone->generateShape();

    // send data to vbo
    glBufferData(GL_ARRAY_BUFFER,m_coneData.size() * sizeof(GLfloat),m_coneData.data(), GL_STATIC_DRAW);

    // generate and bind vao
    glGenVertexArrays(1, &m_cone_vao);
    glBindVertexArray(m_cone_vao);

    // attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0 * sizeof(GLfloat)));

    // attribute 1 to store normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

/**
 * @brief sets up all vbo and vao for shapes
 * @param param1
 * @param param2
 */
void Setup::setupShapes(int param1, int param2) {
    setupSphere(50,50);
    setupCube(50,50);
    setupCylinder(50,50);
    setupCone(50,50);
}

/**
 * @brief binds vao depending on primitive type
 * @param type
 */
void Setup::bindVAO(PrimitiveType type) {
    switch (type)
    {
    case PrimitiveType::PRIMITIVE_SPHERE:
        glBindVertexArray(m_sphere_vao);
        break;
    case PrimitiveType::PRIMITIVE_CUBE:
        glBindVertexArray(m_cube_vao);
        break;
    case PrimitiveType::PRIMITIVE_CONE:
        glBindVertexArray(m_cone_vao);
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        glBindVertexArray(m_cylinder_vao);
        break;
    default:
        break;
    }
}

/**
 * @brief draws/renders primitive based on type
 * @param type
 */
void Setup::drawArrays(PrimitiveType type) {
    switch (type)
    {
    case PrimitiveType::PRIMITIVE_SPHERE:
        glDrawArrays(GL_TRIANGLES, 0, m_sphereData.size() / 6);
        break;
    case PrimitiveType::PRIMITIVE_CUBE:
        glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 6);
        break;
    case PrimitiveType::PRIMITIVE_CONE:
        glDrawArrays(GL_TRIANGLES, 0, m_coneData.size() / 6);
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        glDrawArrays(GL_TRIANGLES, 0, m_cylinderData.size() / 6);
        break;
    default:
        break;
    }
}

/**
 * @brief clean up VBO memory, called in finish
 */
void Setup::deleteBuffers() {
    glDeleteBuffers(1, &m_sphere_vbo);
    glDeleteBuffers(1, &m_cube_vbo);
    glDeleteBuffers(1, &m_cylinder_vbo);
    glDeleteBuffers(1, &m_cone_vbo);
}

/**
 * @brief clean up VAO memory, called in finish
 */
void Setup::deleteVertexArrays() {
    glDeleteVertexArrays(1, &m_sphere_vbo);
    glDeleteVertexArrays(1, &m_cube_vbo);
    glDeleteVertexArrays(1, &m_cylinder_vbo);
    glDeleteVertexArrays(1, &m_cone_vbo);
}


GLuint Setup::getSphereVAO() {
    return m_sphere_vao;
}
