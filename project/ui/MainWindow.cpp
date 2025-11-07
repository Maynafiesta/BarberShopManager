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

// JSON / Dosya
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("BarberShopManager");
    buildUi();
    refreshTables();
    refreshAppointments();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::buildUi() {
    central = new QWidget(this);
    auto* v = new QVBoxLayout(central);

    // Üst şerit (demo + tarih/saat + oluştur)
    auto* top = new QHBoxLayout();
    btnLoadDemo   = new QPushButton("Demo veriyi yükle", central);

    dateEdit = new QDateEdit(QDate::currentDate(), central);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");

    timeEdit = new QTimeEdit(QTime(10,0), central);
    timeEdit->setDisplayFormat("HH:mm");

    btnCreateAppt = new QPushButton("Seçili çalışan + hizmet ile randevu", central);

    top->addWidget(btnLoadDemo);
    top->addSpacing(12);
    top->addWidget(new QLabel("Tarih:", central));
    top->addWidget(dateEdit);
    top->addSpacing(8);
    top->addWidget(new QLabel("Saat:", central));
    top->addWidget(timeEdit);
    top->addSpacing(12);
    top->addWidget(btnCreateAppt);
    top->addStretch(1);

    // Orta tablolar (çalışanlar ve hizmetler)
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

    mid->addWidget(tblEmployees, 1);
    mid->addWidget(tblServices,  1);

    // Randevular tablosu
    tblAppointments = new QTableWidget(0, 5, central);
    tblAppointments->setHorizontalHeaderLabels({ "Tarih/Saat", "Çalışan", "Hizmet", "Durum", "Ücret" });
    tblAppointments->horizontalHeader()->setStretchLastSection(true);
    tblAppointments->verticalHeader()->setVisible(false);
    tblAppointments->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblAppointments->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblAppointments->setSelectionMode(QAbstractItemView::SingleSelection);

    // Randevu aksiyonları
    auto* actions = new QHBoxLayout();
    btnReject = new QPushButton("Seçileni Reddet", central);
    btnDelete = new QPushButton("Seçileni Sil", central);
    actions->addWidget(btnReject);
    actions->addWidget(btnDelete);
    actions->addStretch(1);

    // Kalıcılık butonları (kaydet/yükle)
    auto* persist = new QHBoxLayout();
    btnSave = new QPushButton("JSON’a Kaydet", central);
    btnLoad = new QPushButton("JSON’dan Yükle", central);
    persist->addWidget(btnSave);
    persist->addWidget(btnLoad);
    persist->addStretch(1);

    // Alt log
    txtLog = new QPlainTextEdit(central);
    txtLog->setReadOnly(true);
    txtLog->setMinimumHeight(120);

    v->addLayout(top);
    v->addLayout(mid);
    v->addWidget(new QLabel("Randevular:", central));
    v->addWidget(tblAppointments);
    v->addLayout(actions);
    v->addLayout(persist);
    v->addWidget(txtLog);

    setCentralWidget(central);

    connect(btnLoadDemo,   &QPushButton::clicked, this, &MainWindow::onLoadDemo);
    connect(btnCreateAppt, &QPushButton::clicked, this, &MainWindow::onCreateAppointment);
    connect(btnReject,     &QPushButton::clicked, this, &MainWindow::onRejectSelected);
    connect(btnDelete,     &QPushButton::clicked, this, &MainWindow::onDeleteSelected);

    connect(btnSave,       &QPushButton::clicked, this, &MainWindow::onSaveJson);
    connect(btnLoad,       &QPushButton::clicked, this, &MainWindow::onLoadJson);
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

void MainWindow::refreshAppointments() {
    const auto& apps = salon.getAppointments();
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

int MainWindow::selectedAppointmentRow() const {
    auto sel = tblAppointments->selectionModel()->selectedRows();
    return sel.isEmpty() ? -1 : sel.front().row();
}

void MainWindow::onLoadDemo() {
    const auto today = QDate::currentDate();
    const auto start = QDateTime(today, QTime(9,0), Qt::LocalTime).toSecsSinceEpoch();

    salon = Salon("Merkez Şube");
    salon.setWorkingHours(TimeSlot{ static_cast<std::time_t>(start), 12*60 }); // 09:00-21:00

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

    salon.addService(Service("Saç kesimi", 30, 250.0));
    salon.addService(Service("Sakal tıraşı", 20, 150.0));
    salon.addService(Service("Boya", 90, 900.0));
    salon.addService(Service("Fön", 15, 120.0));

    customers.clear();
    customers.emplace_back("Tarık", "0555 555 55 55");

    refreshTables();
    refreshAppointments();
    log("Demo yüklendi. Tarih/Saat seç, bir çalışan + bir hizmet seç ve randevu oluştur.");
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

    const QDate d = dateEdit->date();
    const QTime t = timeEdit->time();
    TimeSlot slot { static_cast<std::time_t>(
        QDateTime(d, t, Qt::LocalTime).toSecsSinceEpoch()),
        service.getDurationMinutes()
    };

    Appointment created;
    const auto res = scheduler.createAppointment(salon, customer, employee, service, slot, created);

    switch (res) {
        case Scheduler::CreateResult::Ok:
            log(QString("Randevu onaylandı: %1 - %2 @ %3 (%4) ₺%5")
                .arg(QString::fromStdString(employee.getName()))
                .arg(QString::fromStdString(service.getName()))
                .arg(QDateTime::fromSecsSinceEpoch(created.getSlot().startEpoch).toString("dd.MM.yyyy HH:mm"))
                .arg(service.getDurationMinutes())
                .arg(created.getTotalPrice(), 0, 'f', 2));
            refreshAppointments();
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
    if (salon.rejectAppointmentAt(static_cast<size_t>(row))) {
        log("Randevu reddedildi.");
        refreshAppointments();
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

    if (salon.removeAppointmentAt(static_cast<size_t>(row))) {
        log("Randevu silindi.");
        refreshAppointments();
    } else {
        log("Randevu silinemedi (geçersiz indeks).");
    }
}

/* ===================== JSON Kalıcılık ===================== */

void MainWindow::onSaveJson() {
    const QString path = QFileDialog::getSaveFileName(this, "JSON'a Kaydet", "salon.json", "JSON (*.json)");
    if (path.isEmpty()) return;

    const QByteArray data = serializeSalonToJson();
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) {
        log("Dosya açılamadı: " + path);
        return;
    }
    f.write(data);
    f.close();
    log("Kaydedildi: " + path);
}

void MainWindow::onLoadJson() {
    const QString path = QFileDialog::getOpenFileName(this, "JSON'dan Yükle", "", "JSON (*.json)");
    if (path.isEmpty()) return;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        log("Dosya açılamadı: " + path);
        return;
    }
    const QByteArray data = f.readAll();
    f.close();

    if (deserializeSalonFromJson(data)) {
        refreshTables();
        refreshAppointments();
        log("Yüklendi: " + path);
    } else {
        log("JSON yüklenemedi: " + path);
    }
}

QByteArray MainWindow::serializeSalonToJson() const {
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
            to["duration"]   = t.durationMinutes;
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
    // employee* → index map’i kur
    QHash<const Employee*, int> empIndex;
    for (int i = 0; i < employees.size(); ++i) {
        empIndex.insert(&emps[static_cast<size_t>(i)], i);
    }
    QJsonArray apps;
    const auto& aList = salon.getAppointments();
    for (const auto& a : aList) {
        QJsonObject ao;
        ao["startEpoch"] = static_cast<qint64>(a.getSlot().startEpoch);
        ao["duration"]   = a.getSlot().durationMinutes;

        // employee index
        int ei = -1;
        if (a.getEmployee()) {
            auto it = empIndex.find(a.getEmployee());
            if (it != empIndex.end()) ei = it.value();
        }
        ao["employeeIndex"] = ei;

        // service index: adına bakarak bul (küçük MVP)
        int si = -1;
        for (int i = 0; i < services.size(); ++i) {
            if (services[i].toObject().value("name").toString() ==
                QString::fromStdString(a.getService().getName())) { si = i; break; }
        }
        ao["serviceIndex"] = si;

        // status
        QString st = "Pending";
        switch (a.getStatus()) {
            case Appointment::Status::Pending:  st = "Pending"; break;
            case Appointment::Status::Approved: st = "Approved"; break;
            case Appointment::Status::Rejected: st = "Rejected"; break;
        }
        ao["status"] = st;

        // price (service.price’ı zaten saklıyoruz, yine de yaz)
        ao["price"] = a.getTotalPrice();

        apps.push_back(ao);
    }

    // salon / working hours
    QJsonObject working;
    working["startEpoch"] = static_cast<qint64>(salon.getWorkingHours().startEpoch);
    working["duration"]   = salon.getWorkingHours().durationMinutes;

    // customers (MVP: tek müşteri)
    QJsonArray custs;
    if (!customers.empty()) {
        QJsonObject c;
        c["name"]  = QString::fromStdString(customers.front().getName());
        c["phone"] = QString::fromStdString(customers.front().getPhone());
        custs.push_back(c);
    }

    QJsonObject root;
    root["name"]         = QString::fromStdString(salon.getName());
    root["workingHours"] = working;
    root["employees"]    = employees;
    root["services"]     = services;
    root["appointments"] = apps;
    root["customers"]    = custs;

    return QJsonDocument(root).toJson(QJsonDocument::Indented);
}

bool MainWindow::deserializeSalonFromJson(const QByteArray& data) {
    QJsonParseError err{};
    const auto doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return false;

    const auto root = doc.object();

    // reset salon
    salon = Salon(root.value("name").toString("Salon").toStdString());

    // working hours
    const auto wh = root.value("workingHours").toObject();
    TimeSlot whs;
    whs.startEpoch     = static_cast<std::time_t>(wh.value("startEpoch").toVariant().toLongLong());
    whs.durationMinutes= wh.value("duration").toInt(12*60);
    salon.setWorkingHours(whs);

    // employees
    std::vector<Employee> empsTmp;
    const auto employees = root.value("employees").toArray();
    empsTmp.reserve(static_cast<size_t>(employees.size()));
    for (const auto& v : employees) {
        const auto e = v.toObject();
        Employee emp(e.value("name").toString().toStdString(),
                     e.value("phone").toString().toStdString());

        for (const auto& sv : e.value("skills").toArray())
            emp.addSkill(sv.toString().toStdString());

        for (const auto& av : e.value("availability").toArray()) {
            const auto ao = av.toObject();
            TimeSlot ts;
            ts.startEpoch      = static_cast<std::time_t>(ao.value("startEpoch").toVariant().toLongLong());
            ts.durationMinutes = ao.value("duration").toInt();
            emp.addAvailability(ts);
        }
        empsTmp.push_back(emp);
    }
    for (const auto& e : empsTmp) salon.addEmployee(e);

    // services
    std::vector<Service> svcsTmp;
    const auto services = root.value("services").toArray();
    svcsTmp.reserve(static_cast<size_t>(services.size()));
    for (const auto& v : services) {
        const auto s = v.toObject();
        svcsTmp.emplace_back(
            s.value("name").toString().toStdString(),
            s.value("duration").toInt(),
            s.value("price").toDouble()
        );
    }
    for (const auto& s : svcsTmp) salon.addService(s);

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

    // appointments
    const auto apps = root.value("appointments").toArray();
    for (const auto& v : apps) {
        const auto a = v.toObject();

        const int ei = a.value("employeeIndex").toInt(-1);
        const int si = a.value("serviceIndex").toInt(-1);

        const Employee* eptr = nullptr;
        const Service*  sptr = nullptr;

        if (ei >= 0 && ei < employees.size())
            eptr = &salon.getEmployees()[static_cast<size_t>(ei)];

        if (si >= 0 && si < services.size()) {
            const auto name = services[si].toObject().value("name").toString();
            // isme göre bul
            for (const auto& s : salon.getServices()) {
                if (QString::fromStdString(s.getName()) == name) { sptr = &s; break; }
            }
        }

        if (!eptr || !sptr) continue; // veri tutarsızsa atla

        TimeSlot ts;
        ts.startEpoch      = static_cast<std::time_t>(a.value("startEpoch").toVariant().toLongLong());
        ts.durationMinutes = a.value("duration").toInt();

        Appointment made(&customers.front(), eptr, *sptr, ts);

        const auto st = a.value("status").toString("Approved");
        if (st == "Rejected")      made.reject();
        else /*Pending/Approved*/  made.approve();

        salon.addAppointment(made);
    }

    return true;
}
