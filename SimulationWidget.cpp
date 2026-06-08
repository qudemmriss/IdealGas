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

std::unordered_map<long long, GridCell> spatialGrid;

float cellSize = 0.06f;

long long SimulationWidget::hashCell(
    int x,
    int y,
    int z) const
{
    const long long p1 = 73856093;
    const long long p2 = 19349663;
    const long long p3 = 83492791;

    return
        (x * p1) ^
        (y * p2) ^
        (z * p3);
}

void SimulationWidget::buildSpatialGrid()
{
    spatialGrid.clear();

    for (int i = 0; i < particles.size(); ++i)
    {
        const auto& p = particles[i];

        int cx = static_cast<int>(
            std::floor(p.x / cellSize));

        int cy = static_cast<int>(
            std::floor(p.y / cellSize));

        int cz = static_cast<int>(
            std::floor(p.z / cellSize));

        long long h = hashCell(cx, cy, cz);

        spatialGrid[h].particles.push_back(i);
    }
}

void SimulationWidget::solveCollisions()
{
    for (int i = 0; i < particles.size(); ++i)
    {
        auto& p = particles[i];

        int cx = static_cast<int>(
            std::floor(p.x / cellSize));

        int cy = static_cast<int>(
            std::floor(p.y / cellSize));

        int cz = static_cast<int>(
            std::floor(p.z / cellSize));

        // Проверяем свою и соседние клетки
        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dz = -1; dz <= 1; ++dz)
                {
                    long long h =
                        hashCell(
                            cx + dx,
                            cy + dy,
                            cz + dz);

                    auto it = spatialGrid.find(h);

                    if (it == spatialGrid.end())
                        continue;

                    const auto& cellParticles =
                        it->second.particles;

                    for (int j : cellParticles)
                    {
                        if (j <= i)
                            continue;

                        resolveCollision(
                            particles[i],
                            particles[j]);
                    }
                }
            }
        }
    }
}

void SimulationWidget::resolveCollision(
    Particle& a,
    Particle& b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float dz = b.z - a.z;

    float dist2 =
        dx * dx +
        dy * dy +
        dz * dz;

    float minDist =
        a.radius + b.radius;

    if (dist2 >= minDist * minDist)
        return;

    float dist = std::sqrt(dist2);

    if (dist < 1e-6f)
        return;

    float nx = dx / dist;
    float ny = dy / dist;
    float nz = dz / dist;

    float rvx = b.vx - a.vx;
    float rvy = b.vy - a.vy;
    float rvz = b.vz - a.vz;

    float velAlongNormal =
        rvx * nx +
        rvy * ny +
        rvz * nz;

    // Уже разлетаются
    if (velAlongNormal > 0.0f)
        return;

    float restitution = 1.0f;

    float invMassA = 1.0f / a.mass;
    float invMassB = 1.0f / b.mass;

    float j = -(1.0f + restitution) * velAlongNormal;

    j /= (invMassA + invMassB);

    float impulseX = j * nx;
    float impulseY = j * ny;
    float impulseZ = j * nz;

    a.vx -= impulseX * invMassA;
    a.vy -= impulseY * invMassA;
    a.vz -= impulseZ * invMassA;

    b.vx += impulseX * invMassB;
    b.vy += impulseY * invMassB;
    b.vz += impulseZ * invMassB;

    // Position correction
    float overlap = minDist - dist;

    constexpr float percent = 0.8f;

    float correction = overlap * percent;

    float correctionX = correction * nx;
    float correctionY = correction * ny;
    float correctionZ = correction * nz;

    float totalInvMass = invMassA + invMassB;

    a.x -= correctionX * (invMassA / totalInvMass);
    a.y -= correctionY * (invMassA / totalInvMass);
    a.z -= correctionZ * (invMassA / totalInvMass);

    b.x += correctionX * (invMassB / totalInvMass);
    b.y += correctionY * (invMassB / totalInvMass);
    b.z += correctionZ * (invMassB / totalInvMass);
}

void SimulationWidget::resolveWallCollision(
    float& pos,
    float& vel,
    float radius,
    float mass)
{
    const float min = -1.0f + radius;
    const float max =  1.0f - radius;

    if (pos < min)
    {
        pos = min;

        float impulse =
            2.0f * mass * std::abs(vel);

        pressureAccumulator += impulse;

        vel = std::abs(vel);
    }

    if (pos > max)
    {
        pos = max;

        float impulse =
            2.0f * mass * std::abs(vel);

        pressureAccumulator += impulse;

        vel = -std::abs(vel);
    }
}

void SimulationWidget::updateSimulation()
{
    constexpr int substeps = 4;

    float subDt = dt / substeps;

    for (int step = 0; step < substeps; ++step)
    {
        // integrate
        for (auto& p : particles)
        {
            p.x += p.vx * subDt;
            p.y += p.vy * subDt;
            p.z += p.vz * subDt;
        }

        // broad phase
        buildSpatialGrid();

        // narrow phase
        solveCollisions();

        // walls
        for (auto& p : particles)
        {
            resolveWallCollision(
                p.x,
                p.vx,
                p.radius,
                p.mass);

            resolveWallCollision(
                p.y,
                p.vy,
                p.radius,
                p.mass);

            resolveWallCollision(
                p.z,
                p.vz,
                p.radius,
                p.mass);
        }
    }

    float instantPressure =
        pressureAccumulator
        / (6.0f * wallArea * dt);

    currentPressure = instantPressure;

    pressureSum += instantPressure;
    pressureSamples++;

    pressureAccumulator = 0.0f;

    updateParticleBuffer();

    update();
}

void SimulationWidget::resetSimulation()
{
    currentTemperature = 50.0f;
    displayedTemperature = 50.0f;

    createParticles(100);

    pressureAccumulator = 0.0f;
    currentPressure = 0.0f;
    pressureSum = 0.0f;
    pressureSamples = 0;

    makeCurrent();
    updateParticleBuffer();
    doneCurrent();

    update();
}

float SimulationWidget::getPressure() const
{
    return currentPressure;
}

float SimulationWidget::getTemperature() const
{
    if (particles.empty())
        return 0.0f;

    float energy = 0.0f;

    for (const auto& p : particles)
    {
        energy += p.vx * p.vx + p.vy * p.vy + p.vz * p.vz;
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
    if (particles.empty())
        return 0.0f;

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
    if (particles.empty())
        return 0.0f;

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
    if (pressureSamples == 0)
        return currentPressure;

    float averagePressure =
        pressureSum / pressureSamples;

    pressureSum = 0.0f;
    pressureSamples = 0;

    return averagePressure;
}

void SimulationWidget::setTemperature(float value)
{
    if (value <= 0.0f)
        value = 1.0f;

    displayedTemperature = value;

    if (currentTemperature <= 0.0f)
        currentTemperature = value;

    float scale = std::sqrt(value / currentTemperature);

    for (auto& p : particles)
    {
        p.vx *= scale;
        p.vy *= scale;
        p.vz *= scale;
    }

    currentTemperature = value;
}

float SimulationWidget::getDisplayedTemperature() const
{
    return displayedTemperature;
}

void SimulationWidget::setParticleCount(int count)
{
    if (count <= 0)
        count = 1;

    createParticles(count);

    makeCurrent();
    updateParticleBuffer();
    doneCurrent();

    update();
}
