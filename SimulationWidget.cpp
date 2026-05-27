#include "SimulationWidget.h"

#include <QRandomGenerator>
#include <QMatrix4x4>
#include <cmath>
#include <QMouseEvent>
#include <QWheelEvent>

SimulationWidget::SimulationWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    setFocusPolicy(Qt::StrongFocus);

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

        p.vx = QRandomGenerator::global()->generateDouble() * 2.0 - 1.0;
        p.vy = QRandomGenerator::global()->generateDouble() * 2.0 - 1.0;
        p.vz = QRandomGenerator::global()->generateDouble() * 2.0 - 1.0;

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
            FragColor = vec4(1.0, 0.41, 0.71, 1.0);
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

void SimulationWidget::mousePressEvent(QMouseEvent* event)
{
    lastMousePosition = event->pos();
}

void SimulationWidget::mouseMoveEvent(QMouseEvent* event)
{
    int dx = event->x() - lastMousePosition.x();
    int dy = event->y() - lastMousePosition.y();

    rotationY += dx * 0.5f;
    rotationX += dy * 0.5f;

    lastMousePosition = event->pos();

    update();
}

void SimulationWidget::wheelEvent(QWheelEvent* event)
{
    QPoint numDegrees = event->angleDelta();

    if (!numDegrees.isNull())
    {
        cameraDistance -= numDegrees.y() * 0.01f;

        if (cameraDistance < 2.0f)
            cameraDistance = 2.0f;

        if (cameraDistance > 15.0f)
            cameraDistance = 15.0f;

        update();
    }
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
    view.translate(0.0f, 0.0f, -cameraDistance);

    QMatrix4x4 model;
    model.rotate(rotationX, 1, 0, 0);
    model.rotate(rotationY, 0, 1, 0);

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
    // Вектор между центрами частиц
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float dz = b.z - a.z;

    // Квадрат расстояния
    float dist2 = dx * dx + dy * dy + dz * dz;

    // Минимальная дистанция касания
    float minDist = a.radius + b.radius;

    // Если частицы не касаются
    if (dist2 >= minDist * minDist)
        return;

    float dist = std::sqrt(dist2);

    if (dist < 0.0001f)
        dist = 0.0001f;

    // Нормаль столкновения
    float nx = dx / dist;
    float ny = dy / dist;
    float nz = dz / dist;

    // Относительная скорость
    float rvx = a.vx - b.vx;
    float rvy = a.vy - b.vy;
    float rvz = a.vz - b.vz;

    // Скорость вдоль нормали
    float velAlongNormal =
        rvx * nx +
        rvy * ny +
        rvz * nz;

    // Если частицы уже разлетаются
    if (velAlongNormal > 0)
        return;

    // Коэффициент упругости
    float restitution = 1.0f;

    // Импульс столкновения
    float impulse =
        -(1.0f + restitution) * velAlongNormal;

    impulse /= (1.0f / a.mass + 1.0f / b.mass);

    // Компоненты импульса
    float impulseX = impulse * nx;
    float impulseY = impulse * ny;
    float impulseZ = impulse * nz;

    // Меняем скорости
    a.vx += impulseX / a.mass;
    a.vy += impulseY / a.mass;
    a.vz += impulseZ / a.mass;

    b.vx -= impulseX / b.mass;
    b.vy -= impulseY / b.mass;
    b.vz -= impulseZ / b.mass;

    // Раздвигаем частицы,
    // чтобы они не залипали друг в друге
    float overlap = minDist - dist;

    float correction = overlap * 0.5f;

    a.x -= correction * nx;
    a.y -= correction * ny;
    a.z -= correction * nz;

    b.x += correction * nx;
    b.y += correction * ny;
    b.z += correction * nz;
}

void SimulationWidget::resolveWallCollision(
    float& pos,
    float& vel,
    float radius)
{
    const float min = -1.0f + radius;
    const float max =  1.0f - radius;

    if (pos < min)
    {
        pos = min;

        float impulse =
            2.0f * std::abs(vel);

        pressureAccumulator += impulse;

        vel = std::abs(vel);
    }

    if (pos > max)
    {
        pos = max;

        float impulse =
            2.0f * std::abs(vel);

        pressureAccumulator += impulse;

        vel = -std::abs(vel);
    }
}

void SimulationWidget::updateSimulation()
{
    for (auto& p : particles)
    {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
    }

    for (size_t i = 0; i < particles.size(); ++i)
    {
        for (size_t j = i + 1; j < particles.size(); ++j)
        {
            resolveCollision(particles[i], particles[j]);
        }
    }

    for (auto& p : particles)
    {
        resolveWallCollision(p.x, p.vx, p.radius);
        resolveWallCollision(p.y, p.vy, p.radius);
        resolveWallCollision(p.z, p.vz, p.radius);
    }

    currentPressure = pressureAccumulator / (6.0f * wallArea * dt);

    pressureAccumulator = 0.0f;

    float targetTemperature = 1.0f;

    float currentEnergy = 0.0f;

    for (const auto& p : particles)
    {
        currentEnergy +=
            p.vx * p.vx +
            p.vy * p.vy +
            p.vz * p.vz;
    }

    float currentTemperature =
        currentEnergy / particles.size();

    float scale =
        std::sqrt(
            targetTemperature
            / currentTemperature
            );

    for (auto& p : particles)
    {
        p.vx *= scale;
        p.vy *= scale;
        p.vz *= scale;
    }

    updateParticleBuffer();
    update();

}

float SimulationWidget::getPressure() const
{
    return currentPressure;
}

float SimulationWidget::getTemperature() const
{
    float energy = 0.0f;

    for (const auto& p : particles)
    {
        energy +=
            p.vx * p.vx +
            p.vy * p.vy +
            p.vz * p.vz;
    }

    return energy / particles.size();
}
std::vector<float> SimulationWidget::getSpeeds() const
{
    std::vector<float> speeds;

    for (const auto& p : particles)
    {
        float v = std::sqrt(
            p.vx * p.vx +
            p.vy * p.vy +
            p.vz * p.vz
            );

        speeds.push_back(v);
    }

    return speeds;
}

std::vector<float> SimulationWidget::getVX() const
{
    std::vector<float> values;

    for (const auto& p : particles)
        values.push_back(p.vx);

    return values;
}

std::vector<float> SimulationWidget::getVY() const
{
    std::vector<float> values;

    for (const auto& p : particles)
        values.push_back(p.vy);

    return values;
}

std::vector<float> SimulationWidget::getVZ() const
{
    std::vector<float> values;

    for (const auto& p : particles)
        values.push_back(p.vz);

    return values;
}

float SimulationWidget::getAverageSpeed() const
{
    float sum = 0.0f;

    for (const auto& p : particles)
    {
        float v = std::sqrt(
            p.vx * p.vx +
            p.vy * p.vy +
            p.vz * p.vz
            );

        sum += v;
    }

    return sum / particles.size();
}

float SimulationWidget::getAverageEnergy() const
{
    float sum = 0.0f;

    for (const auto& p : particles)
    {
        float v2 =
            p.vx * p.vx +
            p.vy * p.vy +
            p.vz * p.vz;

        sum += 0.5f * p.mass * v2;
    }

    return sum / particles.size();
}

int SimulationWidget::getParticleCount() const
{
    return particles.size();
}

float SimulationWidget::consumePressure()
{
    return currentPressure;
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
