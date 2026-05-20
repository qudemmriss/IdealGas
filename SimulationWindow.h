#ifndef SIMULATIONWINDOW_H
#define SIMULATIONWINDOW_H

#include <QMainWindow>
#include <QPushButton>

class SimulationWindow : public QMainWindow
{
    Q_OBJECT

public:
    SimulationWindow(QWidget *parent = nullptr);

private:
    QPushButton *graphicsButton;
    QPushButton *tableButton;
    QPushButton *resetButton;

};

#endif