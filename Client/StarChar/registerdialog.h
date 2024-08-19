#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "global.h"
namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_get_code_PB_clicked();
    void slot_reg_mod_finish(ReqId id,QString res, ErrorCodes err);

    void on_OK_PB_clicked();

    void on_return_PB_2_clicked();

    void on_return_PB_clicked();

private:
    void initHttpHandlers();
    Ui::RegisterDialog *ui;
    void showTip(QString str,bool b_ok);
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);
    bool checkUserValid();
    bool checkPassValid();
    bool checkEmailValid();
    bool checkVarifyValid();
    bool checkConfirmValid();
    QMap<TipErr,QString>_tip_errs;//用于缓存提示的错误
    QMap<ReqId,std::function<void(const QJsonObject&)>> _handlers;

    QTimer *_countdown_timer;
    int _countdown;
    void ChangeTipPage();

signals:
    void sigSwitchLogin();
};

#endif // REGISTERDIALOG_H
