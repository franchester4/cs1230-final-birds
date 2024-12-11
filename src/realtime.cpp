#include "realtime.h"
#include "utils/shaderloader.h"


#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
// ================== Project 5: Lights, Camera

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
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    clearBuffers();
    glDeleteProgram(m_filter_shader);
    glDeleteProgram(m_shader);
    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteBuffers(1, &m_fullscreen_vbo);

    // Task 35: Delete OpenGL memory here
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);
    this->doneCurrent();
}

void Realtime::clearBuffers() {
    // assume makeCurrent called, doneCurrent after
    glDeleteBuffers(vboVec.size(), vboVec.data());
    glDeleteVertexArrays(vaoVec.size(), vaoVec.data());
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();
    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

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

    // Students: anything requiring OpenGL calls when the program starts should be done here
    glClearColor(0, 0, 0, 1);

    // load shader
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_filter_shader = ShaderLoader::createShaderProgram(":/resources/shaders/filter.vert", ":/resources/shaders/filter.frag");

    setCurrentSettings();
    // want to parse the scene
    bool success = SceneParser::parse(cur_scene, rendered);
    if (!success) {
        std::cerr << "Error loading scene: " << settings.sceneFilePath << std::endl;
    }
    processRendered();

    // should be done but just in case
    // Clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glUseProgram(m_filter_shader);
    auto sampler_loc = glGetUniformLocation(m_filter_shader, "texture_sampler");
    glUniform1i(sampler_loc, 0);

    glUseProgram(0);

    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS    //
            -1.0f,  1.0f, 0.0f,
            0.f, 1.f, 0.f,
            -1.0f, -1.0f, 0.0f,
            0.f, 0.f, 0.f,
            1.0f, -1.0f, 0.0f,
            1.f, 0.f, 0.f,
            1.0f,  1.0f, 0.0f,
            1.f, 1.f, 0.f,
            -1.0f,  1.0f, 0.0f,
            0.f, 1.f, 0.f,
            1.0f, -1.0f, 0.0f,
            1.f, 0.f, 0.f
        };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    makeFBO();
}

