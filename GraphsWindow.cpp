#include "GraphsWindow.h"
#include "SimulationWidget.h"

#include <QtCharts>

#include <QVBoxLayout>
#include <cmath>

GraphDialog::GraphDialog(
    SimulationWidget* sim,
    QWidget *parent)
    : QDialog(parent),
    simulation(sim)
{
    setWindowTitle("Графики");
    resize(1400, 1000);

    QVBoxLayout* layout =
        new QVBoxLayout(this);

    // MAXWELL

    experimentalSeries = new QLineSeries();
    experimentalSeries->setName("Эксперимент");
    experimentalSeries->setColor(Qt::blue);

    maxwellSeries = new QSplineSeries();
    maxwellSeries->setName("Теория Максвелла");
    maxwellSeries->setColor(Qt::red);

    speedChart = new QChart();

    speedChart->addSeries(experimentalSeries);
    speedChart->addSeries(maxwellSeries);
    speedChart->setTitle("Распределение скоростей");

    speedAxisX = new QValueAxis();
    speedAxisY = new QValueAxis();

    speedAxisX->setTitleText("Скорость v");
    speedAxisY->setTitleText("Плотность распределения f(v)");

    speedAxisX->setRange(0, 1.5);
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

    layout->addWidget(speedView);

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

    componentChart->addSeries(vxSeries);
    componentChart->addSeries(vySeries);
    componentChart->addSeries(vzSeries);

    componentChart->setTitle(
        "Распределение компонент скорости"
        );

    componentAxisX = new QValueAxis();
    componentAxisY = new QValueAxis();

    componentAxisX->setTitleText("Компонента скорости");
    componentAxisY->setTitleText("Количество частиц");

    componentAxisX->setRange(-2.0, 2.0);
    componentAxisX->setTickCount(9);

    componentAxisY->setRange(-20, 220);
    componentAxisY->setTickCount(6);

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

    layout->addWidget(componentView);

    // PRESSURE

    pressureSeries =
        new QLineSeries();

    pressureSeries->setColor(Qt::blue);

    pressureChart = new QChart();

    pressureChart->addSeries(
        pressureSeries);

    pressureChart->setTitle(
        "Давление на стенки"
        );

    pressureAxisX = new QValueAxis();
    pressureAxisY = new QValueAxis();

    pressureAxisX->setTitleText("Время");
    pressureAxisY->setTitleText("Давление");

    pressureAxisX->setRange(0, 200);

    pressureAxisY->setRange(0, 10);

    pressureChart->addAxis(
        pressureAxisX,
        Qt::AlignBottom);

    pressureChart->addAxis(
        pressureAxisY,
        Qt::AlignLeft);

    pressureSeries->attachAxis(
        pressureAxisX);

    pressureSeries->attachAxis(
        pressureAxisY);

    QChartView* pressureView =
        new QChartView(pressureChart);

    pressureView->setRenderHint(
        QPainter::Antialiasing);

    layout->addWidget(pressureView);

    connect(&graphTimer,
            &QTimer::timeout,
            this,
            &GraphDialog::updateGraphs);

    graphTimer.start(100);
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

    pressureSeries->append(
        pressureTime++,
        smoothPressure
        );

    if (pressureSeries->count() > 200)
    {
        pressureSeries->remove(0);
    }

    pressureAxisX->setRange(
        std::max(0, pressureTime - 200),
        pressureTime
        );

    pressureAxisY->setRange(
        0,
        std::max(1.0,
                 smoothPressure * 1.5)
        );
}