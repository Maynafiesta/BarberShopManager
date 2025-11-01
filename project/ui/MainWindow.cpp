// project/ui/MainWindow.cpp
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QDateTime>
#include <QTimeEdit>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("BarberShopManager");
    buildUi();
    refreshTables();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::buildUi() {
    central = new QWidget(this);
    auto* v = new QVBoxLayout(central);

    // Üst şerit
    auto* top = new QHBoxLayout();
    btnLoadDemo     = new QPushButton("Demo veriyi yükle", central);
    timeEdit        = new QTimeEdit(QTime(10,0), central);
    timeEdit->setDisplayFormat("HH:mm");
    btnCreateAppt   = new QPushButton("Seçili çalışan + hizmet ile randevu", central);

    top->addWidget(btnLoadDemo);
    top->addSpacing(12);
    top->addWidget(new QLabel("Saat:", central));
    top->addWidget(timeEdit);
    top->addSpacing(12);
    top->addWidget(btnCreateAppt);
    top->addStretch(1);

    // Orta tablolar
    auto* mid = new QHBoxLayout();
    tblEmployees = new QTableWidget(0, 2, central);
    tblEmployees->setHorizontalHeaderLabels({ "Çalışan", "Yetenekler" });
    tblEmployees->horizontalHeader()->setStretchLastSection(true);
    tblEmployees->verticalHeader()->setVisible(false);
    tblEmployees->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblEmployees->setSelectionMode(QAbstractItemView::SingleSelection);
    tblEmployees->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tblServices = new QTableWidget(0, 3, central);
    tblServices->setHorizontalHeaderLabels({ "Hizmet", "Süre (dk)", "Ücret" });
    tblServices->horizontalHeader()->setStretchLastSection(true);
    tblServices->verticalHeader()->setVisible(false);
    tblServices->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblServices->setSelectionMode(QAbstractItemView::SingleSelection);
    tblServices->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mid->addWidget(tblEmployees, /*stretch*/1);
    mid->addWidget(tblServices,  /*stretch*/1);

    // Alt log
    txtLog = new QPlainTextEdit(central);
    txtLog->setReadOnly(true);
    txtLog->setMinimumHeight(120);

    v->addLayout(top);
    v->addLayout(mid);
    v->addWidget(txtLog);

    setCentralWidget(central);

    connect(btnLoadDemo,   &QPushButton::clicked, this, &MainWindow::onLoadDemo);
    connect(btnCreateAppt, &QPushButton::clicked, this, &MainWindow::onCreateAppointment);
}

void MainWindow::refreshTables() {
    // Employees
    const auto& emps = salon.getEmployees();
    tblEmployees->setRowCount(static_cast<int>(emps.size()));
    for (int i = 0; i < static_cast<int>(emps.size()); ++i) {
        const auto& e = emps[static_cast<size_t>(i)];
        auto* nameItem = new QTableWidgetItem(QString::fromStdString(e.getName()));
        QString skills;
        for (const auto& s : e.getSkills()) {
            if (!skills.isEmpty()) skills += ", ";
            skills += QString::fromStdString(s);
        }
        auto* skillItem = new QTableWidgetItem(skills);
        tblEmployees->setItem(i, 0, nameItem);
        tblEmployees->setItem(i, 1, skillItem);
    }
    tblEmployees->resizeColumnsToContents();

    // Services
    const auto& svcs = salon.getServices();
    tblServices->setRowCount(static_cast<int>(svcs.size()));
    for (int i = 0; i < static_cast<int>(svcs.size()); ++i) {
        const auto& s = svcs[static_cast<size_t>(i)];
        tblServices->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(s.getName())));
        tblServices->setItem(i, 1, new QTableWidgetItem(QString::number(s.getDurationMinutes())));
        tblServices->setItem(i, 2, new QTableWidgetItem(QString::number(s.getPrice(), 'f', 2)));
    }
    tblServices->resizeColumnsToContents();
}

void MainWindow::log(const QString& msg) {
    txtLog->appendPlainText(QTime::currentTime().toString("HH:mm:ss") + "  " + msg);
}

