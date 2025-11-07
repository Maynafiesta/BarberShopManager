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
    void onRejectSelected();
    void onDeleteSelected();

    // yeni: kalıcılık
    void onSaveJson();
    void onLoadJson();

private:
    void buildUi();
    void refreshTables();
    void refreshAppointments();
    void log(const QString& msg);

    int selectedEmployeeRow() const;
    int selectedServiceRow() const;
    int selectedAppointmentRow() const;

    // yeni: JSON serileştirme yardımcıları
    QByteArray serializeSalonToJson() const;
    bool       deserializeSalonFromJson(const QByteArray& data);

private:
    Ui::MainWindow* ui{nullptr};

    QWidget*        central{nullptr};
    QTableWidget*   tblEmployees{nullptr};
    QTableWidget*   tblServices{nullptr};
    QTableWidget*   tblAppointments{nullptr};

    QPushButton*    btnLoadDemo{nullptr};
    QPushButton*    btnCreateAppt{nullptr};
    QPushButton*    btnReject{nullptr};
    QPushButton*    btnDelete{nullptr};
    QPushButton*    btnSave{nullptr};      // yeni
    QPushButton*    btnLoad{nullptr};      // yeni

    QPlainTextEdit* txtLog{nullptr};
    QTimeEdit*      timeEdit{nullptr};
    QDateEdit*      dateEdit{nullptr};

    // iş verisi
    Salon salon{"Merkez Şube"};
    std::vector<Customer> customers; // basit MVP: tek müşteri ile ilerliyoruz (customers.front())
    Scheduler scheduler;
};
