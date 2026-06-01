#include "SimulationWindow.h"
#include "SimulationWidget.h"
#include "GraphsWindow.h"
#include "Constants.h"
#include "MainWindow.h"

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
    resize(700, 700);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout* contentLayout = new QHBoxLayout();

    sim = new SimulationWidget(centralWidget);
    sim->setMinimumSize(500, 400);

    setStyleSheet(
        "QPushButton {"
        "font-size: 17px;"
        "padding: 1px;"
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

    tempSlider = new QSlider(Qt::Horizontal);
    tempSlider->setFixedSize(300, 25);
    tempSlider->setRange(1, 500);
    tempSlider->setValue(50);

    tempValueLabel = new QLabel("Температура: " + QString::number(tempSlider->value()));
    tempValueLabel->setStyleSheet("font-size: 16px; color: black;");

    countSlider = new QSlider(Qt::Horizontal);
    countSlider->setFixedSize(300, 25);
    countSlider->setRange(10, 3000);
    countSlider->setValue(100);

    countValueLabel = new QLabel("Количество частиц: " + QString::number(countSlider->value()));
    countValueLabel->setStyleSheet("font-size: 16px; color: black;");

    connect(tempSlider, &QSlider::valueChanged,
            sim, &SimulationWidget::setTemperature);

    connect(tempSlider, &QSlider::valueChanged,
            this,
            [this](int value)
            {
                tempValueLabel->setText("Температура: " + QString::number(value));
            });

    connect(countSlider, &QSlider::valueChanged,
            sim, &SimulationWidget::setParticleCount);

    connect(countSlider, &QSlider::valueChanged,
            this,
            [this](int value)
            {
                countValueLabel->setText("Количество частиц: " + QString::number(value));
            });

    backButton = new QPushButton("Назад");
    graphicsButton = new QPushButton("Показать графики");
    tableButton = new QPushButton("Показать результаты");
    resetButton = new QPushButton("Сброс");

    backButton->setFixedSize(100, 40);
    graphicsButton->setFixedSize(180, 40);
    tableButton->setFixedSize(180, 40);
    resetButton->setFixedSize(100, 40);

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

    connect(backButton, &QPushButton::clicked,
            this,
            [this]()
            {
                MainWindow* mainWindow = new MainWindow();
                mainWindow->show();

                this->close();
            });


    ///=============== ТАБЛИЦА ============

    statsTable = new QTableWidget(5, 2);
    statsTable->setMaximumWidth(350);
    statsTable->setMinimumWidth(250);

    statsTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers);
    statsTable->verticalHeader()->setVisible(false);

    statsTable->setHorizontalHeaderLabels(
        {"Параметр", "Значение"}
        );

    statsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    statsTable->verticalHeader()->setDefaultSectionSize(25);
    statsTable->setMaximumHeight(180);

    statsTable->setItem(0, 0, new QTableWidgetItem("Температура"));
    statsTable->setItem(1, 0, new QTableWidgetItem("Количество частиц"));
    statsTable->setItem(2, 0, new QTableWidgetItem("Средняя скорость"));
    statsTable->setItem(3, 0, new QTableWidgetItem("Средняя кин. энергия"));
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

    buttonPanel->addWidget(backButton);
    buttonPanel->addWidget(graphicsButton);
    buttonPanel->addWidget(tableButton);
    buttonPanel->addWidget(resetButton);

    leftSide->addLayout(controlPanel, 1);
    leftSide->addWidget(sim, 3);

    rightSide->addWidget(statsTable);
    rightSide->addStretch();
    rightSide->addWidget(noteText);

    contentLayout->addLayout(leftSide, 5);
    contentLayout->addLayout(rightSide, 1);

    mainLayout->addLayout(buttonPanel);
    mainLayout->addLayout(contentLayout);

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
    tempSlider->blockSignals(true);
    countSlider->blockSignals(true);

    tempSlider->setValue(50);
    countSlider->setValue(100);

    tempSlider->blockSignals(false);
    countSlider->blockSignals(false);

    tempValueLabel->setText("Температура: 50");
    countValueLabel->setText("Количество частиц: 100");

    sim->resetSimulation();

    for (int row = 0; row < 5; ++row)
    {
        statsTable->item(row, 1)->setText("");
    }
}