void Realtime::makeFBO(){
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void Realtime::processRendered() {
    shape_metadata.clear();
    for (auto& shape : rendered.shapes) {
        ShapeMeta curShape = ShapeMeta(shape);
        curShape.p1 = cur_p1;
        curShape.p2 = cur_p2;
        shape_metadata.push_back(curShape);
    }
    numLights = rendered.lights.size();
    for (int i = 0; i < numLights; i++) {
        lightType[i] = static_cast<int>(rendered.lights[i].type);
        if (rendered.lights[i].type != LightType::LIGHT_DIRECTIONAL) {
            lightPos[i] = rendered.lights[i].pos;
        }
        else lightPos[i] = glm::vec4(0.0);

        if (rendered.lights[i].type != LightType::LIGHT_POINT) {
            lightDir[i] = rendered.lights[i].dir;
        }
        else lightDir[i] = glm::vec4(0.0);

        lightColor[i] = rendered.lights[i].color;

        // penumbra + angle for spot light RADIANS
        if (rendered.lights[i].type == LightType::LIGHT_SPOT) {
            lightPenumbra[i] = rendered.lights[i].penumbra;
            lightAngle[i] = rendered.lights[i].angle;
        }
        else {
            lightPenumbra[i] = 0;
            lightAngle[i] = 0;
        }

        attn[i] = rendered.lights[i].function;
    }
    cam.setCamera(rendered.cameraData, cur_near, cur_far, size().width(), size().height());
    bezier.init(cam.pos, cam.look);
    // bezier.dir = glm::normalize(cam.look);

    // generate buffers
    int num = shape_metadata.size();
    vboVec.resize(num);
    vaoVec.resize(num);

    glGenBuffers(num, vboVec.data());
    glGenVertexArrays(num, vaoVec.data());

    for (int i = 0; i < num; i++) {
        // glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // pass in stuff
        shape_metadata[i].vbo = vboVec[i];
    }
    for (int i = 0; i < num; i++) {
        shape_metadata[i].vao = vaoVec[i];
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Realtime::paintGL() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_fbo_width, m_fbo_height);

    // Students: anything requiring OpenGL calls every frame should be done here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    paintScene();

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_screen_width, m_screen_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintTexture(m_fbo_texture);

}

void Realtime::paintScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto& shape : shape_metadata) {
        glUseProgram(m_shader);
        glBindVertexArray(shape.vao);

        // set VERT uniforms
        auto ctm_loc = glGetUniformLocation(m_shader, "ctm");
        glUniformMatrix4fv(ctm_loc, 1, GL_FALSE, &shape.ctm[0][0]);

        auto ctm_tinv = glGetUniformLocation(m_shader, "ctm_tinv");
        glUniformMatrix3fv(ctm_tinv, 1, GL_FALSE, &shape.tinv_ctm[0][0]);

        auto pv_loc = glGetUniformLocation(m_shader, "pv");
        glUniformMatrix4fv(pv_loc, 1, GL_FALSE, &cam.pv[0][0]);

        // set FRAG uniforms
        // lights
        for (int i = 0; i < 8; i++) {
            std::string type_str = "lightType[" + std::to_string(i) + "]";
            auto type_loc = glGetUniformLocation(m_shader, type_str.data());
            glUniform1i(type_loc, lightType[i]);

            std::string col_str = "lightColor[" + std::to_string(i) + "]";
            auto col_loc = glGetUniformLocation(m_shader, col_str.data());
            glUniform4fv(col_loc, 1, &lightColor[i][0]);

            std::string pos_str = "lightPos[" + std::to_string(i) + "]";
            auto pos_loc = glGetUniformLocation(m_shader, pos_str.data());
            glUniform4fv(pos_loc, 1, &lightPos[i][0]);

            std::string dir_str = "lightDir[" + std::to_string(i) + "]";
            auto dir_loc = glGetUniformLocation(m_shader, dir_str.data());
            glUniform4fv(dir_loc, 1, &lightDir[i][0]);

            // spot light

            std::string penumbra_str = "lightPenumbra[" + std::to_string(i) + "]";
            auto penum_loc = glGetUniformLocation(m_shader, penumbra_str.data());
            glUniform1f(penum_loc, lightPenumbra[i]);

            std::string angle_str = "lightAngle[" + std::to_string(i) + "]";
            auto angle_loc = glGetUniformLocation(m_shader, angle_str.data());
            glUniform1f(angle_loc, lightAngle[i]);

            std::string attn_str = "attn[" + std::to_string(i) + "]";
            auto attn_loc = glGetUniformLocation(m_shader, attn_str.data());
            glUniform3fv(attn_loc, 1, &attn[i][0]);

        }

        auto num_light_loc = glGetUniformLocation(m_shader, "numLights");
        glUniform1i(num_light_loc, numLights);

        // global material
        auto ka_loc = glGetUniformLocation(m_shader, "ka");
        glUniform1f(ka_loc, rendered.globalData.ka);
        auto kd_loc = glGetUniformLocation(m_shader, "kd");
        glUniform1f(kd_loc, rendered.globalData.kd);
        auto ks_loc = glGetUniformLocation(m_shader, "ks");
        glUniform1f(ks_loc, rendered.globalData.ks);

        // shape material
        auto ca_loc = glGetUniformLocation(m_shader, "ca");
        glUniform4fv(ca_loc, 1, &shape.shape.primitive.material.cAmbient[0]);
        auto cd_loc = glGetUniformLocation(m_shader, "cd");
        glUniform4fv(cd_loc, 1, &shape.shape.primitive.material.cDiffuse[0]);
        auto cs_loc = glGetUniformLocation(m_shader, "cs");
        glUniform4fv(cs_loc, 1, &shape.shape.primitive.material.cSpecular[0]);
        auto cn_loc = glGetUniformLocation(m_shader, "cn");
        glUniform1i(cn_loc, shape.shape.primitive.material.shininess);

        // cam
        auto cam_loc = glGetUniformLocation(m_shader, "cam");
        glUniform4fv(cam_loc, 1, &cam.pos[0]);

        glDrawArrays(GL_TRIANGLES, 0, shape.num_v);
        glUseProgram(0);

    }
}


