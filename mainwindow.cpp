#include "mainwindow.h"
#include "SimulationWindow.h"

#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QDialog>
#include <QScrollArea>
#include <QLabel>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Ideal Gas Model");
    resize(900, 600);

    setStyleSheet(
        "QMainWindow {"
        "background-image: url(/Users/angelinamirnaa/IdealGas_/elsa.jpg);"
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

    QLabel* textLabel = new QLabel(
        "Программа предназначена для:\n"
        "   - Моделирования поведения частиц идеального газа на основе\n"
        "  положений молекулярно-кинетической теории в ограниченном сосуде;\n"
        "   - Вычисления следующих макроскопических параметров системы:\n"
        "           1. средняя скорость движения молекул;\n"
        "           2. средняя кинетическая энергия молекул;\n"
        "           3. давление на стенки сосуда.\n"
        "   - Отображения следующих графиков:\n"
        "           1. распределение скоростей (гистограмма и кривая Максвелла);\n"
        "           2. распределение по компонентам v_x,v_y,v_z разными цветами\n"
        "               в одной плоскости;\n"
        "           3. зависимость давления P(t) на стенках сосуда.\n\n"
        "Данная программа может использоваться студентами и учениками школ\n"
        "для выполнения лабораторных и курсовых работ по молекулярной физике;\n"
        "преподавателями для демонстрации движения молекул идеального газа и\n"
        "сопутствующих показателей и изменений; в прочих учебных целях для\n"
        "визуализации статистических закономерностей.\n\n"
        "Программа предусмотрена для эксплуатации на персональных\n"
        "компьютерах в учебных аудиториях и домашних условиях. Эксплуатация\n"
        "осуществляется пользователями без специальной подготовки в области\n"
        "программирования.\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
        "Разработчицы: Букетова Д. С., Мирная А. В..\n"
        "Краснодар, ФГБОУ ВО «КубГУ», 2026.\n"
        );

    textLabel->setWordWrap(true);
    textLabel->setStyleSheet("font-size: 18px; padding: 20px;");
    textLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(textLabel);
    scrollArea->setWidgetResizable(true);

    layout->addWidget(scrollArea);

    aboutDialog->exec();
}