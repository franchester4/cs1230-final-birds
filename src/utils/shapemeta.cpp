#include "shapemeta.h"
ShapeMeta::ShapeMeta(RenderShapeData shape_) {
    shape = shape_;
    ctm = shape.ctm;
    inv_ctm = inverse(ctm);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            tinv_ctm[i][j] = ctm[i][j];
        }
    }
    tinv_ctm = inverse(transpose(tinv_ctm));
}

std::vector<float> ShapeMeta::getVBOData() {
    switch (shape.primitive.type) {
    case PrimitiveType::PRIMITIVE_CUBE:
        cube.updateParams(p1);
        return cube.generateShape();
        // break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        cyl.updateParams(p1, p2);
        return cyl.generateShape();
        break;
    case PrimitiveType::PRIMITIVE_SPHERE:
        sphere.updateParams(p1, p2);
        return sphere.generateShape();
        break;
    case PrimitiveType::PRIMITIVE_CONE:
        cone.updateParams(p1, p2);
        return cone.generateShape();
        break;
    case PrimitiveType::PRIMITIVE_MESH:
        return {};
    }
}

void ShapeMeta::setVBO_VAO() {
    std::vector<GLfloat> v_data = getVBOData();

    num_v = v_data.size()/6.f;
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * v_data.size(), v_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);


    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

