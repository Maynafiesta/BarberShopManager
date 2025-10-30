// project/ui/AppointmentDialog.cpp
#include "AppointmentDialog.h"
#include "ui_AppointmentDialog.h"

AppointmentDialog::AppointmentDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::AppointmentDialog) {
    ui->setupUi(this);
}

AppointmentDialog::~AppointmentDialog() { delete ui; }