void Realtime::paintTexture(GLuint texture){
    glUseProgram(m_filter_shader);
    auto pixel_loc = glGetUniformLocation(m_filter_shader, "pixel_flag");
    glUniform1i(pixel_loc, cur_pixel);
    auto kernel_loc = glGetUniformLocation(m_filter_shader, "kernel_flag");
    glUniform1i(kernel_loc, cur_kernel);

    auto w_loc = glGetUniformLocation(m_filter_shader, "screen_w");
    glUniform1i(w_loc, m_screen_width);
    auto h_loc = glGetUniformLocation(m_filter_shader, "screen_h");
    glUniform1i(h_loc, m_screen_height);

    glBindVertexArray(m_fullscreen_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    cam.w = w;
    cam.h = h;
    cam.aspect = w * 1.0 / h;
    cam.updateNearFar(cur_near, cur_far);

    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    makeFBO();

    update();
}

void Realtime::setCurrentSettings() {
    cur_scene = settings.sceneFilePath;
    cur_p1 = settings.shapeParameter1;
    cur_p2 = settings.shapeParameter2;
    cur_near = settings.nearPlane;
    cur_far = settings.farPlane;
    cur_pixel = settings.perPixelFilter;
    cur_kernel = settings.kernelBasedFilter;
}

void Realtime::updateTessellations() {
    for (auto& shape : shape_metadata) {
        shape.p1 = cur_p1;
        shape.p2 = cur_p2;
        shape.setVBO_VAO();
    }
}

void Realtime::sceneChanged() {
    this->makeCurrent();
    setCurrentSettings();
    clearBuffers();
    bool success = SceneParser::parse(settings.sceneFilePath, rendered);
    if (!success) {
        std::cerr << "Error loading scene: " << settings.sceneFilePath << std::endl;
    }
    processRendered(); // update our shapes, lights, cameras, VBO/VAO
    updateTessellations(); // set shape tessellations
    setCurrentSettings(); // set settings as current
    this->doneCurrent();
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    if (cur_p1 != settings.shapeParameter1 || cur_p2 != settings.shapeParameter2) {
        setCurrentSettings();
        updateTessellations();
    }
    else if (cur_near != settings.nearPlane || cur_far != settings.farPlane) {
        // update camera
        setCurrentSettings();
        cam.updateNearFar(cur_near, cur_far);
    }
    else if (cur_pixel != settings.perPixelFilter || cur_kernel != settings.kernelBasedFilter) {
        setCurrentSettings();
        // ??? idk if anything needs to happen
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

glm::mat3 rodrigues(glm::vec3 r) {
    glm::mat3 res;
    float theta = glm::length(r);
    res[0][0] = cos(theta) + pow(r[0], 2) * (1 - cos(theta));
    res[0][1] = r[0] * r[1] * (1 - cos(theta)) + r[2] * sin(theta);
    res[0][2] = r[0] * r[2] * (1 - cos(theta)) - r[1] * sin(theta);

    res[1][0] = r[1] * r[0] * (1 - cos(theta)) - r[2] * sin(theta);
    res[1][1] = cos(theta) + pow(r[1], 2) * (1 - cos(theta));
    res[1][2] = r[1] * r[2] * (1 - cos(theta)) + r[0] * sin(theta);

    res[2][0] = r[2] * r[0] * (1 - cos(theta)) + r[1] * sin(theta);
    res[2][1] = r[2] * r[1] * (1 - cos(theta)) - r[0] * sin(theta);
    res[2][2] = cos(theta) + pow(r[2], 2) * (1 - cos(theta));
    return res;
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        // float speed = 0.015f;
        // int posX = event->position().x();
        // int posY = event->position().y();
        // int deltaX = posX - m_prev_mouse_pos.x;
        // int deltaY = posY - m_prev_mouse_pos.y;
        // m_prev_mouse_pos = glm::vec2(posX, posY);

        // // Use deltaX and deltaY here to rotate

        // // deltaX
        // glm::vec3 r = (deltaX * speed) * glm::vec3(0, 1, 0);
        // glm::mat3 rot_mat = rodrigues(r);
        // // cam.pos = glm::vec4(rot_mat * glm::vec3(cam.pos), 1.f);
        // cam.look = glm::vec4(rot_mat * glm::vec3(cam.look), 0.f);
        // // cam.up = glm::vec4(rot_mat * glm::vec3(cam.up), 0.f);
        // cam.setViewMatrix();
        // cam.setProjMatrix();
        // cam.setPV();

        // // deltaY
        // glm::vec3 perp = (deltaY * speed) * glm::normalize(glm::cross(glm::vec3(cam.look), glm::vec3(cam.up)));
        // glm::mat3 rot_mat2 = rodrigues(perp);
        // cam.look = glm::vec4(rot_mat2 * glm::vec3(cam.look), 0.f);
        // // cam.up = glm::vec4(rot_mat2 * glm::vec3(cam.up), 0.f);
        // cam.setViewMatrix();
        // cam.setProjMatrix();
        // cam.setPV();

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;

    float t = deltaTime * 0.2;
    m_elapsedTimer.restart();
    float denom = 10.f;
    float mx = M_PI / 5.f;

    // move the camera along cur bezier
    auto cur_mat = bezier.bezierCoeffs();

    // i have a 4 x 3, take transpose for 3 x 4
    auto new_pos = glm::transpose(cur_mat) * glm::vec4(pow(t, 3), pow(t, 2), t, 1);

    // what is the gradient?
    //
    auto new_grad = glm::transpose(cur_mat) * glm::vec4(3 * pow(t, 2), 2 * t, 1, 0);

    new_grad = glm::normalize(new_grad);

    cam.pos = glm::vec4(new_pos, 1.f);
    cam.look = glm::vec4(new_grad, 0.f);

    cam.setViewMatrix();
    cam.setProjMatrix();
    cam.setPV();


    bezier.pts[0] = new_pos;
    bezier.dir = glm::normalize(cam.look);

    float theta1 = 0.f;
    float theta2 = 0.f;

    // Use deltaTime and m_keyMap here to move around
    if (m_keyMap[Qt::Key_S]) { // translate look
        theta2 +=  fmin(fmax(deltaTime * M_PI / denom, 0.f), mx);
    }
    if (m_keyMap[Qt::Key_W]) { // translate look backwards
        // theta2 = - deltaTime * M_PI / denom;
        theta2 += fmin(fmax(-deltaTime * M_PI / denom, -mx), 0.f);
    }
    if (m_keyMap[Qt::Key_D]) {
        // theta1 = deltaTime * M_PI / denom;
        theta1 += fmin(fmax(deltaTime * M_PI / denom, 0.f), mx);

    }
    if (m_keyMap[Qt::Key_A]) {
        // theta1 = deltaTime * -M_PI / denom;
        theta1 += fmin(fmax(-deltaTime * M_PI / denom, -mx), 0.f);
    }

    bezier.updatePoints(theta1, theta2);

    update(); // asks for a PaintGL() call to occur
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
