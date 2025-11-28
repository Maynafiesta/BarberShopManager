// project/ui/MainWindow.cpp
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QDateTime>
#include <QTimeEdit>
#include <QDateEdit>
#include <QMessageBox>
#include <QLabel>
#include <QComboBox>
#include <QTabWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QStringList>
#include <QListWidget>
#include <QItemSelectionModel>

// JSON / Dosya
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHash>
#include <algorithm>
#include <set>
#include <QCoreApplication>

namespace {
int safeToInt(const QVariant& var, bool& ok) {
    const int val = var.toInt(&ok);
    return ok ? val : -1;
}

int safeToInt(const QJsonValue& value, bool& ok) {
    const int val = value.toInt(&ok);
    return ok ? val : -1;
}
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , salonController(salons, customers)
    , appointmentController(scheduler) {
    dataDirPath  = QDir(QCoreApplication::applicationDirPath()).filePath("data");
    dataFilePath = QDir(dataDirPath).filePath("state.json");
    QDir().mkpath(dataDirPath);

    ui->setupUi(this);
    setWindowTitle("BarberShopManager");
    buildUi();
    appointmentController.setActiveSalon(currentSalon());

    if (!loadStateFromDisk()) {
        refreshTables();
        refreshAppointments();
    }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::buildUi() {
    central  = new QWidget(this);
    auto* v0 = new QVBoxLayout(central);

    roleTabs = new QTabWidget(central);
    auto* customerTab = new QWidget(roleTabs);
    auto* adminTab    = new QWidget(roleTabs);
    roleTabs->addTab(customerTab, "Müşteri");
    roleTabs->addTab(adminTab,    "Yönetici");

    // --- Müşteri paneli ---
    auto* v = new QVBoxLayout(customerTab);

    // Üst şerit (demo + tarih/saat + oluştur)
    auto* top = new QHBoxLayout();
    btnLoadDemo   = new QPushButton("Demo veriyi yükle", customerTab);
    cmbSalons     = new QComboBox(customerTab);
    cmbSalons->setPlaceholderText("Salon seç");
    cmbSalons->setEnabled(false);

    dateEdit = new QDateEdit(QDate::currentDate(), customerTab);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");

    timeEdit = new QTimeEdit(QTime(10,0), customerTab);
    timeEdit->setDisplayFormat("HH:mm");

    btnCreateAppt = new QPushButton("Seçili çalışan + hizmet ile randevu", customerTab);

    top->addWidget(btnLoadDemo);
    top->addSpacing(12);
    top->addWidget(new QLabel("Salon:", customerTab));
    top->addWidget(cmbSalons);
    top->addSpacing(12);
    top->addWidget(new QLabel("Tarih:", customerTab));
    top->addWidget(dateEdit);
    top->addSpacing(8);
    top->addWidget(new QLabel("Saat:", customerTab));
    top->addWidget(timeEdit);
    top->addSpacing(12);
    top->addWidget(btnCreateAppt);
    top->addStretch(1);

    // Orta tablolar (çalışanlar ve hizmetler)
    auto* mid = new QHBoxLayout();
    tblEmployees = new QTableWidget(0, 3, customerTab);
    tblEmployees->setHorizontalHeaderLabels({ "Çalışan", "Ücret", "Yetenekler" });
    tblEmployees->horizontalHeader()->setStretchLastSection(true);
    tblEmployees->verticalHeader()->setVisible(false);
    tblEmployees->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblEmployees->setSelectionMode(QAbstractItemView::SingleSelection);
    tblEmployees->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tblServices = new QTableWidget(0, 3, customerTab);
    tblServices->setHorizontalHeaderLabels({ "Hizmet", "Süre (dk)", "Ücret" });
    tblServices->horizontalHeader()->setStretchLastSection(true);
    tblServices->verticalHeader()->setVisible(false);
    tblServices->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblServices->setSelectionMode(QAbstractItemView::SingleSelection);
    tblServices->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mid->addWidget(tblEmployees, 1);
    mid->addWidget(tblServices,  1);

    // Randevular tablosu
    tblAppointments = new QTableWidget(0, 5, customerTab);
    tblAppointments->setHorizontalHeaderLabels({ "Tarih/Saat", "Çalışan", "Hizmet", "Durum", "Ücret" });
    tblAppointments->horizontalHeader()->setStretchLastSection(true);
    tblAppointments->verticalHeader()->setVisible(false);
    tblAppointments->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblAppointments->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblAppointments->setSelectionMode(QAbstractItemView::SingleSelection);

    // Randevu aksiyonları
    auto* actions = new QHBoxLayout();
    btnReject = new QPushButton("Seçileni Reddet", customerTab);
    btnDelete = new QPushButton("Seçileni Sil", customerTab);
    actions->addWidget(btnReject);
    actions->addWidget(btnDelete);
    actions->addStretch(1);

    // Kalıcılık butonları (kaydet/yükle)
    auto* persist = new QHBoxLayout();
    btnSave = new QPushButton("JSON’a Kaydet", customerTab);
    btnLoad = new QPushButton("JSON’dan Yükle", customerTab);
    persist->addWidget(btnSave);
    persist->addWidget(btnLoad);
    persist->addStretch(1);

    v->addLayout(top);
    v->addLayout(mid);
    v->addWidget(new QLabel("Randevular:", customerTab));
    v->addWidget(tblAppointments);
    v->addLayout(actions);
    v->addLayout(persist);

    // --- Yönetici paneli ---
    auto* adminLayout = new QVBoxLayout(adminTab);

    auto* gbSalon = new QGroupBox("Salon ekle / mesai", adminTab);
    auto* fSalon  = new QFormLayout(gbSalon);
    edtSalonName     = new QLineEdit(gbSalon);
    edtSalonStart    = new QTimeEdit(QTime(9, 0), gbSalon);
    edtSalonStart->setDisplayFormat("HH:mm");
    edtSalonEnd      = new QTimeEdit(QTime(21, 0), gbSalon);
    edtSalonEnd->setDisplayFormat("HH:mm");
    auto* btnAddSalon = new QPushButton("Salon Ekle", gbSalon);
    fSalon->addRow("Salon adı", edtSalonName);
    fSalon->addRow("Başlangıç (günlük)", edtSalonStart);
    fSalon->addRow("Kapanış", edtSalonEnd);
    fSalon->addRow(btnAddSalon);

    auto* gbEmp = new QGroupBox("Çalışan ekle", adminTab);
    auto* fEmp  = new QFormLayout(gbEmp);
    edtEmpName  = new QLineEdit(gbEmp);
    edtEmpPhone = new QLineEdit(gbEmp);
    cmbSalonForEmployee = new QComboBox(gbEmp);
    cmbSalonForEmployee->setPlaceholderText("Salon seç");
    cmbSalonForEmployee->setEnabled(false);
    lstEmpSkills = new QListWidget(gbEmp);
    lstEmpSkills->setSelectionMode(QAbstractItemView::NoSelection);
    edtEmpAvailStart = new QTimeEdit(QTime(10, 0), gbEmp);
    edtEmpAvailStart->setDisplayFormat("HH:mm");
    edtEmpAvailEnd   = new QTimeEdit(QTime(18, 0), gbEmp);
    edtEmpAvailEnd->setDisplayFormat("HH:mm");
    auto* btnAddEmp = new QPushButton("Çalışan Ekle", gbEmp);
    fEmp->addRow("Ad Soyad", edtEmpName);
    fEmp->addRow("Telefon", edtEmpPhone);
    fEmp->addRow("Salon", cmbSalonForEmployee);
    fEmp->addRow("Yetenek havuzu", lstEmpSkills);
    fEmp->addRow("Uygunluk başlangıç", edtEmpAvailStart);
    fEmp->addRow("Uygunluk bitiş", edtEmpAvailEnd);
    fEmp->addRow(btnAddEmp);

    auto* gbSvc = new QGroupBox("Hizmet ekle", adminTab);
    auto* fSvc  = new QFormLayout(gbSvc);
    edtServiceName      = new QLineEdit(gbSvc);
    spnServiceDuration  = new QSpinBox(gbSvc);
    spnServiceDuration->setRange(5, 8 * 60);
    spnServiceDuration->setValue(30);
    spnServicePrice     = new QSpinBox(gbSvc);
    spnServicePrice->setRange(0, 100000);
    spnServicePrice->setSuffix(" ₺");
    spnServicePrice->setValue(250);
    auto* btnAddSvc = new QPushButton("Hizmet Ekle", gbSvc);
    fSvc->addRow("Ad", edtServiceName);
    fSvc->addRow("Süre (dk)", spnServiceDuration);
    fSvc->addRow("Ücret", spnServicePrice);
    fSvc->addRow(btnAddSvc);

    auto* gbEdit = new QGroupBox("Personel düzenle (aktif salon)", adminTab);
    auto* fEdit  = new QFormLayout(gbEdit);
    cmbEmployeeEdit   = new QComboBox(gbEdit);
    cmbEmployeeEdit->setPlaceholderText("Salon + çalışan seç");
    lstEditSkills    = new QListWidget(gbEdit);
    lstEditSkills->setSelectionMode(QAbstractItemView::NoSelection);
    edtNewAvailStart = new QTimeEdit(QTime(12,0), gbEdit);
    edtNewAvailStart->setDisplayFormat("HH:mm");
    edtNewAvailEnd   = new QTimeEdit(QTime(18,0), gbEdit);
    edtNewAvailEnd->setDisplayFormat("HH:mm");
    auto* btnAddSkill = new QPushButton("Yetenekleri Güncelle", gbEdit);
    auto* btnAddAvail = new QPushButton("Yeni Uygunluk Ekle", gbEdit);
    fEdit->addRow("Çalışan", cmbEmployeeEdit);
    fEdit->addRow("Yetenekler", lstEditSkills);
    fEdit->addRow(btnAddSkill);
    fEdit->addRow("Uygunluk başlangıç", edtNewAvailStart);
    fEdit->addRow("Uygunluk bitiş", edtNewAvailEnd);
    fEdit->addRow(btnAddAvail);

    adminLayout->addWidget(gbSalon);
    adminLayout->addWidget(gbEmp);
    adminLayout->addWidget(gbSvc);
    adminLayout->addWidget(gbEdit);
    adminLayout->addStretch(1);

    // Alt log
    txtLog = new QPlainTextEdit(central);
    txtLog->setReadOnly(true);
    txtLog->setMinimumHeight(140);

    v0->addWidget(roleTabs);
    v0->addWidget(txtLog);

    setCentralWidget(central);

    connect(btnLoadDemo,   &QPushButton::clicked, this, &MainWindow::onLoadDemo);
    connect(cmbSalons, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSalonChanged);
    connect(btnCreateAppt, &QPushButton::clicked, this, &MainWindow::onCreateAppointment);
    connect(btnReject,     &QPushButton::clicked, this, &MainWindow::onRejectSelected);
    connect(btnDelete,     &QPushButton::clicked, this, &MainWindow::onDeleteSelected);
    connect(tblServices->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::refreshEmployeeCandidates);
    connect(timeEdit, &QTimeEdit::timeChanged, this, &MainWindow::refreshEmployeeCandidates);
    connect(dateEdit, &QDateEdit::dateChanged, this, &MainWindow::refreshEmployeeCandidates);

    connect(btnSave,       &QPushButton::clicked, this, &MainWindow::onSaveJson);
    connect(btnLoad,       &QPushButton::clicked, this, &MainWindow::onLoadJson);

    // admin
    connect(btnAddSalon, &QPushButton::clicked, this, &MainWindow::onAddSalon);
    connect(btnAddEmp,   &QPushButton::clicked, this, &MainWindow::onAddEmployee);
    connect(btnAddSvc,   &QPushButton::clicked, this, &MainWindow::onAddService);
    connect(btnAddSkill, &QPushButton::clicked, this, &MainWindow::onAddSkillToEmployee);
    connect(btnAddAvail, &QPushButton::clicked, this, &MainWindow::onAddAvailabilityToEmployee);
    connect(cmbSalonForEmployee, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::refreshSkillPool);
    connect(cmbEmployeeEdit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::refreshSkillPool);
}

void MainWindow::refreshTables() {
    const auto* salon = currentSalon();

    // Services
    const auto& svcs = salonController.services();
    tblServices->setRowCount(static_cast<int>(svcs.size()));
    for (int i = 0; i < static_cast<int>(svcs.size()); ++i) {
        const auto& s = svcs[static_cast<size_t>(i)];
        tblServices->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(s.getName())));
        tblServices->setItem(i, 1, new QTableWidgetItem(QString::number(s.getDurationMinutes())));
        tblServices->setItem(i, 2, new QTableWidgetItem(QString::number(s.getPrice(), 'f', 2)));
    }
    tblServices->resizeColumnsToContents();