int MainWindow::selectedEmployeeRow() const {
    auto sel = tblEmployees->selectionModel()->selectedRows();
    return sel.isEmpty() ? -1 : sel.front().row();
}

int MainWindow::selectedServiceRow() const {
    auto sel = tblServices->selectionModel()->selectedRows();
    return sel.isEmpty() ? -1 : sel.front().row();
}

void MainWindow::onLoadDemo() {
    const auto today = QDate::currentDate();
    const auto start = QDateTime(today, QTime(9,0), Qt::LocalTime).toSecsSinceEpoch();
    salon = Salon("Merkez Şube");
    salon.setWorkingHours(TimeSlot{ static_cast<std::time_t>(start), 12*60 }); // 09:00-21:00

    // Çalışanlar ve uygunluk
    Employee e1("Ahmet Usta",  "0500 000 0001");
    e1.addSkill("Saç kesimi");
    e1.addSkill("Sakal tıraşı");
    e1.addAvailability(TimeSlot{ static_cast<std::time_t>(
        QDateTime(today, QTime(10,0), Qt::LocalTime).toSecsSinceEpoch()), 4*60 }); // 10-14

    Employee e2("Merve", "0500 000 0002");
    e2.addSkill("Boya");
    e2.addSkill("Fön");
    e2.addAvailability(TimeSlot{ static_cast<std::time_t>(
        QDateTime(today, QTime(12,0), Qt::LocalTime).toSecsSinceEpoch()), 6*60 }); // 12-18

    salon.addEmployee(e1);
    salon.addEmployee(e2);

    // Hizmetler
    salon.addService(Service("Saç kesimi", 30, 250.0));
    salon.addService(Service("Sakal tıraşı", 20, 150.0));
    salon.addService(Service("Boya", 90, 900.0));
    salon.addService(Service("Fön", 15, 120.0));

    // Bir müşteri
    customers.clear();
    customers.emplace_back("Tarık", "0555 555 55 55");

    refreshTables();
    log("Demo veriler yüklendi. Tablolardan bir çalışan ve bir hizmet seç; saat alanından randevu saati belirle.");
}

void MainWindow::onCreateAppointment() {
    if (salon.getEmployees().empty() || salon.getServices().empty() || customers.empty()) {
        log("Önce demo verilerini yükle.");
        return;
    }

    const int erow = selectedEmployeeRow();
    if (erow < 0) { log("Lütfen bir çalışan seç."); return; }

    const int srow = selectedServiceRow();
    if (srow < 0) { log("Lütfen bir hizmet seç."); return; }

    const auto& employee = salon.getEmployees()[static_cast<size_t>(erow)];
    const auto& service  = salon.getServices()[static_cast<size_t>(srow)];
    const auto& customer = customers.front();

    const auto today = QDate::currentDate();
    const QTime t = timeEdit->time();
    TimeSlot slot { static_cast<std::time_t>(
        QDateTime(today, t, Qt::LocalTime).toSecsSinceEpoch()),
        service.getDurationMinutes()
    };

    Appointment created;
    const auto res = scheduler.createAppointment(salon, customer, employee, service, slot, created);

    switch (res) {
        case Scheduler::CreateResult::Ok:
            log(QString("Randevu onaylandı: %1 - %2 @ %3 (₺%4)")
                .arg(QString::fromStdString(employee.getName()))
                .arg(QString::fromStdString(service.getName()))
                .arg(t.toString("HH:mm"))
                .arg(created.getTotalPrice(), 0, 'f', 2));
            break;
        case Scheduler::CreateResult::OutsideWorkingHours:
            log("Randevu mesai saatleri dışında.");
            break;
        case Scheduler::CreateResult::EmployeeNotAvailable:
            log("Çalışan bu saatte uygun değil (uygunluk dilimleri dışında).");
            break;
        case Scheduler::CreateResult::Collision:
            log("Bu saatte bu çalışan için çakışan randevu var.");
            break;
    }
}
