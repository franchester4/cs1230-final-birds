#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "camera.h"
#include "setup.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include "terraingenerator.h"
#include "bezier.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged(bool first_parse);
    void settingsChanged();
    void saveViewportImage(std::string filePath);

    void makeTerrainData();
public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    GLuint m_shader;     // Stores id of shader program

    glm::vec3 cameraWorldPos;

    void updateCameraSettings();

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;

    Setup setup;

    glm::vec4 m_lightPos; // The world-space position of the point light

    RenderData renderData;

    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view  = glm::mat4(1);
    glm::mat4 m_proj  = glm::mat4(1);

    Camera camera;

    Bezier bezier;

    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;

    GLuint m_texture_shader;

    GLuint m_defaultFBO;
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    int m_fbo_width;
    int m_fbo_height;
    int m_screen_width;
    int m_screen_height;

    void makeFBO();

    void paintGeometry();
    void paintTexture(GLuint texture, bool isPixelFilter, bool isKernelFilter, bool isExtraCredit1, bool isExtraCredit2, bool isExtraCredit3);

    std::vector<float> sharpenKernel;
    std::vector<float> sobelKernelX;
    std::vector<float> sobelKernelY;

    float aspectRatio;

    TerrainGenerator m_terrain = TerrainGenerator();
    GLuint m_terrainVBO;
    GLuint m_terrainVAO;
    GLuint m_terrain_shader;
    std::vector<float> m_terrainData;

    //CHANGE TERRAIN PARAMETERS HERE
    struct tmd {
        glm::mat4 ctm = glm::mat4(200,0,0,0,
                                  0,200,0,0,
                                  0,0,200,0,
                                  -100,-60,-100,1);
        glm::mat4 invCtm = glm::inverse(glm::transpose(ctm));
        glm::vec3 ambient = glm::vec3(0.3,0.2,0.2);
        glm::vec3 diffuse = glm::vec3(1.0,0.3,0.6);
        glm::vec3 specular = glm::vec3(0.5,0.5,0.5);
        float shininess = 3;
    };
    tmd m_terrainMetaData;
    void updateCTMs(glm::mat4& transition);
};