    refreshEmployeeCandidates();

    if (!salon) {
        tblAppointments->setRowCount(0);
    }

    refreshAdminCombos();
}

void MainWindow::refreshAppointments() {
    const auto& apps = salonController.appointments();
    tblAppointments->setRowCount(static_cast<int>(apps.size()));
    for (int i = 0; i < static_cast<int>(apps.size()); ++i) {
        const auto& a = apps[static_cast<size_t>(i)];
        const auto start = QDateTime::fromSecsSinceEpoch(a.getSlot().startEpoch);
        QString when = start.toString("dd.MM.yyyy HH:mm");
        QString who  = a.getEmployee() ? QString::fromStdString(a.getEmployee()->getName()) : "-";
        QString what = QString::fromStdString(a.getService().getName());
        QString st;
        switch (a.getStatus()) {
            case Appointment::Status::Pending:  st = "Bekliyor"; break;
            case Appointment::Status::Approved: st = "Onaylı";   break;
            case Appointment::Status::Rejected: st = "Reddedildi"; break;
        }
        QString price = QString::number(a.getTotalPrice(), 'f', 2);

        tblAppointments->setItem(i, 0, new QTableWidgetItem(when));
        tblAppointments->setItem(i, 1, new QTableWidgetItem(who));
        tblAppointments->setItem(i, 2, new QTableWidgetItem(what));
        tblAppointments->setItem(i, 3, new QTableWidgetItem(st));
        tblAppointments->setItem(i, 4, new QTableWidgetItem(price));
    }
    tblAppointments->resizeColumnsToContents();
}

