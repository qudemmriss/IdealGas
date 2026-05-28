#ifndef GRAPHSWINDOW_H
#define GRAPHSWINDOW_H

#include <QDialog>
#include <QTimer>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>

class SimulationWidget;

class GraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GraphDialog(
        SimulationWidget* sim,
        QWidget *parent = nullptr);

private slots:
    void updateGraphs();

private:

    void updateMaxwellGraph();
    void updateComponentGraph();
    void updatePressureGraph();

    SimulationWidget* simulation;

    QTimer graphTimer;

    // Maxwell

    QChart* speedChart;

    QLineSeries* experimentalSeries;
    QSplineSeries* maxwellSeries;

    QValueAxis* speedAxisX;
    QValueAxis* speedAxisY;

    // Components

    QChart* componentChart;

    QSplineSeries* vxSeries;
    QSplineSeries* vySeries;
    QSplineSeries* vzSeries;

    QValueAxis* componentAxisX;
    QValueAxis* componentAxisY;

    // Pressure

    QChart* pressureChart;

    QLineSeries* pressureSeries;

    QValueAxis* pressureAxisX;
    QValueAxis* pressureAxisY;

    int pressureTime = 0;
};

#endif