#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "camera.h"
#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include "utils/sceneparser.h"
#include "utils/shapemeta.h"



class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

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

    void makeFBO();
    void paintTexture(GLuint texture);
    void paintScene();
    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;

    // Current Settings
    std::string cur_scene;

    int cur_p1;
    int cur_p2;

    float cur_near;
    float cur_far;

    float cur_pixel;
    float cur_kernel;
    bool cur_outline;

    // glsl specific
    GLuint m_shader;

    // filter fbo stuff
    GLuint m_filter_shader;
    GLuint m_defaultFBO;

    int m_fbo_width;
    int m_fbo_height;
    int m_screen_width;
    int m_screen_height;


    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;

    // Lights
    int lightType[8];
    glm::vec4 lightColor[8];
    glm::vec4 lightPos[8];
    glm::vec4 lightDir[8];

    // RADIANS
    float lightPenumbra[8];
    float lightAngle[8];

    glm::vec3 attn[8];

    int numLights;

    // Shapes
    std::vector<ShapeMeta>shape_metadata;

    // Rendered Data
    RenderData rendered;
    void setCurrentSettings();
    void updateTessellations();
    void processRendered();

    std::vector<GLuint>vboVec;
    std::vector<GLuint>vaoVec;
    void clearBuffers();
    // Camera
    Camera cam;
};