void MainWindow::refreshAdminCombos() {
    const auto& list = salonController.salons();

    if (cmbSalonForEmployee) {
        cmbSalonForEmployee->blockSignals(true);
        cmbSalonForEmployee->clear();
        for (const auto& s : list)
            cmbSalonForEmployee->addItem(QString::fromStdString(s.getName()));
        cmbSalonForEmployee->setEnabled(!list.empty());
        if (!list.empty())
            cmbSalonForEmployee->setCurrentIndex(static_cast<int>(salonController.activeSalonIndex()));
        cmbSalonForEmployee->blockSignals(false);
    }

    if (cmbEmployeeEdit) {
        cmbEmployeeEdit->blockSignals(true);
        cmbEmployeeEdit->clear();

        const auto* salon = currentSalon();
        if (!salon) {
            cmbEmployeeEdit->setEnabled(false);
        } else {
            const auto& emps = salonController.employees();
            for (const auto& e : emps)
                cmbEmployeeEdit->addItem(QString::fromStdString(e.getName()));

            cmbEmployeeEdit->setEnabled(!emps.empty());
        }
        cmbEmployeeEdit->blockSignals(false);
    }

    refreshSkillPool();
}

void MainWindow::refreshSkillPool() {
    auto gatherSkills = [&](const Salon& salon) {
        std::set<std::string> out;
        for (const auto& s : salon.getServices())
            out.insert(s.getName());
        return out;
    };

    const auto& allSalons = salonController.salons();
    const int targetSalonIdx = cmbSalonForEmployee ? cmbSalonForEmployee->currentIndex() : -1;
    const size_t idx = (targetSalonIdx >= 0 && static_cast<size_t>(targetSalonIdx) < allSalons.size())
        ? static_cast<size_t>(targetSalonIdx)
        : salonController.activeSalonIndex();

    std::set<std::string> addSkills;
    if (idx < allSalons.size())
        addSkills = gatherSkills(allSalons[idx]);

    if (lstEmpSkills) {
        lstEmpSkills->clear();
        for (const auto& s : addSkills) {
            auto* item = new QListWidgetItem(QString::fromStdString(s), lstEmpSkills);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);
        }
        lstEmpSkills->setEnabled(!addSkills.empty());
    }

    std::set<std::string> editSkills;
    if (const auto* salon = currentSalon())
        editSkills = gatherSkills(*salon);

    if (lstEditSkills) {
        lstEditSkills->clear();
        const auto& emps = salonController.employees();
        const int empIdx = cmbEmployeeEdit ? cmbEmployeeEdit->currentIndex() : -1;
        std::set<std::string> owned;
        if (empIdx >= 0 && empIdx < static_cast<int>(emps.size())) {
            for (const auto& sk : emps[static_cast<size_t>(empIdx)].getSkills())
                owned.insert(sk);
        }

        for (const auto& s : editSkills) {
            auto* item = new QListWidgetItem(QString::fromStdString(s), lstEditSkills);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(owned.count(s) ? Qt::Checked : Qt::Unchecked);
        }
        lstEditSkills->setEnabled(!editSkills.empty() && empIdx >= 0);
    }
}

