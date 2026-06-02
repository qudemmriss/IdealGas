#include "MainWindow.h"
#include "SimulationWindow.h"
#include "Constants.h"

#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QDialog>
#include <QScrollArea>
#include <QLabel>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Модель идеального газа");
    resize(600, 600);

    setStyleSheet(
        "QMainWindow {"
        "background-image: url(:/new/prefix1/3.jpg);"
        "background-repeat: no-repeat;"
        "background-position: center;"
        "}"
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

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    startButton = new QPushButton("Начать");
    aboutButton = new QPushButton("О программе");
    exitButton = new QPushButton("Выход");

    startButton->setFixedSize(200, 50);
    aboutButton->setFixedSize(200, 50);
    exitButton->setFixedSize(200, 50);

    layout->addWidget(startButton, 0, Qt::AlignCenter);
    layout->addWidget(aboutButton, 0, Qt::AlignCenter);
    layout->addWidget(exitButton, 0, Qt::AlignCenter);

    QList<QPushButton*> buttons = {startButton, aboutButton, exitButton};

    for (QPushButton* button : buttons)
    {
        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
        shadow->setBlurRadius(20);
        shadow->setOffset(4, 4);
        shadow->setColor(QColor(5, 55, 123));

        button->setGraphicsEffect(shadow);
    }

    connect(startButton, &QPushButton::clicked,
            this, &MainWindow::startSimulation);

    connect(aboutButton, &QPushButton::clicked,
            this, &MainWindow::showAbout);

    connect(exitButton, &QPushButton::clicked,
            this, &MainWindow::close);
}

void MainWindow::startSimulation()
{
    SimulationWindow* simWindow = new SimulationWindow();
    simWindow->show();
    QTimer::singleShot(0, this, &MainWindow::close);

    this->close();
}

void MainWindow::showAbout()
{
    QDialog* aboutDialog = new QDialog(this);
    aboutDialog->setWindowTitle("О программе");
    aboutDialog->resize(800, 600);

    QVBoxLayout* layout = new QVBoxLayout(aboutDialog);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(15);
    layout->setContentsMargins(20, 20, 20, 20);

    QLabel* textLabel = new QLabel(ABOUT_TEXT);

    textLabel->setWordWrap(true);
    textLabel->setStyleSheet("font-size: 18px; padding: 20px;");
    textLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(textLabel);
    scrollArea->setWidgetResizable(true);

    layout->addWidget(scrollArea);

    aboutDialog->exec();
}