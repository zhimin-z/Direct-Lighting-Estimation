#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <cameracontrolshelp.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionLoad_Scene_triggered()
{
    ui->mygl->SceneLoadDialog();
}

void MainWindow::slot_ResizeToCamera(int w, int h)
{
    setFixedWidth(w);
    setFixedWidth(h);
}

void MainWindow::on_actionRender_triggered()
{
    ui->mygl->RaytraceScene();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}