void MainWindow::refreshEmployeeCandidates() {
    tblEmployees->setRowCount(0);
    const auto* salon = currentSalon();
    if (!salon) return;

    const auto& svcs = salonController.services();
    const int svcRow = selectedServiceRow();
    if (svcRow < 0 || svcRow >= static_cast<int>(svcs.size())) return;

    const auto& service = svcs[static_cast<size_t>(svcRow)];
    const QDateTime startDt(dateEdit->date(), timeEdit->time(), Qt::LocalTime);
    TimeSlot desired { static_cast<std::time_t>(startDt.toSecsSinceEpoch()), service.getDurationMinutes() };

    if (!salon->getWorkingHours().contains(desired)) return;

    const auto& emps = salonController.employees();
    const auto& apps = salonController.appointments();
    int row = 0;
    for (size_t idx = 0; idx < emps.size(); ++idx) {
        const auto& e = emps[idx];
        const auto& skills = e.getSkills();
        if (std::find(skills.begin(), skills.end(), service.getName()) == skills.end())
            continue;

        bool avail = false;
        for (const auto& s : e.getAvailability()) {
            if (s.contains(desired)) { avail = true; break; }
        }
        if (!avail) continue;

        bool collision = false;
        for (const auto& a : apps) {
            if (a.getEmployee() == &e && a.getSlot().overlaps(desired)) { collision = true; break; }
        }
        if (collision) continue;

        tblEmployees->insertRow(row);
        auto* nameItem = new QTableWidgetItem(QString::fromStdString(e.getName()));
        nameItem->setData(Qt::UserRole, static_cast<int>(idx));
        tblEmployees->setItem(row, 0, nameItem);
        tblEmployees->setItem(row, 1, new QTableWidgetItem(QString::number(service.getPrice(), 'f', 2)));
        QString skillStr;
        for (const auto& s : skills) {
            if (!skillStr.isEmpty()) skillStr += ", ";
            skillStr += QString::fromStdString(s);
        }
        tblEmployees->setItem(row, 2, new QTableWidgetItem(skillStr));
        ++row;
    }
    tblEmployees->resizeColumnsToContents();
}

void MainWindow::log(const QString& msg) {
    txtLog->appendPlainText(QTime::currentTime().toString("HH:mm:ss") + "  " + msg);
}

void MainWindow::refreshSalonCombo() {
    cmbSalons->blockSignals(true);
    cmbSalons->clear();

    const auto& list = salonController.salons();
    for (const auto& s : list) {
        cmbSalons->addItem(QString::fromStdString(s.getName()));
    }

    cmbSalons->setEnabled(!list.empty());
    if (!list.empty()) {
        const int idx = static_cast<int>(std::min(salonController.activeSalonIndex(), list.size() - 1));
        cmbSalons->setCurrentIndex(idx);
    }
    cmbSalons->blockSignals(false);

    refreshAdminCombos();
}

Salon* MainWindow::currentSalon() {
    return salonController.currentSalon();
}

const Salon* MainWindow::currentSalon() const {
    return salonController.currentSalon();
}

int MainWindow::selectedEmployeeRow() const {
    auto sel = tblEmployees->selectionModel()->selectedRows();
    return sel.isEmpty() ? -1 : sel.front().row();
}

int MainWindow::selectedServiceRow() const {
    auto sel = tblServices->selectionModel()->selectedRows();
    return sel.isEmpty() ? -1 : sel.front().row();
}

int MainWindow::selectedAppointmentRow() const {
    auto sel = tblAppointments->selectionModel()->selectedRows();
    return sel.isEmpty() ? -1 : sel.front().row();
}

