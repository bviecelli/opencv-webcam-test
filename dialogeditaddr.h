#ifndef DIALOGEDITADDR_H
#define DIALOGEDITADDR_H

#include <QDialog>

namespace Ui {
class DialogEditAddr;
}

class DialogEditAddr : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditAddr(QString str, QWidget *parent = 0);
    ~DialogEditAddr();

    QString getLineEditText() const;
    void setLineEditText(const QString &value);

private:
    Ui::DialogEditAddr *ui;
    QString lineEditText;

private slots:
    void checkModified();
};

#endif // DIALOGEDITADDR_H
