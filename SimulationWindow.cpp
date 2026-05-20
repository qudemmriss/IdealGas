#include "SimulationWindow.h"
#include "SimulationWidget.h"
#include "GraphsWindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QPushButton>

SimulationWindow::SimulationWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Simulation");
    resize(1200, 700);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    SimulationWidget* sim = new SimulationWidget(centralWidget);
    sim->setMinimumSize(800, 600);

    setStyleSheet(
        "QPushButton {"
        "font-size: 18px;"
        "padding: 10px;"
        "border-radius: 10px;"
        "background-color: #4A90E2;"
        "color: white;"
        "background-image:url(/Users/angelinamirnaa/IdealGas_/cat.jpg);"
        "}"
        "QPushButton:hover {"
        "background-color: #357ABD;"
        "}"
        );

    QVBoxLayout* mainLayout1 = new QVBoxLayout();
    QVBoxLayout* controlPanel = new QVBoxLayout();
    QHBoxLayout* buttonPanel = new QHBoxLayout();

    QLabel* tempLabel = new QLabel("Температура");
    QSlider* tempSlider = new QSlider(Qt::Horizontal);
    tempSlider->setFixedSize(300, 25);
    tempSlider->setRange(1, 100);
    tempSlider->setValue(50);

    QLabel* countLabel = new QLabel("Количество частиц");
    QSlider* countSlider = new QSlider(Qt::Horizontal);
    countSlider->setFixedSize(300, 25);
    countSlider->setRange(10, 300);
    countSlider->setValue(100);

    connect(tempSlider, &QSlider::valueChanged,
            sim, &SimulationWidget::setTemperature);

    connect(countSlider, &QSlider::valueChanged,
            sim, &SimulationWidget::setParticleCount);

    graphicsButton = new QPushButton("Показать графики");
    tableButton = new QPushButton("Показать результаты");
    resetButton = new QPushButton("Сброс");

    graphicsButton->setFixedSize(200, 50);
    tableButton->setFixedSize(200, 50);
    resetButton->setFixedSize(200, 50);

    connect(graphicsButton, &QPushButton::clicked, this, [this](){
        GraphDialog* dialog = new GraphDialog(this);
        dialog->exec();

    });

    controlPanel->addWidget(tempLabel);
    controlPanel->addWidget(tempSlider);
    controlPanel->addWidget(countLabel);
    controlPanel->addWidget(countSlider);

    buttonPanel->addWidget(graphicsButton, 0, Qt::AlignTop);
    buttonPanel->addWidget(tableButton, 0, Qt::AlignTop);
    buttonPanel->addWidget(resetButton, 0, Qt::AlignTop);

    mainLayout1->addLayout(controlPanel, 1);
    mainLayout1->addWidget(sim, 3);

    mainLayout->addLayout(mainLayout1, 1);
    mainLayout->addLayout(buttonPanel, 6);
}