void MainWindow::onLoadDemo() {
    const auto today = QDate::currentDate();

    salonController.loadDemoData(today);

    refreshSalonCombo();
    appointmentController.setActiveSalon(currentSalon());
    refreshTables();
    refreshAppointments();
    saveStateToDisk();
    log("Demo yüklendi. Salon seç, Tarih/Saat seç, bir çalışan + bir hizmet seç ve randevu oluştur.");
}

void MainWindow::onSalonChanged(int index) {
    if (index < 0) return;
    if (!salonController.setActiveSalon(static_cast<size_t>(index))) return;

    appointmentController.setActiveSalon(currentSalon());
    refreshTables();
    refreshAppointments();
    refreshEmployeeCandidates();
    refreshSkillPool();

    const auto& list = salonController.salons();
    if (static_cast<size_t>(index) < list.size())
        log(QString("Aktif salon: %1").arg(QString::fromStdString(list[static_cast<size_t>(index)].getName())));
}

void MainWindow::onCreateAppointment() {
    const auto* salon = currentSalon();
    if (!salon || salonController.employees().empty() || salonController.services().empty() || customers.empty()) {
        log("Önce demo verilerini yükle.");
        return;
    }

    const int erow = selectedEmployeeRow();
    if (erow < 0) { log("Lütfen bir çalışan seç."); return; }

    const int srow = selectedServiceRow();
    if (srow < 0) { log("Lütfen bir hizmet seç."); return; }

    const auto* nameItem = tblEmployees->item(erow, 0);
    const int empIdx      = [&]() {
        if (!nameItem) return -1;

        bool ok = false;
        return safeToInt(nameItem->data(Qt::UserRole), ok);
    }();
    if (empIdx < 0 || empIdx >= static_cast<int>(salonController.employees().size())) {
        log("Seçili çalışan bulunamadı.");
        return;
    }

    const auto& employee = salonController.employees()[static_cast<size_t>(empIdx)];
    const auto& service  = salonController.services()[static_cast<size_t>(srow)];
    const auto& customer = customers.front();

    const QDate d = dateEdit->date();
    const QTime t = timeEdit->time();
    TimeSlot slot { static_cast<std::time_t>(
        QDateTime(d, t, Qt::LocalTime).toSecsSinceEpoch()),
        service.getDurationMinutes()
    };

    Appointment created;
    const auto res = appointmentController.create(customer, employee, service, slot, created);

    switch (res) {
        case Scheduler::CreateResult::Ok:
            log(QString("Randevu onaylandı: %1 - %2 @ %3 (%4) ₺%5")
                .arg(QString::fromStdString(employee.getName()))
                .arg(QString::fromStdString(service.getName()))
                .arg(QDateTime::fromSecsSinceEpoch(created.getSlot().startEpoch).toString("dd.MM.yyyy HH:mm"))
                .arg(service.getDurationMinutes())
                .arg(created.getTotalPrice(), 0, 'f', 2));
            refreshAppointments();
            saveStateToDisk();
            break;
        case Scheduler::CreateResult::OutsideWorkingHours:
            log("Randevu mesai saatleri dışında.");
            break;
        case Scheduler::CreateResult::EmployeeNotAvailable:
            log("Çalışan bu saatte uygun değil (uygunluk dilimleri dışında).");
            break;
        case Scheduler::CreateResult::EmployeeLacksSkill:
            log("Çalışan bu hizmeti yapamıyor (yetenek listesinde yok).");
            break;
        case Scheduler::CreateResult::Collision:
            log("Bu saatte bu çalışan için çakışan randevu var.");
            break;
    }
}

void MainWindow::onRejectSelected() {
    const int row = selectedAppointmentRow();
    if (row < 0) { log("Önce randevu seç."); return; }
    if (salonController.rejectAppointmentAt(static_cast<size_t>(row))) {
        log("Randevu reddedildi.");
        refreshAppointments();
        saveStateToDisk();
    } else {
        log("Randevu reddedilemedi (geçersiz indeks).");
    }
}

void MainWindow::onDeleteSelected() {
    const int row = selectedAppointmentRow();
    if (row < 0) { log("Önce randevu seç."); return; }

    const auto ret = QMessageBox::question(this, "Sil",
        "Seçili randevuyu kalıcı olarak silmek istiyor musun?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    if (salonController.removeAppointmentAt(static_cast<size_t>(row))) {
        log("Randevu silindi.");
        refreshAppointments();
        saveStateToDisk();
    } else {
        log("Randevu silinemedi (geçersiz indeks).");
    }
}

void MainWindow::onAddSalon() {
    const QString name = edtSalonName->text().trimmed();
    if (name.isEmpty()) {
        log("Salon adı gir.");
        return;
    }

    const QDate d = dateEdit->date();
    const QTime start = edtSalonStart->time();
    const QTime end   = edtSalonEnd->time();
    if (end <= start) {
        log("Kapanış saati başlangıçtan sonra olmalı.");
        return;
    }
    const int durationMinutes = static_cast<int>(start.secsTo(end) / 60);
    TimeSlot wh { static_cast<std::time_t>(QDateTime(d, start, Qt::LocalTime).toSecsSinceEpoch()),
                  durationMinutes };

    if (customers.empty())
        customers.emplace_back("Müşteri", "05xx xxx xx xx");

    if (!salonController.addSalon(name.toStdString(), wh)) {
        log("Aynı isimde salon zaten var.");
        return;
    }
    refreshSalonCombo();
    appointmentController.setActiveSalon(currentSalon());
    refreshTables();
    refreshAppointments();
    refreshSkillPool();
    saveStateToDisk();

    log(QString("Salon eklendi ve aktif: %1").arg(name));
}

