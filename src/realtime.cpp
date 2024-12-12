#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "settings.h"
#include "utils/shaderloader.h"

#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

// ================== Project 5: Lights, Camera

float currNear;
float currFar;
int currParam1;
int currParam2;

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
    m_shader = 0;
    m_texture_shader = 0;
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    // clean up VBO and VAO memory
    setup.deleteBuffers();
    setup.deleteVertexArrays();

    glDeleteProgram(m_shader);
    glDeleteProgram(m_texture_shader);

    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteBuffers(1, &m_fullscreen_vbo);

    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    this->doneCurrent();
}

void Realtime::initializeGL() {

    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    bezier = Bezier();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Set clear color to black
    glClearColor(0,0,0,1);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // set FBO width and height
    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = size().width() * m_devicePixelRatio;
    m_fbo_height = size().height() * m_devicePixelRatio;

    aspectRatio = float(size().width())/size().height();

    // load shaders
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    //m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/filter.vert", ":/resources/shaders/filter.frag");

    setup.setupShapes(settings.shapeParameter1, settings.shapeParameter2);

    // keep track of current settings
    currNear = settings.nearPlane;
    currFar = settings.farPlane;
    currParam1 = settings.shapeParameter1;
    currParam2 = settings.shapeParameter2;

    // set texture uniform
    glUseProgram(m_texture_shader);
    glUniform1i(glGetUniformLocation(m_texture_shader, "textureUniform"), 0);

    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS    //
            -1.f,  1.f, 0.0f,
            0.f, 1.f,
            -1.f, -1.f, 0.0f,
            0.f, 0.f,
            1.f, -1.f, 0.0f,
            1.f, 0.f,
            1.f,  1.f, 0.0f,
            1.f, 1.f,
            -1.f,  1.f, 0.0f,
            0.f, 1.f,
            1.f, -1.f, 0.0f,
            1.f, 0.f

        };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(0 * sizeof(GLfloat)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    makeFBO();
    makeTerrainData();

    // initialize sharpen kernel and pass as uniform to texture shader
    float sum = 0.f;
    sharpenKernel = {-1.f/9.f, -1.f/9.f, -1.f/9.f, -1.f/9.f, 17.f/9.f, -1.f/9.f, -1.f/9.f, -1.f/9.f, -1.f/9.f};
    GLint kernelLoc = glGetUniformLocation(m_texture_shader, "kernel");
    glUniform1fv(kernelLoc, 9, &sharpenKernel[0]);

    sobelKernelX = {-1.f, 0.f, 1.f, -2.f, 0.f, 2.f, -1.f, 0.f, 1.f};
    GLint sobelXLoc = glGetUniformLocation(m_texture_shader, "sobelXKernel");
    glUniform1fv(sobelXLoc, 9, &sobelKernelX[0]);

    sobelKernelY = {-1.f, -2.f, -1.f, 0.f, 0.f, 0.f, 1.f, 2.f, 1.f};
    GLint sobelYLoc = glGetUniformLocation(m_texture_shader, "sobelYKernel");
    glUniform1fv(sobelYLoc, 9, &sobelKernelY[0]);

    sceneChanged(true);
}

