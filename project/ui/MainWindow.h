// project/ui/MainWindow.h
#pragma once
#include <QMainWindow>
#include <QDir>
#include <memory>

class QTableWidget;
class QPushButton;
class QPlainTextEdit;
class QTimeEdit;
class QDateEdit;
class QComboBox;
class QTabWidget;
class QLineEdit;
class QSpinBox;
class QListWidget;

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

    // yönetici işlemleri
    void onAddSalon();
    void onAddEmployee();
    void onAddService();
    void onAddSkillToEmployee();
    void onAddAvailabilityToEmployee();

    // yeni: kalıcılık
    void onSaveJson();
    void onLoadJson();
    void saveStateToDisk();
    bool loadStateFromDisk();

private:
    void buildUi();
    void refreshSalonCombo();
    void refreshTables();
    void refreshAppointments();
    void refreshAdminCombos();
    void refreshEmployeeCandidates();
    void refreshSkillPool();
    void log(const QString& msg);

    Salon* currentSalon();
    const Salon* currentSalon() const;

    int selectedEmployeeRow() const;
    int selectedServiceRow() const;
    int selectedAppointmentRow() const;

    // yeni: JSON serileştirme yardımcıları
    QByteArray serializeSalonToJson() const;
    bool       deserializeSalonFromJson(const QByteArray& data);

    QString dataDirPath;
    QString dataFilePath;

private:
    Ui::MainWindow* ui{nullptr};

    QWidget*        central{nullptr};
    QTabWidget*     roleTabs{nullptr};

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

    // yönetici paneli
    QLineEdit*      edtSalonName{nullptr};
    QTimeEdit*      edtSalonStart{nullptr};
    QTimeEdit*      edtSalonEnd{nullptr};

    QLineEdit*      edtEmpName{nullptr};
    QLineEdit*      edtEmpPhone{nullptr};
    QComboBox*      cmbSalonForEmployee{nullptr};
    QListWidget*    lstEmpSkills{nullptr};
    QTimeEdit*      edtEmpAvailStart{nullptr};
    QTimeEdit*      edtEmpAvailEnd{nullptr};

    QLineEdit*      edtServiceName{nullptr};
    QSpinBox*       spnServiceDuration{nullptr};
    QSpinBox*       spnServicePrice{nullptr};

    QComboBox*      cmbEmployeeEdit{nullptr};
    QListWidget*    lstEditSkills{nullptr};
    QTimeEdit*      edtNewAvailStart{nullptr};
    QTimeEdit*      edtNewAvailEnd{nullptr};

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
