#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

private:
    Ui::ResetDialog *ui;
    void showTip(QString str,bool b_ok);
    bool checkUserValid();
    bool checkPassValid();
    bool checkEmailValid();
    bool checkVarifyValid();
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);
    void initHandlers();
    QMap<TipErr,QString> _tip_errs;
    QMap<ReqId,std::function<void(const QJsonObject&)>> _handlers;

private slots:
    void slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err);
    void on_return_PB_clicked();

    void on_OK_PB_clicked();

    void on_get_code_PB_clicked();

signals:
    void switchLogin();
};

#endif // RESETDIALOG_H
