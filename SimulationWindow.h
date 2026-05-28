#ifndef SIMULATIONWINDOW_H
#define SIMULATIONWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollArea>
#include <QTimer>

class SimulationWidget;

class SimulationWindow : public QMainWindow
{
    Q_OBJECT

public:
    SimulationWindow(QWidget *parent = nullptr);

private slots:
    void updateStatistics();
    void resetSimulation();

private:
    SimulationWidget* sim;
    QPushButton *graphicsButton;
    QPushButton *tableButton;
    QPushButton *resetButton;
    QTableWidget* statsTable;

};

#endif