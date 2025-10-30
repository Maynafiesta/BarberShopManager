// project/ui/MainWindow.cpp
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("BarberShopManager");
}

MainWindow::~MainWindow() {
    delete ui;
}
