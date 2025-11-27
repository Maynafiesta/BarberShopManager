// project/ui/MainWindow.h
#pragma once
#include <QMainWindow>
#include <memory>

class QTableWidget;
class QPushButton;
class QPlainTextEdit;
class QTimeEdit;
class QDateEdit;
class QComboBox;

#include "model/Salon.h"
#include "model/Customer.h"
#include "model/Employee.h"
#include "model/Service.h"
#include "core/Scheduler.h"
#include "controllers/AppointmentController.h"
#include "controllers/SalonController.h"

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadDemo();
    void onSalonChanged(int index);
    void onCreateAppointment();
    void onRejectSelected();
    void onDeleteSelected();

    // yeni: kalıcılık
    void onSaveJson();
    void onLoadJson();

private:
    void buildUi();
    void refreshSalonCombo();
    void refreshTables();
    void refreshAppointments();
    void log(const QString& msg);

    Salon* currentSalon();
    const Salon* currentSalon() const;

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
    QComboBox*      cmbSalons{nullptr};
    QPushButton*    btnCreateAppt{nullptr};
    QPushButton*    btnReject{nullptr};
    QPushButton*    btnDelete{nullptr};
    QPushButton*    btnSave{nullptr};      // yeni
    QPushButton*    btnLoad{nullptr};      // yeni

    QPlainTextEdit* txtLog{nullptr};
    QTimeEdit*      timeEdit{nullptr};
    QDateEdit*      dateEdit{nullptr};

    // iş verisi
    std::vector<Salon>     salons;
    std::vector<Customer>  customers; // basit MVP: tek müşteri ile ilerliyoruz (customers.front())
    Scheduler              scheduler;

    SalonController        salonController;
    AppointmentController  appointmentController;
};
