#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

void dfsScene(SceneNode node, glm::mat4 curCTM, std::vector<RenderShapeData> &shapes, std::vector<SceneLightData> &lights) {
    glm::mat4 newCTM = curCTM;
    for (auto& x : node.transformations) {
        switch (x->type) {
        case TransformationType::TRANSFORMATION_MATRIX:
            newCTM = newCTM * x->matrix;
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            newCTM = newCTM * glm::rotate(x->angle, x->rotate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            newCTM = newCTM * glm::scale(x->scale);
            break;
        case TransformationType::TRANSFORMATION_TRANSLATE:
            newCTM = newCTM * glm::translate(x->translate);
        default:
            break;
        }
    }
    for (auto& prim : node.primitives) {
        RenderShapeData x;
        x.primitive = *prim;
        x.ctm = newCTM;
        shapes.push_back(x);
    }
    for (auto &light : node.lights) {
        SceneLightData x;
        x.id = light->id;
        x.color = light->color;
        x.function = light->function;
        x.type = light->type;

        x.pos = newCTM * glm::vec4{0, 0, 0, 1};
        x.dir = newCTM * light->dir;

        x.penumbra = light->penumbra;
        x.angle = light->angle;
        x.width = light->width;
        x.height = light->height;

        lights.push_back(x);

    }
    for (auto& child : node.children) {
        dfsScene(*child, newCTM, shapes, lights);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // Lab 5 code
    SceneGlobalData globalData = fileReader.getGlobalData();
    SceneCameraData cameraData = fileReader.getCameraData();
    renderData.globalData = globalData;
    renderData.cameraData = cameraData;


    renderData.shapes.clear();
    renderData.lights.clear();
    SceneNode *root = fileReader.getRootNode();
    dfsScene(*root, glm::mat4(1), renderData.shapes, renderData.lights);
    return true;
}
