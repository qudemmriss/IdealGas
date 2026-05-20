#ifndef SIMULATIONWIDGET_H
#define SIMULATIONWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <vector>

struct Particle
{
    float x, y, z;
    float vx, vy, vz;

    float radius = 0.05f;
    float mass = 1.0f;
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

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private slots:
    void updateSimulation();

private:
    void initCube();
    void initParticles();
    void updateParticleBuffer();
    void createParticles(int count);
    void resolveCollision(Particle& a, Particle& b);

    std::vector<Particle> particles;

    QOpenGLShaderProgram program;

    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;

    GLuint particleVAO = 0;
    GLuint particleVBO = 0;

    float currentTemperature = 50.0f;

    QTimer timer;
};

#endif