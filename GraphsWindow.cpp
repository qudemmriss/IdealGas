#include "GraphsWindow.h"
#include "SimulationWidget.h"

#include <QtCharts>
#include <QVBoxLayout>
#include <cmath>
#include <QTabWidget>
#include <QScreen>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

GraphDialog::GraphDialog(
    SimulationWidget* sim,
    QWidget *parent)
    : QDialog(parent),
    simulation(sim)

{
    setWindowTitle("Графики");

    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    int w = std::min(700, int(screenGeometry.width() * 0.9));
    int h = std::min(700, int(screenGeometry.height() * 0.9));

    resize(w, h);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QTabWidget* tabs = new QTabWidget(this);
    layout->addWidget(tabs);

    // MAXWELL

    experimentalSeries = new QLineSeries();
    experimentalSeries->setName("Эксперимент");
    experimentalSeries->setColor(Qt::blue);

    maxwellSeries = new QSplineSeries();
    maxwellSeries->setName("Теория Максвелла");
    maxwellSeries->setColor(Qt::red);

    speedChart = new QChart();
    speedChart->setMargins(QMargins(5, 5, 5, 5));

    speedChart->addSeries(experimentalSeries);
    speedChart->addSeries(maxwellSeries);
    speedChart->setTitle("Распределение скоростей");

    speedAxisX = new QValueAxis();
    speedAxisY = new QValueAxis();

    speedAxisX->setTitleText("v, м/с");
    speedAxisY->setTitleText("f(v)");

    speedAxisX->setRange(0, 2.5);
    speedAxisX->setTickCount(6);

    speedAxisY->setRange(0, 1.5);
    speedAxisY->setTickCount(6);

    speedChart->addAxis(
        speedAxisX,
        Qt::AlignBottom);

    speedChart->addAxis(
        speedAxisY,
        Qt::AlignLeft);

    experimentalSeries->attachAxis(speedAxisX);
    experimentalSeries->attachAxis(speedAxisY);

    maxwellSeries->attachAxis(speedAxisX);
    maxwellSeries->attachAxis(speedAxisY);

    QChartView* speedView =
        new QChartView(speedChart);

    speedView->setRenderHint(
        QPainter::Antialiasing);

    tabs->addTab(speedView, "Скорости");

    // VX VY VZ

    vxSeries = new QSplineSeries();
    vySeries = new QSplineSeries();
    vzSeries = new QSplineSeries();

    vxSeries->setName("vx");
    vySeries->setName("vy");
    vzSeries->setName("vz");

    vxSeries->setColor(Qt::red);
    vySeries->setColor(Qt::green);
    vzSeries->setColor(Qt::blue);

    componentChart = new QChart();
    componentChart->setMargins(QMargins(5, 5, 5, 5));

    componentChart->addSeries(vxSeries);
    componentChart->addSeries(vySeries);
    componentChart->addSeries(vzSeries);

    componentChart->setTitle(
        "Распределение компонент скорости"
        );

    componentAxisX = new QValueAxis();
    componentAxisY = new QValueAxis();

    componentAxisX->setTitleText("vx, vy, vz");
    componentAxisY->setTitleText("N");

    componentAxisX->setRange(-2.0, 2.0);
    componentAxisX->setTickCount(9);

    componentAxisY->setRange(-20, 400);
    componentAxisY->setTickCount(13);

    componentChart->addAxis(
        componentAxisX,
        Qt::AlignBottom);

    componentChart->addAxis(
        componentAxisY,
        Qt::AlignLeft);

    vxSeries->attachAxis(componentAxisX);
    vxSeries->attachAxis(componentAxisY);

    vySeries->attachAxis(componentAxisX);
    vySeries->attachAxis(componentAxisY);

    vzSeries->attachAxis(componentAxisX);
    vzSeries->attachAxis(componentAxisY);

    QChartView* componentView =
        new QChartView(componentChart);

    componentView->setRenderHint(
        QPainter::Antialiasing);

    tabs->addTab(componentView, "Компоненты");

    // PRESSURE

    pressureSeries = new QLineSeries();
    pressureSeries->setName("Эксперимент");
    pressureSeries->setColor(Qt::blue);

    pressureTheorySeries = new QSplineSeries();
    pressureTheorySeries->setName("Теория идеального газа");
    pressureTheorySeries->setColor(Qt::red);

    pressureChart = new QChart();
    pressureChart->setMargins(QMargins(5, 5, 5, 5));

    pressureChart->addSeries(pressureSeries);
    pressureChart->addSeries(pressureTheorySeries);

    pressureChart->setTitle(
        "Давление на стенки"
        );

    pressureAxisX = new QValueAxis();
    pressureAxisY = new QValueAxis();

    pressureAxisX->setTitleText("t, с");
    pressureAxisY->setTitleText("P, Па");

    pressureAxisX->setRange(0, 200);
    pressureAxisY->setRange(0, 10);

    pressureChart->addAxis(
        pressureAxisX,
        Qt::AlignBottom);

    pressureChart->addAxis(
        pressureAxisY,
        Qt::AlignLeft);

    pressureSeries->attachAxis(pressureAxisX);
    pressureSeries->attachAxis(pressureAxisY);

    pressureTheorySeries->attachAxis(pressureAxisX);
    pressureTheorySeries->attachAxis(pressureAxisY);

    QWidget* pressureTab = new QWidget(this);
    QVBoxLayout* pressureLayout = new QVBoxLayout(pressureTab);

    QHBoxLayout* pressureControls = new QHBoxLayout();

    QLabel* rangeLabel =
        new QLabel("Показывать последних точек:", pressureTab);

    pressureRangeSpin =
        new QSpinBox(pressureTab);

    pressureRangeSpin->setRange(50, 2000);
    pressureRangeSpin->setSingleStep(50);
    pressureRangeSpin->setValue(pressureVisiblePoints);

    pressureControls->addWidget(rangeLabel);
    pressureControls->addWidget(pressureRangeSpin);
    pressureControls->addStretch();

    pressureLayout->addLayout(pressureControls);

    QChartView* pressureView =
        new QChartView(pressureChart);

    pressureView->setRenderHint(
        QPainter::Antialiasing);

    pressureLayout->addWidget(pressureView);

    tabs->addTab(pressureTab, "Давление");

    connect(pressureRangeSpin,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            [this](int value)
            {
                pressureVisiblePoints = value;
            });

    connect(&graphTimer,
            &QTimer::timeout,
            this,
            &GraphDialog::updateGraphs);

    graphTimer.start(100);
}

