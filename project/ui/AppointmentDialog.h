// project/ui/AppointmentDialog.h
#pragma once
#include <QDialog>

namespace Ui { class AppointmentDialog; }

class AppointmentDialog : public QDialog {
    Q_OBJECT
public:
    explicit AppointmentDialog(QWidget* parent = nullptr);
    ~AppointmentDialog();
private:
    Ui::AppointmentDialog* ui;
};