void Realtime::paintGL() {

    // bind fbo
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // set viewport to size of screen
    glViewport(0, 0, m_screen_width, m_screen_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    paintGeometry();

    // bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

    // clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    paintTexture(m_fbo_texture, settings.perPixelFilter, settings.kernelBasedFilter, settings.extraCredit1, settings.extraCredit2, settings.extraCredit3);
}

/**
 * @brief sets up FBO and its color and depth attachments
 */
void Realtime::makeFBO() {

    // generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);

    // bind to slot 0
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

void Realtime::makeTerrainData() {
    //Load Sphere Data VBO/VAO
    glGenBuffers(1, &m_terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_terrainVBO);
    // Generate sphere data
    m_terrainData = m_terrain.generateTerrain();
    // Send data to VBO
    glBufferData(GL_ARRAY_BUFFER, m_terrainData.size() * sizeof(GLfloat), m_terrainData.data(), GL_STATIC_DRAW);
    // Generate, and bind vao
    glGenVertexArrays(1, &m_terrainVAO);
    glBindVertexArray(m_terrainVAO);

    // Enable and define attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    // verifyVAO(m_terrainData, 0,3,6 * sizeof(GLfloat),reinterpret_cast<void *>(0));
    // verifyVAO(m_terrainData, 1,3,6 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // Clean-up bindings
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}

/**
 * @brief paints from texture that is passed in onto screen
 * @param texture
 * @param isPixelFilter
 * @param isKernelFilter
 */
void Realtime::paintTexture(GLuint texture, bool isPixelFilter, bool isKernelFilter, bool isExtraCredit1, bool isExtraCredit2, bool isExtraCredit3) {

    glUseProgram(m_texture_shader);

    glUniform1i(glGetUniformLocation(m_texture_shader, "isPixelFilter"), isPixelFilter);
    glUniform1i(glGetUniformLocation(m_texture_shader, "isKernelFilter"), isKernelFilter);
    glUniform1i(glGetUniformLocation(m_texture_shader, "isBlurFilter"), isExtraCredit1);
    glUniform1i(glGetUniformLocation(m_texture_shader, "isGrayscaleFilter"), isExtraCredit2);
    glUniform1i(glGetUniformLocation(m_texture_shader, "isSobelFilter"), isExtraCredit3);

    // if kernel filter selected, pass in fbo width and height
    if (isKernelFilter || isExtraCredit1) {
        glUniform1f(glGetUniformLocation(m_texture_shader, "fboWidth"), float(m_fbo_width));
        glUniform1f(glGetUniformLocation(m_texture_shader, "fboHeight"), float(m_fbo_height));
    }

    glBindVertexArray(m_fullscreen_vao);

    // bind "texture" to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

/**
 * @brief for rendering objects in the scene
 */
void Realtime::paintGeometry() {

    // Clear screen color and depth before painting
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // activate the shader program by calling glUseProgram with `m_shader`
    glUseProgram(m_shader);

    // pass in uniform for number of lights in scene
    glUniform1f(glGetUniformLocation(m_shader, "numLights"), renderData.lights.size());

    // pass in m_view and m_proj into vertex shader as unniform
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_view"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_proj"), 1, GL_FALSE, &m_proj[0][0]);

    // pass ambient coefficient into the fragment shader as a uniform
    glUniform1f(glGetUniformLocation(m_shader, "k_a"), renderData.globalData.ka);

    // pass diffuse coefficient into the fragment shader as a uniform
    glUniform1f(glGetUniformLocation(m_shader, "k_d"), renderData.globalData.kd);

    // pass specular coefficient into the fragment shader as a uniform
    glUniform1f(glGetUniformLocation(m_shader, "k_s"), renderData.globalData.ks);

    // pass camera position in world space into the fragment shader as a uniform
    glUniform4fv(glGetUniformLocation(m_shader, "cameraPos"), 1, &cameraWorldPos[0]);

    // loop through lights and pass in needed uniforms
    for (int j = 0; j < renderData.lights.size(); j++) {
        SceneLightData lightData = renderData.lights[j];

        // pass in light type as integer
        std::string lightTypeName = "lightTypes[" + std::to_string(j) + "]";
        GLint lightTypeLoc = glGetUniformLocation(m_shader, lightTypeName.data());
        glUniform1i(lightTypeLoc, static_cast<std::underlying_type_t<LightType>>(lightData.type));

        if (renderData.lights[j].type == LightType::LIGHT_DIRECTIONAL || renderData.lights[j].type == LightType::LIGHT_SPOT) {

            // pass in array of light directions
            std::string name = "lightDirections[" + std::to_string(j) + "]";
            GLint loc = glGetUniformLocation(m_shader, name.data());
            glUniform3f(loc, lightData.dir.x, lightData.dir.y, lightData.dir.z);
        }

        if (renderData.lights[j].type == LightType::LIGHT_POINT || renderData.lights[j].type == LightType::LIGHT_SPOT) {

            // pass in light position
            std::string name = "lightPositions[" + std::to_string(j) + "]";
            GLint loc = glGetUniformLocation(m_shader, name.data());
            glUniform3f(loc, lightData.pos.x, lightData.pos.y, lightData.pos.z);

            // pass in light function for attenuation
            std::string lightFuncName = "lightFunctions[" + std::to_string(j) + "]";
            GLint lightFuncLoc = glGetUniformLocation(m_shader, lightFuncName.data());
            glUniform3f(lightFuncLoc, lightData.function.x, lightData.function.y, lightData.function.z);
        }

        if (renderData.lights[j].type == LightType::LIGHT_SPOT) {
            // pass in angle
            std::string lightAngleName = "lightAngles[" + std::to_string(j) + "]";
            GLint lightAngleLoc = glGetUniformLocation(m_shader, lightAngleName.data());
            glUniform1f(lightAngleLoc, lightData.angle);

            // pass in penumbra
            std::string lightPenumbraName = "lightPenumbras[" + std::to_string(j) + "]";
            GLint lightPenumbraLoc = glGetUniformLocation(m_shader, lightPenumbraName.data());
            glUniform1f(lightPenumbraLoc, lightData.penumbra);
        }

        // pass in array of light colors
        std::string name = "lightColors[" + std::to_string(j) + "]";
        GLint loc = glGetUniformLocation(m_shader, name.data());
        glUniform3f(loc, lightData.color.x, lightData.color.y, lightData.color.z);
    }

    // loop through shapes and pass in needed uniforms
    for (int i = 0; i < renderData.shapes.size(); i++) {
        RenderShapeData shapeData = renderData.shapes[i];

        // bind vao depending on shape type
        setup.bindVAO(shapeData.primitive.type);

        // pass in model matrix to vertex shader as uniform
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_model"), 1, GL_FALSE, &shapeData.ctm[0][0]);

        // pass in inverse transpose of model matrix as uniform
        glm::mat3 inverseTransposeMat = inverse(transpose(shapeData.ctm));
        glUniformMatrix3fv(glGetUniformLocation(m_shader, "invTransMat"), 1, GL_FALSE, &inverseTransposeMat[0][0]);

        // pass in mvp matrix as uniform
        glm::mat4 mvpMat = m_proj*m_view*shapeData.ctm;
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &mvpMat[0][0]);

        // pass in material coefficients and shininess to fragment shader as uniforms
        glUniform3fv(glGetUniformLocation(m_shader, "material_a"), 1, &shapeData.primitive.material.cAmbient[0]);
        glUniform3fv(glGetUniformLocation(m_shader, "material_d"), 1, &shapeData.primitive.material.cDiffuse[0]);
        glUniform3fv(glGetUniformLocation(m_shader, "material_s"), 1, &shapeData.primitive.material.cSpecular[0]);
        glUniform1f(glGetUniformLocation(m_shader, "shininess"), shapeData.primitive.material.shininess);

        // draw depending on shape type
        setup.drawArrays(shapeData.primitive.type);

        // unbind vertex array
        glBindVertexArray(0);
    }

    //Rendering terrain:
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_model"), 1, GL_FALSE, &m_terrainMetaData.ctm[0][0]);

    glUniformMatrix3fv(glGetUniformLocation(m_shader, "invTransMat"), 1, GL_FALSE, &m_terrainMetaData.invCtm[0][0]);

    // pass in mvp matrix as uniform
    glm::mat4 mvpMat = m_proj*m_view*m_terrainMetaData.ctm;
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &mvpMat[0][0]);

    glUniform3fv(glGetUniformLocation(m_shader, "material_a"), 1, &m_terrainMetaData.ambient[0]);

    glUniform3fv(glGetUniformLocation(m_shader, "material_d"), 1, &m_terrainMetaData.diffuse[0]);

    glUniform3fv(glGetUniformLocation(m_shader, "material_s"), 1, &m_terrainMetaData.specular[0]);

    glUniform1fv(glGetUniformLocation(m_shader, "shininess"), 1, &m_terrainMetaData.shininess);

    // Bind Shape Vertex Data
    glBindVertexArray(m_terrainVAO);

    glDrawArrays(GL_TRIANGLES, 0, m_terrainData.size()/6);

    // Task 18: Unbind your VAO here
    glBindVertexArray(0);



    // deactivate the shader program by passing 0 into glUseProgram
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    // regenerate your FBOs
    makeFBO();
}

