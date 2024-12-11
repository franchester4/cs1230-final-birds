#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>


glm::mat4 transformationMatrix(SceneTransformation &transformation) {

    TransformationType type = transformation.type;

    glm::mat4 transformationMat;

    switch(type) {
    case TransformationType::TRANSFORMATION_TRANSLATE:
        transformationMat = glm::translate(transformation.translate);
        break;
    case TransformationType::TRANSFORMATION_SCALE:
        transformationMat = glm::scale(transformation.scale);
        break;
    case TransformationType::TRANSFORMATION_ROTATE:
        transformationMat = glm::rotate(transformation.angle, transformation.rotate);
        break;
    case TransformationType::TRANSFORMATION_MATRIX:
        transformationMat = transformation.matrix;
        break;
    default:
        break;
    }

    return transformationMat;
}

void buildCTM(SceneNode &node, glm::mat4 parentTransformations, std::vector<RenderShapeData> &shapes, std::vector<SceneLightData> &lights) {

    glm::mat4 ctm = parentTransformations;

    for (int i = 0; i < node.transformations.size(); i++) {
        glm::mat4 transformation = transformationMatrix(*node.transformations[i]);
        ctm = ctm*transformation;
    }

    for (int j = 0; j < node.primitives.size(); j++) {
        RenderShapeData shape;
        shape.primitive = *node.primitives[j];
        shape.ctm = ctm;
        shape.original_ctm;

        shapes.push_back(shape);
    }

    for (int k = 0; k < node.lights.size(); k++) {
        SceneLightData light;

        SceneLight nodeLight = *node.lights[k];
        light.id = nodeLight.id;
        light.type = nodeLight.type;
        light.color = nodeLight.color;
        light.function = nodeLight.function;

        glm::vec4 origin = glm::vec4(0.f,0.f,0.f,1.f);
        glm::vec4 position = ctm*origin;
        light.pos = position;

        light.dir = ctm*nodeLight.dir;

        light.penumbra = nodeLight.penumbra;
        light.angle = nodeLight.angle;
        light.width = nodeLight.width;
        light.height = nodeLight.height;

        lights.push_back(light);
    }

    for (int h = 0; h < node.children.size(); h++) {
        buildCTM(*node.children[h], ctm, shapes, lights);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {

    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // populate renderData with global data, and camera data;
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    // populate renderData's list of primitives and their transforms.
    SceneNode root = *fileReader.getRootNode();
    renderData.shapes.clear();
    renderData.lights.clear();

    glm::mat4 identityMatrix = glm::mat4(1.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,1.f);

    buildCTM(root, identityMatrix, renderData.shapes, renderData.lights);
    return true;

    return true;
}