GraphDialog::~GraphDialog()
{
    graphTimer.stop();

    disconnect(&graphTimer, nullptr, this, nullptr);
}

void GraphDialog::updateGraphs()
{
    updateMaxwellGraph();
    updateComponentGraph();
    updatePressureGraph();
}

// MAXWELL

void GraphDialog::updateMaxwellGraph()
{
    auto speeds =
        simulation->getSpeeds();

    int bins = 60;

    float maxSpeed = 0.001f;

    for (float v : speeds)
    {
        maxSpeed = std::max(maxSpeed, v);
    }

    std::vector<float> hist(bins, 0.0f);

    // Гистограмма
    for (float v : speeds)
    {
        int index =
            int(v / maxSpeed * (bins - 1));

        if (index >= 0 && index < bins)
        {
            hist[index] += 1.0f;
        }
    }

    experimentalSeries->clear();
    maxwellSeries->clear();

    float dv = maxSpeed / bins;

    // Нормировка экспериментальной кривой
    for (int i = 0; i < bins; ++i)
    {
        float x =
            (i + 0.5f) * dv;

        float density =
            hist[i]
            / (speeds.size() * dv);

        experimentalSeries->append(x, density);
    }

    // Теория Максвелла
    float T =
        simulation->getTemperature();

    float maxTheory = 0.0f;

    std::vector<QPointF> theoryPoints;

    for (int i = 0; i < 200; ++i)
    {
        float v =
            maxSpeed * i / 200.0f;

        float a = 1.0f / T;

        float f =
            4.0f
            * M_PI
            * std::pow(a / M_PI, 1.5f)
            * v * v
            * std::exp(-a * v * v);

        theoryPoints.push_back(QPointF(v, f));

        maxTheory = std::max(maxTheory, f);
    }

    for (auto& p : theoryPoints)
    {
        maxwellSeries->append(p);
    }

}

// COMPONENTS

void GraphDialog::updateComponentGraph()
{
    auto vx = simulation->getVX();
    auto vy = simulation->getVY();
    auto vz = simulation->getVZ();

    int bins = 20;

    float minV = -2.5f;
    float maxV = 2.5f;

    std::vector<int> histVX(
        bins, 0);

    std::vector<int> histVY(
        bins, 0);

    std::vector<int> histVZ(
        bins, 0);

    for (float v : vx)
    {
        int index =
            ((v - minV)
             / (maxV - minV))
            * (bins - 1);

        if (index >= 0 && index < bins)
            histVX[index]++;
    }

    for (float v : vy)
    {
        int index =
            ((v - minV)
             / (maxV - minV))
            * (bins - 1);

        if (index >= 0 && index < bins)
            histVY[index]++;
    }

    for (float v : vz)
    {
        int index =
            ((v - minV)
             / (maxV - minV))
            * (bins - 1);

        if (index >= 0 && index < bins)
            histVZ[index]++;
    }

    vxSeries->clear();
    vySeries->clear();
    vzSeries->clear();

    int maxCount = 1;

    for (int i = 0; i < bins; ++i)
    {
        float x =
            minV +
            (maxV - minV)
                * i / bins;

        vxSeries->append(
            x,
            histVX[i]);

        vySeries->append(
            x,
            histVY[i]);

        vzSeries->append(
            x,
            histVZ[i]);

        maxCount =
            std::max(maxCount,
                     histVX[i]);

        maxCount =
            std::max(maxCount,
                     histVY[i]);

        maxCount =
            std::max(maxCount,
                     histVZ[i]);
    }

}

// PRESSURE

void GraphDialog::updatePressureGraph()
{
    static float smoothPressure = 0.0f;

    float current =
        simulation->consumePressure();

    smoothPressure =
        0.9f * smoothPressure +
        0.1f * current;

    int N =
        simulation->getParticleCount();

    const float volume = 8.0f;

    float averageEnergy =
        simulation->getAverageEnergy();

    float theoryPressure =
        2.0f * N * averageEnergy
        / (3.0f * volume);

    pressureSeries->append(
        pressureTime,
        smoothPressure
        );

    pressureTheorySeries->append(
        pressureTime,
        theoryPressure
        );

    pressureTime++;

    while (pressureSeries->count() > pressureVisiblePoints)
    {
        pressureSeries->remove(0);
    }

    while (pressureTheorySeries->count() > pressureVisiblePoints)
    {
        pressureTheorySeries->remove(0);
    }

    pressureAxisX->setRange(
        std::max(0, pressureTime - pressureVisiblePoints),
        pressureTime
        );

    float maxPressure =
        std::max(
            smoothPressure,
            theoryPressure
            );

    pressureAxisY->setRange(
        0,
        std::max(1.0f, maxPressure * 1.5f)
        );
}