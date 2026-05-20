#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void startSimulation();
    void showAbout();

private:
    QPushButton* startButton;
    QPushButton* aboutButton;
    QPushButton* exitButton;
};

#endif