#include "SimulationWindow.h"
#include "SimulationWidget.h"
#include "GraphsWindow.h"
#include "Constants.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollArea>
#include <QTimer>

SimulationWindow::SimulationWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Simulation");
    resize(1200, 700);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    sim = new SimulationWidget(centralWidget);
    sim->setMinimumSize(800, 600);

    setStyleSheet(
        "QPushButton {"
        "font-size: 18px;"
        "padding: 10px;"
        "border-radius: 10px;"
        "background-color: #4A90E2;"
        "color: white;"
        "}"
        "QPushButton:hover {"
        "background-color: #357ABD;"
        "}"
        );

    QVBoxLayout* leftSide = new QVBoxLayout();
    QVBoxLayout* controlPanel = new QVBoxLayout();
    QVBoxLayout* rightSide = new QVBoxLayout();
    QHBoxLayout* buttonPanel = new QHBoxLayout();

    QLabel* noteText = new QLabel(NOTE_TEXT);

    QSlider* tempSlider = new QSlider(Qt::Horizontal);
    tempSlider->setFixedSize(300, 25);
    tempSlider->setRange(1, 500);
    tempSlider->setValue(50);
    QLabel* tempValueLabel = new QLabel("Температура: " + QString::number(tempSlider->value()));
    tempValueLabel->setStyleSheet("font-size: 16px; color: black;");

    QSlider* countSlider = new QSlider(Qt::Horizontal);
    countSlider->setFixedSize(300, 25);
    countSlider->setRange(10, 3000);
    countSlider->setValue(100);
    QLabel* countValueLabel = new QLabel("Количество частиц: " + QString::number(countSlider->value()));
    countValueLabel->setStyleSheet("font-size: 16px; color: black;");

    connect(tempSlider, &QSlider::valueChanged,
            sim, &SimulationWidget::setTemperature);

    connect(tempSlider, &QSlider::valueChanged,
        [tempValueLabel](int value)
        {
            tempValueLabel->setText("Температура: " + QString::number(value));
        });

    connect(countSlider, &QSlider::valueChanged,
            sim, &SimulationWidget::setParticleCount);

    connect(countSlider, &QSlider::valueChanged,
        [countValueLabel](int value)
        {
            countValueLabel->setText("Количество частиц: " + QString::number(value));
        });

    graphicsButton = new QPushButton("Показать графики");
    tableButton = new QPushButton("Показать результаты");
    resetButton = new QPushButton("Сброс");

    graphicsButton->setFixedSize(200, 50);
    tableButton->setFixedSize(200, 50);
    resetButton->setFixedSize(200, 50);

    connect(graphicsButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                GraphDialog* dialog =
                    new GraphDialog(this->sim, this);

                dialog->exec();
            });

    connect(tableButton,
            &QPushButton::clicked,
            this,
            &SimulationWindow::updateStatistics);

    connect(resetButton,
            &QPushButton::clicked,
            this,
            &SimulationWindow::resetSimulation);


    ///=============== ТАБЛИЦА ============

    statsTable = new QTableWidget(5, 2);

    statsTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers);
    statsTable->verticalHeader()->setVisible(false);

    statsTable->setHorizontalHeaderLabels(
        {"Параметр", "Значение"}
        );

    statsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    statsTable->setItem(0, 0, new QTableWidgetItem("Температура"));
    statsTable->setItem(1, 0, new QTableWidgetItem("Количество частиц"));
    statsTable->setItem(2, 0, new QTableWidgetItem("Средняя скорость"));
    statsTable->setItem(3, 0, new QTableWidgetItem("Средняя кинетическая энергия"));
    statsTable->setItem(4, 0, new QTableWidgetItem("Давление"));

    for (int row = 0; row < 5; ++row)
    {
        statsTable->setItem(row, 1, new QTableWidgetItem(""));
    }



    ///==================== РАЗМЕЩЕНИЕ =============


    controlPanel->addWidget(tempValueLabel);
    controlPanel->addWidget(tempSlider);
    controlPanel->addWidget(countValueLabel);
    controlPanel->addWidget(countSlider);

    buttonPanel->addWidget(graphicsButton, 0, Qt::AlignTop);
    buttonPanel->addWidget(tableButton, 0, Qt::AlignTop);
    buttonPanel->addWidget(resetButton, 0, Qt::AlignTop);

    leftSide->addLayout(controlPanel, 1);
    leftSide->addWidget(sim, 3);

    rightSide->addLayout(buttonPanel);
    rightSide->addWidget(statsTable, Qt::AlignCenter);
    rightSide->addWidget(noteText);

    mainLayout->addLayout(leftSide, 1);
    mainLayout->addLayout(rightSide, 6);
}

void SimulationWindow::updateStatistics()
{
    statsTable->item(0,1)->setText(
        QString("%1 K")
            .arg(sim->getDisplayedTemperature(), 0, 'f', 0));

    statsTable->item(1,1)->setText(
        QString("%1 шт")
            .arg(sim->getParticleCount()));

    statsTable->item(2,1)->setText(
        QString("%1 м/с")
            .arg(sim->getAverageSpeed(), 0, 'f', 2));

    statsTable->item(3,1)->setText(
        QString("%1 Дж")
            .arg(sim->getAverageEnergy(), 0, 'e', 2));

    statsTable->item(4,1)->setText(
        QString("%1 Па")
            .arg(sim->getPressure(), 0, 'f', 2));
}

void SimulationWindow::resetSimulation()
{
    sim->resetSimulation();

    for (int row = 0; row < 5; ++row)
    {
        statsTable->item(row,1)->setText("");
    }
}