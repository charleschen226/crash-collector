#include "CrashDialog.h"
#include "ui_CrashDialog.h"

CrashDialog::CrashDialog(QString code, QString addr, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CrashDialog)
{
    ui->setupUi(this);
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QFont font("Consolas", 11);
    ui->errorCode->setFont(font);
    ui->errorAddr->setFont(font);

    ui->errorCode->setText(code);
    ui->errorAddr->setText(addr);
    setWindowTitle(tr("错误报告"));
}

CrashDialog::~CrashDialog()
{
    delete ui;
}
