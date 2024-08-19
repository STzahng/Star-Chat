/******************************************************************************
 *
 * @file       logindialog.cpp
 * @brief
 *
 * @author     张明翔
 * @date       2024/07/27
 * @history
 *****************************************************************************/
#include "logindialog.h"
#include "ui_logindialog.h"
#include <QPainter>
#include "httpmgr.h"
LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{

    ui->setupUi(this);
    ui->forget_label->SetState("normal","hover","","selected","selected_hover","");
    ui->err_tip->setProperty("state","normal");
    repolish(ui->err_tip);
    ui->err_tip->clear();
    connect(ui->forget_label, &ClickedLabel::clicked, this, &LoginDialog::slot_forget_pwd);

    initHead();
    initHttpHandlers();
    //连接登录回包信号
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_login_mod_finish, this,
                &LoginDialog::slot_login_mod_finish);

    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });
    connect(ui->pwd_edit, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });

}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::initHttpHandlers()
{
    //注册获取登录回包逻辑
        _handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject jsonObj){
            int error = jsonObj["error"].toInt();
            if(error != ErrorCodes::SUCCESS){
                showTip(tr("参数错误"),false);

                return;
            }
            auto email = jsonObj["email"].toString();
            //发送信号通知tcpMgr发送长连接
            ServerInfo si;
            si.Uid = jsonObj["Uid"].toInt();
            si.Host = jsonObj["Host"].toString();
            si.Port = jsonObj["Port"].toString();
            si.Token = jsonObj["Token"].toString();

            showTip(tr("登录成功"), true);
            qDebug()<< "email is " << email ;
            emit sig_connect_tcp(si);
        });
}

void LoginDialog::initHead()
{
    //加载图片
    QPixmap originalPixmap(":/res/head1.jpg");
    qDebug()<<"1";
    //设置图片自动缩放
    originalPixmap = originalPixmap.scaled(ui->head_label->size(),
                        Qt::KeepAspectRatio,Qt::SmoothTransformation);

    //创建一个和原始图片相同大小的QPixmap，用于绘制圆角图片
    QPixmap roundedPixmap(originalPixmap.size());
    roundedPixmap.fill(Qt::transparent);//用透明色填充
    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);//设置抗锯齿，使圆角更平滑
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    //设置圆角
    QPainterPath path;
    path.addRoundedRect(0,0,originalPixmap.width(),originalPixmap.height(),10,10);
    painter.setClipPath(path);
    //将原始图片绘制到roundedPixmap上
    painter.drawPixmap(0,0,originalPixmap);
    //设置绘制好的圆角图片到QLabel上
    ui->head_label->setPixmap(roundedPixmap);
}
void LoginDialog::showTip(QString str, bool b_ok)
{
    if(b_ok){
        ui->err_tip->setProperty("state","normal");
    }else{
        ui->err_tip->setProperty("state","err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

void LoginDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void LoginDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
        if(_tip_errs.empty()){
          ui->err_tip->clear();
          return;
        }

        showTip(_tip_errs.first(), false);
}

void LoginDialog::enableBtn(bool enabled)
{
    ui->Login_PB->setEnabled(enabled);
    ui->Reg_PB->setEnabled(enabled);

}

bool LoginDialog::checkEmailValid()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    if(email== ""){
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱不能为空"));
        return false;
    }
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool LoginDialog::checkPassValid()
{
    auto pass = ui->pwd_edit->text();
    if(pass == ""){
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码不能为空"));
        return false;
    }

    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    return true;
}

void LoginDialog::on_Reg_PB_clicked()
{
    emit switchRegister();
}

void LoginDialog::slot_forget_pwd()
{
    emit switchReset();
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    enableBtn(true);
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"),false);
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"),false);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }


    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}

void LoginDialog::on_Login_PB_clicked()
{
    qDebug()<<"login btn clicked";
    if(checkEmailValid() == false){
        return;
    }

    if(checkPassValid() == false){
        return ;
    }
    enableBtn(false);
    auto email = ui->email_edit->text();
    auto pwd = ui->pwd_edit->text();
    //发送http请求登录
    QJsonObject json_obj;
    json_obj["email"] = email;
    json_obj["passwd"] = xorString(pwd);
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_login"),
                                        json_obj, ReqId::ID_LOGIN_USER,Modules::LOGINMOD);

}
