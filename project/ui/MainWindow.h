// project/ui/MainWindow.h
#pragma once
#include <QMainWindow>
#include <memory>

class QTableWidget;
class QPushButton;
class QPlainTextEdit;
class QTimeEdit;
class QDateEdit;

#include "model/Salon.h"
#include "model/Customer.h"
#include "model/Employee.h"
#include "model/Service.h"
#include "core/Scheduler.h"

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadDemo();
    void onCreateAppointment();
    void onRejectSelected();          // yeni
    void onDeleteSelected();          // yeni

private:
    void buildUi();
    void refreshTables();
    void refreshAppointments();
    void log(const QString& msg);

    int selectedEmployeeRow() const;
    int selectedServiceRow() const;
    int selectedAppointmentRow() const;   // yeni

private:
    Ui::MainWindow* ui{nullptr};

    QWidget*        central{nullptr};
    QTableWidget*   tblEmployees{nullptr};
    QTableWidget*   tblServices{nullptr};
    QTableWidget*   tblAppointments{nullptr};

    QPushButton*    btnLoadDemo{nullptr};
    QPushButton*    btnCreateAppt{nullptr};
    QPushButton*    btnReject{nullptr};   // yeni
    QPushButton*    btnDelete{nullptr};   // yeni

    QPlainTextEdit* txtLog{nullptr};
    QTimeEdit*      timeEdit{nullptr};
    QDateEdit*      dateEdit{nullptr};

    Salon salon{"Merkez Şube"};
    std::vector<Customer> customers;
    Scheduler scheduler;
};
