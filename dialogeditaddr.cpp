#include "dialogeditaddr.h"
#include "ui_dialogeditaddr.h"

DialogEditAddr::DialogEditAddr(QString str, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditAddr)
{
    ui->setupUi(this);
    lineEditText = str;
    ui->lineEdit->setText(str);
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(checkModified()));
}

DialogEditAddr::~DialogEditAddr()
{
    delete ui;
}

QString DialogEditAddr::getLineEditText() const
{
    return lineEditText;
}

void DialogEditAddr::setLineEditText(const QString &value)
{
    lineEditText = value;
}

void DialogEditAddr::checkModified()
{
    if(ui->lineEdit->text() != lineEditText)
    {
        lineEditText = ui->lineEdit->text();
        this->accept();
    }
    else
        this->reject();
}
