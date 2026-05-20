#include "SimulationWidget.h"

#include <QRandomGenerator>
#include <QMatrix4x4>
#include <cmath>

SimulationWidget::SimulationWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);

    createParticles(100);

    connect(&timer, &QTimer::timeout,
            this, &SimulationWidget::updateSimulation);

    timer.start(16);
}

SimulationWidget::~SimulationWidget()
{
    makeCurrent();

    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);

    glDeleteBuffers(1, &particleVBO);
    glDeleteVertexArrays(1, &particleVAO);

    doneCurrent();
}

void SimulationWidget::createParticles(int count)
{
    particles.clear();

    for (int i = 0; i < count; ++i)
    {
        Particle p;

        p.x = QRandomGenerator::global()->generateDouble() * 1.8 - 0.9;
        p.y = QRandomGenerator::global()->generateDouble() * 1.8 - 0.9;
        p.z = QRandomGenerator::global()->generateDouble() * 1.8 - 0.9;

        p.vx = QRandomGenerator::global()->generateDouble() * 0.02 - 0.01;
        p.vy = QRandomGenerator::global()->generateDouble() * 0.02 - 0.01;
        p.vz = QRandomGenerator::global()->generateDouble() * 0.02 - 0.01;

        particles.push_back(p);
    }
}

void SimulationWidget::initializeGL()
{
    initializeOpenGLFunctions();

    Q_ASSERT(QOpenGLContext::currentContext() != nullptr);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    program.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                    R"(#version 330 core
        layout(location = 0) in vec3 position;

        uniform mat4 mvp;

        void main()
        {
            gl_Position = mvp * vec4(position, 1.0);
            gl_PointSize = 8.0;
        })");

    program.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                    R"(#version 330 core
    out vec4 FragColor;

    uniform bool isParticle;

    void main()
    {
        if (isParticle)
        {
            vec2 coord = gl_PointCoord - vec2(0.5);
            float dist = length(coord);

            if (dist > 0.5)
                discard;

            FragColor = vec4(0.0, 1.0, 1.0, 0.8);
        }
        else
        {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    })");

    program.link();

    initCube();
    initParticles();
}

void SimulationWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void SimulationWidget::paintGL()
{
    glClearColor(0.05f, 0.05f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 projection;
    projection.perspective(45.0f,
                           float(width()) / float(height()),
                           0.1f,
                           100.0f);

    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, -5.0f);

    QMatrix4x4 model;
    model.rotate(25.0f, 1, 0, 0);
    model.rotate(35.0f, 0, 1, 0);

    QMatrix4x4 mvp = projection * view * model;

    program.bind();
    program.setUniformValue("mvp", mvp);

    program.setUniformValue("isParticle", false);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_LINES, 0, 24);

    program.setUniformValue("isParticle", true);
    glBindVertexArray(particleVAO);
    glDrawArrays(GL_POINTS, 0, particles.size());

    program.release();
}

void SimulationWidget::initCube()
{
    float vertices[] =
        {
            -1,-1,-1, 1,-1,-1,
            1,-1,-1, 1,1,-1,
            1,1,-1, -1,1,-1,
            -1,1,-1, -1,-1,-1,

            -1,-1,1, 1,-1,1,
            1,-1,1, 1,1,1,
            1,1,1, -1,1,1,
            -1,1,1, -1,-1,1,

            -1,-1,-1, -1,-1,1,
            1,-1,-1, 1,-1,1,
            1,1,-1, 1,1,1,
            -1,1,-1, -1,1,1
        };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertices),
                 vertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(float), nullptr);

    glEnableVertexAttribArray(0);
}

void SimulationWidget::initParticles()
{
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);

    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);

    glBufferData(GL_ARRAY_BUFFER,
                 particles.size() * 3 * sizeof(float),
                 nullptr,
                 GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(float), nullptr);

    glEnableVertexAttribArray(0);

    updateParticleBuffer();
}

void SimulationWidget::updateParticleBuffer()
{
    std::vector<float> data;

    for (auto& p : particles)
    {
        data.push_back(p.x);
        data.push_back(p.y);
        data.push_back(p.z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 data.size() * sizeof(float),
                 data.data(),
                 GL_DYNAMIC_DRAW);
}

void SimulationWidget::resolveCollision(Particle& a, Particle& b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float dz = b.z - a.z;

    float dist2 = dx*dx + dy*dy + dz*dz;
    float minDist = a.radius + b.radius;

    if (dist2 >= minDist * minDist)
        return;

    float dist = std::sqrt(dist2);
    if (dist == 0) return;

    float nx = dx / dist;
    float ny = dy / dist;
    float nz = dz / dist;

    std::swap(a.vx, b.vx);
    std::swap(a.vy, b.vy);
    std::swap(a.vz, b.vz);

    float overlap = 0.5f * (minDist - dist);

    a.x -= overlap * nx;
    a.y -= overlap * ny;
    a.z -= overlap * nz;

    b.x += overlap * nx;
    b.y += overlap * ny;
    b.z += overlap * nz;
}

void SimulationWidget::updateSimulation()
{
    for (auto& p : particles)
    {
        p.x += p.vx;
        p.y += p.vy;
        p.z += p.vz;

        if (p.x < -1 || p.x > 1) p.vx *= -1;
        if (p.y < -1 || p.y > 1) p.vy *= -1;
        if (p.z < -1 || p.z > 1) p.vz *= -1;
    }

    for (size_t i = 0; i < particles.size(); ++i)
    {
        for (size_t j = i + 1; j < particles.size(); ++j)
        {
            resolveCollision(particles[i], particles[j]);
        }
    }

    updateParticleBuffer();
    update();
}

void SimulationWidget::setTemperature(float value)
{
    float scale = std::sqrt(value / currentTemperature);

    for (auto& p : particles)
    {
        p.vx *= scale;
        p.vy *= scale;
        p.vz *= scale;
    }

    currentTemperature = value;
}

void SimulationWidget::setParticleCount(int count)
{
    createParticles(count);
    updateParticleBuffer();
}