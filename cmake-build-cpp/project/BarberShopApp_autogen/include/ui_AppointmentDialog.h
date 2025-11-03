/********************************************************************************
** Form generated from reading UI file 'AppointmentDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APPOINTMENTDIALOG_H
#define UI_APPOINTMENTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AppointmentDialog
{
public:
    QVBoxLayout *verticalLayout;

    void setupUi(QDialog *AppointmentDialog)
    {
        if (AppointmentDialog->objectName().isEmpty())
            AppointmentDialog->setObjectName(QString::fromUtf8("AppointmentDialog"));
        verticalLayout = new QVBoxLayout(AppointmentDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

        retranslateUi(AppointmentDialog);

        QMetaObject::connectSlotsByName(AppointmentDialog);
    } // setupUi

    void retranslateUi(QDialog *AppointmentDialog)
    {
        AppointmentDialog->setWindowTitle(QApplication::translate("AppointmentDialog", "New Appointment", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AppointmentDialog: public Ui_AppointmentDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APPOINTMENTDIALOG_H
