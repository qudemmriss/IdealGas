#ifndef SIMULATIONWIDGET_H
#define SIMULATIONWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <vector>
#include <QMouseEvent>
#include <QWheelEvent>
#include <unordered_map>
#include <cmath>

struct Particle
{
    float x, y, z;
    float vx, vy, vz;

    float radius = 0.03f;
    float mass = 1.0f;
};

struct GridCell
{
    std::vector<int> particles;
};

class SimulationWidget : public QOpenGLWidget,
                         protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit SimulationWidget(QWidget* parent = nullptr);
    ~SimulationWidget();

    void setTemperature(float value);
    void setParticleCount(int count);
    void resetSimulation();

    float getPressure() const;
    float getTemperature() const;
    float getAverageSpeed() const;
    float getAverageEnergy() const;
    int getParticleCount() const;
    float getDisplayedTemperature() const;

    long long hashCell(
        int x,
        int y,
        int z) const;

    std::vector<float> getSpeeds() const;

    std::vector<float> getVX() const;
    std::vector<float> getVY() const;
    std::vector<float> getVZ() const;

    float consumePressure();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void updateSimulation();

private:
    void initCube();
    void initParticles();
    void updateParticleBuffer();
    void createParticles(int count);

    void buildSpatialGrid();

    void resolveWallCollision(float& pos, float& vel, float radius);
    void solveCollisions();
    void resolveCollision(Particle& a, Particle& b);

    std::vector<Particle> particles;

    QOpenGLShaderProgram program;

    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;

    GLuint particleVAO = 0;
    GLuint particleVBO = 0;

    float currentTemperature = 50.0f;
    float pressureAccumulator = 0.0f;
    float currentPressure = 0.0f;

    float rotationX = 25.0f;
    float rotationY = 35.0f;

    float cameraDistance = 5.0f;

    float displayedTemperature = 0.0f;

    const float dt = 0.016f;
    const float wallArea = 4.0f;

    QPoint lastMousePosition;

    QTimer timer;
};

#endif