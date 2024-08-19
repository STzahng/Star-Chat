/******************************************************************************
 *
 * @file       logindialog.h
 * @brief
 *
 * @author     张明翔
 * @date       2024/07/27
 * @history
 *****************************************************************************/
#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"
namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    void initHttpHandlers();
    Ui::LoginDialog *ui;
    void initHead();
    void showTip(QString str,bool b_ok);
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);
    void enableBtn(bool enabled);
    bool checkEmailValid();
    bool checkPassValid();


    int _uid;
    QString _token;
    QMap<TipErr,QString>_tip_errs;//用于缓存提示的错误
    QMap<ReqId,std::function<void(const QJsonObject&)>> _handlers;

signals:
    void switchRegister();
    void switchReset();
    void sig_connect_tcp(ServerInfo);
private slots:
    void on_Reg_PB_clicked();
    void on_Login_PB_clicked();
    void slot_forget_pwd();
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // LOGINDIALOG_H
