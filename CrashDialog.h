#ifndef CRASHDIALOG_H
#define CRASHDIALOG_H

#include <QDialog>

namespace Ui {
class CrashDialog;
}

class CrashDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrashDialog(QString code, QString addr, QWidget *parent = 0);
    ~CrashDialog();

private:
    Ui::CrashDialog *ui;
};

#endif // CRASHDIALOG_H