void MainWindow::onAddEmployee() {
    const QString name  = edtEmpName->text().trimmed();
    const QString phone = edtEmpPhone->text().trimmed();
    if (name.isEmpty()) { log("Çalışan adı gir."); return; }

    const int salonIdx = cmbSalonForEmployee ? cmbSalonForEmployee->currentIndex() : -1;
    if (salonIdx < 0 || salonIdx >= static_cast<int>(salonController.salons().size())) {
        log("Önce salon ekle veya demo yükle.");
        return;
    }

    Employee e(name.toStdString(), phone.toStdString());
    if (lstEmpSkills) {
        for (int i = 0; i < lstEmpSkills->count(); ++i) {
            auto* item = lstEmpSkills->item(i);
            if (item->checkState() == Qt::Checked)
                e.addSkill(item->text().toStdString());
        }
    }

    if (e.getSkills().empty()) {
        log("En az bir yetenek seç.");
        return;
    }

    const QDate d = dateEdit->date();
    const QTime start = edtEmpAvailStart->time();
    const QTime end   = edtEmpAvailEnd->time();
    if (end <= start) { log("Uygunluk bitişi başlangıçtan sonra olmalı."); return; }
    const int duration = static_cast<int>(start.secsTo(end) / 60);
    TimeSlot avail { static_cast<std::time_t>(QDateTime(d, start, Qt::LocalTime).toSecsSinceEpoch()),
                     duration };
    e.addAvailability(avail);

    const bool targetIsActive = salonController.activeSalonIndex() == static_cast<size_t>(salonIdx);
    if (salonController.addEmployeeToSalon(static_cast<size_t>(salonIdx), e)) {
        if (!targetIsActive) salonController.setActiveSalon(static_cast<size_t>(salonIdx));
        log(QString("Çalışan eklendi: %1").arg(name));
        refreshTables();
        refreshAppointments();
        refreshSkillPool();
        saveStateToDisk();
    } else {
        log("Çalışan eklenemedi (aktif salon yok).");
    }
}

void MainWindow::onAddService() {
    if (!currentSalon()) { log("Önce salon ekle veya demo yükle."); return; }

    const QString name = edtServiceName->text().trimmed();
    if (name.isEmpty()) { log("Hizmet adı gir."); return; }

    Service s(name.toStdString(), spnServiceDuration->value(), spnServicePrice->value());
    if (salonController.addServiceToActive(s)) {
        log(QString("Hizmet eklendi: %1 (%2 dk)").arg(name).arg(spnServiceDuration->value()));
        refreshTables();
        refreshSkillPool();
        saveStateToDisk();
        refreshEmployeeCandidates();
    } else {
        log("Hizmet eklenemedi (aktif salon yok).");
    }
}

void MainWindow::onAddSkillToEmployee() {
    if (!currentSalon()) { log("Önce salon ekle veya demo yükle."); return; }
    const int idx = cmbEmployeeEdit->currentIndex();
    if (idx < 0) { log("Çalışan seç."); return; }

    std::vector<std::string> desired;
    if (lstEditSkills) {
        for (int i = 0; i < lstEditSkills->count(); ++i) {
            auto* item = lstEditSkills->item(i);
            if (item->checkState() == Qt::Checked)
                desired.push_back(item->text().toStdString());
        }
    }

    if (desired.empty()) { log("En az bir yetenek seç."); return; }

    if (salonController.setEmployeeSkills(static_cast<size_t>(idx), desired)) {
        log(QString("Yetenekler güncellendi: %1").arg(cmbEmployeeEdit->currentText()));
        refreshTables();
        saveStateToDisk();
    } else {
        log("Yetenek güncellenemedi.");
    }
}

void MainWindow::onAddAvailabilityToEmployee() {
    if (!currentSalon()) { log("Önce salon ekle veya demo yükle."); return; }
    const int idx = cmbEmployeeEdit->currentIndex();
    if (idx < 0) { log("Çalışan seç."); return; }

    const QDate d = dateEdit->date();
    const QTime start = edtNewAvailStart->time();
    const QTime end   = edtNewAvailEnd->time();
    if (end <= start) { log("Uygunluk bitişi başlangıçtan sonra olmalı."); return; }
    const int duration = static_cast<int>(start.secsTo(end) / 60);
    TimeSlot slot { static_cast<std::time_t>(QDateTime(d, start, Qt::LocalTime).toSecsSinceEpoch()),
                    duration };

    if (salonController.addAvailabilityToEmployee(static_cast<size_t>(idx), slot)) {
        log(QString("Uygunluk eklendi: %1 -> %2").arg(QDateTime(d, start, Qt::LocalTime).toString("dd.MM.yyyy HH:mm"))
                .arg(cmbEmployeeEdit->currentText()));
        refreshTables();
        saveStateToDisk();
    } else {
        log("Uygunluk eklenemedi.");
    }
}