/**
 * @brief updates camera settings when camera position or look vector changes
 */
void Realtime::updateCameraSettings() {
    camera.setViewMatrix();
    camera.setWorldPos();
    camera.setInverseViewMatrix();

    m_proj = camera.setProjectionMatrix(aspectRatio, 100, settings.nearPlane);
    m_view = camera.getViewMatrix();
    cameraWorldPos = camera.getWorldPos();
}

void Realtime::sceneChanged(bool first_parse) {
    renderData = *new RenderData;

    SceneParser::parse("/resources/birthday_bird.json", renderData);
    camera = Camera(renderData.cameraData);
    updateCameraSettings();
    bezier.init(glm::vec4(camera.cam_start_position, 0.f), camera.getLook());
    setup.setupShapes(settings.shapeParameter1, settings.shapeParameter2);

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {

    if (m_shader != 0) {

        // only update camera when near or far plane is changed
        if (settings.nearPlane != currNear) {
            currNear = settings.nearPlane;
            updateCameraSettings();
        }

        if (settings.farPlane != currFar) {
            currFar = settings.farPlane;
            updateCameraSettings();
        }

        // only update shape vbo when parameters are changed
        if (settings.shapeParameter1 != currParam1) {
            currParam1 = settings.shapeParameter1;
            setup.setupShapes(settings.shapeParameter1, settings.shapeParameter2);
        }

        if (settings.shapeParameter2 != currParam2) {
            currParam2 = settings.shapeParameter2;
            setup.setupShapes(settings.shapeParameter1, settings.shapeParameter2);
        }
    }

    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        // int posX = event->position().x();
        // int posY = event->position().y();
        // int deltaX = posX - m_prev_mouse_pos.x;
        // int deltaY = posY - m_prev_mouse_pos.y;
        // m_prev_mouse_pos = glm::vec2(posX, posY);

        // // Use deltaX and deltaY here to rotate
        // camera.rotateX(deltaX*0.005f);

        // // camera.rotateY(deltaY*0.001f);
        // // cam_y_rotation += deltaY*0.001f;

        // updateCameraSettings();
        // updateCTMs();

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // BEZIER PARAMETERS
    float t = deltaTime * 0.8; // speed (smaller = slower)
    float denom = 12.f; // controls rotation speed (smaller = faster)
    float mx = M_PI / 5.f; // max angle

    // move the camera along cur bezier
    auto cur_mat = bezier.bezierCoeffs();

    auto new_pos = glm::transpose(cur_mat) * glm::vec4(pow(t, 3), pow(t, 2), t, 1);

    auto new_grad = glm::transpose(cur_mat) * glm::vec4(3 * pow(t, 2), 2 * t, 1, 0);

    new_grad = glm::normalize(new_grad);

    camera.setPosAndLook(new_pos, new_grad);
    updateCameraSettings();
    updateCTMs();

    bezier.pts[0] = new_pos;
    bezier.dir = new_grad;

    float theta1 = 0.f;
    float theta2 = 0.f;

    // Use deltaTime and m_keyMap here to move around
    if (m_keyMap[Qt::Key_S]) { // translate look
        theta2 +=  fmin(fmax(deltaTime * M_PI / denom, 0.f), mx);
    }
    if (m_keyMap[Qt::Key_W]) { // translate look backwards
        theta2 += fmin(fmax(-deltaTime * M_PI / denom, -mx), 0.f);
    }
    if (m_keyMap[Qt::Key_D]) {
        theta1 += fmin(fmax(deltaTime * 2 * M_PI / denom, 0.f), mx);

    }
    if (m_keyMap[Qt::Key_A]) {
        theta1 += fmin(fmax(-deltaTime * 2 * M_PI / denom, -mx), 0.f);
    }
    if (m_keyMap[Qt::Key_Space]) {
        camera.moveUp(deltaTime);
        updateCameraSettings();
        updateCTMs();
    }

    if (m_keyMap[Qt::Key_Control]) {
        camera.moveDown(deltaTime);
        updateCameraSettings();
        updateCTMs();
    }

    bezier.updatePoints(theta1, theta2);

    update(); // asks for a PaintGL() call to occur
}

void Realtime::updateCTMs() {
    for (RenderShapeData &rsd : renderData.shapes) {
        camera.updateCTMs(rsd);
    }
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