/* ===================== JSON Kalıcılık ===================== */

void MainWindow::saveStateToDisk() {
    if (dataFilePath.isEmpty()) return;

    QDir().mkpath(dataDirPath);
    QFile f(dataFilePath);
    if (!f.open(QIODevice::WriteOnly)) {
        log("Otomatik kayıt açılamadı: " + dataFilePath);
        return;
    }

    f.write(serializeSalonToJson());
    f.close();
    log("Değişiklikler kaydedildi: " + dataFilePath);
}

bool MainWindow::loadStateFromDisk() {
    QFile f(dataFilePath);
    if (!f.exists()) return false;

    if (!f.open(QIODevice::ReadOnly)) {
        log("Kayıt dosyası okunamadı: " + dataFilePath);
        return false;
    }

    const QByteArray data = f.readAll();
    f.close();

    if (!deserializeSalonFromJson(data)) {
        log("Kayıt dosyası parse edilemedi, demo veya elle ekleme yapabilirsin.");
        return false;
    }

    refreshSalonCombo();
    refreshTables();
    refreshAppointments();
    refreshEmployeeCandidates();
    refreshSkillPool();

    log("Diskteki kayıt yüklendi: " + dataFilePath);
    return true;
}

void MainWindow::onSaveJson() {
    saveStateToDisk();

    const QString path = QFileDialog::getSaveFileName(this, "JSON'a Kaydet", dataFilePath, "JSON (*.json)");
    if (path.isEmpty() || path == dataFilePath) return;

    QFile::remove(path); // overwriting için

    QFile::copy(dataFilePath, path);
    log("Dışa aktarıldı: " + path);
}

void MainWindow::onLoadJson() {
    const QString path = QFileDialog::getOpenFileName(this, "JSON'dan Yükle", dataDirPath, "JSON (*.json)");
    if (path.isEmpty()) return;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        log("Dosya açılamadı: " + path);
        return;
    }
    const QByteArray data = f.readAll();
    f.close();

    if (deserializeSalonFromJson(data)) {
        refreshSalonCombo();
        refreshTables();
        refreshAppointments();
        refreshEmployeeCandidates();
        saveStateToDisk();
        log("Yüklendi: " + path);
    } else {
        log("JSON yüklenemedi: " + path);
    }
}

QByteArray MainWindow::serializeSalonToJson() const {
    QJsonArray allSalons;
    for (const auto& salon : salonController.salons()) {
        // employees
        QJsonArray employees;
        const auto& emps = salon.getEmployees();
        for (const auto& e : emps) {
            QJsonObject obj;
            obj["name"]  = QString::fromStdString(e.getName());
            obj["phone"] = QString::fromStdString(e.getPhone());

            QJsonArray skills;
            for (const auto& s : e.getSkills())
                skills.push_back(QString::fromStdString(s));
            obj["skills"] = skills;

            QJsonArray avail;
            for (const auto& t : e.getAvailability()) {
                QJsonObject to;
                to["startEpoch"] = static_cast<qint64>(t.startEpoch);
                to["endEpoch"]   = static_cast<qint64>(t.endEpoch());
                avail.push_back(to);
            }
            obj["availability"] = avail;

            employees.push_back(obj);
        }

        // services
        QJsonArray services;
        const auto& svcs = salon.getServices();
        for (const auto& s : svcs) {
            QJsonObject so;
            so["name"]     = QString::fromStdString(s.getName());
            so["duration"] = s.getDurationMinutes();
            so["price"]    = s.getPrice();
            services.push_back(so);
        }

        // appointments (employee/service index ile referanslayacağız)
        QHash<const Employee*, int> empIndex;
        for (int i = 0; i < employees.size(); ++i)
            empIndex.insert(&emps[static_cast<size_t>(i)], i);

        QJsonArray apps;
        const auto& aList = salon.getAppointments();
        for (const auto& a : aList) {
            QJsonObject ao;
            ao["startEpoch"] = static_cast<qint64>(a.getSlot().startEpoch);
            ao["endEpoch"]   = static_cast<qint64>(a.getSlot().endEpoch());

            int ei = -1;
            if (a.getEmployee()) {
                auto it = empIndex.find(a.getEmployee());
                if (it != empIndex.end()) ei = it.value();
            }
            ao["employeeIndex"] = ei;

            int si = -1;
            for (int i = 0; i < services.size(); ++i) {
                if (services[i].toObject().value("name").toString() ==
                    QString::fromStdString(a.getService().getName())) { si = i; break; }
            }
            ao["serviceIndex"] = si;

            QString st = "Pending";
            switch (a.getStatus()) {
                case Appointment::Status::Pending:  st = "Pending"; break;
                case Appointment::Status::Approved: st = "Approved"; break;
                case Appointment::Status::Rejected: st = "Rejected"; break;
            }
            ao["status"] = st;
            ao["price"]  = a.getTotalPrice();

            apps.push_back(ao);
        }

        QJsonObject working;
        working["startEpoch"] = static_cast<qint64>(salon.getWorkingHours().startEpoch);
        working["endEpoch"]   = static_cast<qint64>(salon.getWorkingHours().endEpoch());

        QJsonObject sroot;
        sroot["name"]         = QString::fromStdString(salon.getName());
        sroot["workingHours"] = working;
        sroot["employees"]    = employees;
        sroot["services"]     = services;
        sroot["appointments"] = apps;
        allSalons.push_back(sroot);
    }

    // customers (MVP: tek müşteri)
    QJsonArray custs;
    if (!customers.empty()) {
        QJsonObject c;
        c["name"]  = QString::fromStdString(customers.front().getName());
        c["phone"] = QString::fromStdString(customers.front().getPhone());
        custs.push_back(c);
    }

    QJsonObject root;
    root["salons"]      = allSalons;
    root["activeSalon"] = static_cast<int>(salonController.activeSalonIndex());
    root["customers"]   = custs;

    return QJsonDocument(root).toJson(QJsonDocument::Indented);
}

bool MainWindow::deserializeSalonFromJson(const QByteArray& data) {
    QJsonParseError err{};
    const auto doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return false;

    const auto root = doc.object();

    salons.clear();

    const auto salonArray = root.value("salons").toArray();
    if (salonArray.isEmpty()) return false;

    // customer (MVP: en az 1 müşteri bulunsun)
    customers.clear();
    const auto custs = root.value("customers").toArray();
    if (!custs.isEmpty()) {
        const auto c = custs.first().toObject();
        customers.emplace_back(c.value("name").toString("Tarık").toStdString(),
                               c.value("phone").toString("0555 555 55 55").toStdString());
    } else {
        customers.emplace_back("Tarık", "0555 555 55 55");
    }

    for (const auto& v : salonArray) {
        const auto sObj = v.toObject();
        Salon salon(sObj.value("name").toString("Salon").toStdString());

        const auto wh = sObj.value("workingHours").toObject();
        TimeSlot whs;
        whs.startEpoch = static_cast<std::time_t>(wh.value("startEpoch").toVariant().toLongLong());
        const auto whEnd = static_cast<std::time_t>(wh.value("endEpoch").toVariant().toLongLong());
        if (whEnd > whs.startEpoch)
            whs.durationMinutes = static_cast<int>((whEnd - whs.startEpoch) / 60);
        else
            whs.durationMinutes = wh.value("duration").toInt(12 * 60);
        salon.setWorkingHours(whs);

        const auto employees = sObj.value("employees").toArray();
        for (const auto& ev : employees) {
            const auto e = ev.toObject();
            Employee emp(e.value("name").toString().toStdString(),
                         e.value("phone").toString().toStdString());

            for (const auto& sv : e.value("skills").toArray())
                emp.addSkill(sv.toString().toStdString());

            for (const auto& av : e.value("availability").toArray()) {
                const auto ao = av.toObject();
                TimeSlot ts;
                ts.startEpoch = static_cast<std::time_t>(ao.value("startEpoch").toVariant().toLongLong());
                const auto endEpoch = static_cast<std::time_t>(ao.value("endEpoch").toVariant().toLongLong());
                if (endEpoch > ts.startEpoch)
                    ts.durationMinutes = static_cast<int>((endEpoch - ts.startEpoch) / 60);
                else
                    ts.durationMinutes = ao.value("duration").toInt();
                if (ts.durationMinutes > 0)
                    emp.addAvailability(ts);
            }
            salon.addEmployee(emp);
        }

        const auto services = sObj.value("services").toArray();
        for (const auto& sv : services) {
            const auto s = sv.toObject();
            salon.addService(Service(
                s.value("name").toString().toStdString(),
                s.value("duration").toInt(),
                s.value("price").toDouble()
            ));
        }

        const auto apps = sObj.value("appointments").toArray();
        for (const auto& av : apps) {
            const auto a = av.toObject();

            bool eiOk = false, siOk = false;
            const int ei = safeToInt(a.value("employeeIndex"), eiOk);
            const int si = safeToInt(a.value("serviceIndex"), siOk);

            const auto& empList = salon.getEmployees();
            const auto& svcList = salon.getServices();

            if (!eiOk || !siOk || ei < 0 || si < 0) continue;

            const auto eIdx = static_cast<size_t>(ei);
            const auto sIdx = static_cast<size_t>(si);

            if (eIdx >= empList.size() || sIdx >= svcList.size()) continue;

            const Employee* eptr = &empList[eIdx];
            const Service*  sptr = &svcList[sIdx];

            TimeSlot ts;
            ts.startEpoch = static_cast<std::time_t>(a.value("startEpoch").toVariant().toLongLong());
            const auto endEpoch = static_cast<std::time_t>(a.value("endEpoch").toVariant().toLongLong());
            if (endEpoch > ts.startEpoch)
                ts.durationMinutes = static_cast<int>((endEpoch - ts.startEpoch) / 60);
            else
                ts.durationMinutes = a.value("duration").toInt();
            if (ts.durationMinutes <= 0) continue;

            Appointment made(&customers.front(), eptr, *sptr, ts);

            const auto st = a.value("status").toString("Approved");
            if (st == "Rejected")      made.reject();
            else /*Pending/Approved*/  made.approve();

            salon.addAppointment(made);
        }

        salons.push_back(salon);
    }

    const auto activeIdx = static_cast<size_t>(root.value("activeSalon").toInt(0));
    salonController.setActiveSalon(std::min(activeIdx, salons.empty() ? size_t{0} : salons.size() - 1));
    appointmentController.setActiveSalon(currentSalon());

    return !salons.empty();
}
