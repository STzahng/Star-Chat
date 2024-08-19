#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmgr.h"
RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog),_countdown(5)
{
    ui->setupUi(this);
    ui->err_tip->setProperty("state","normal");
    repolish(ui->err_tip);
    ui->err_tip->clear();
    connect(HttpMgr::GetInstance().get(),&HttpMgr::sig_reg_mod_finish,
            this,&RegisterDialog::slot_reg_mod_finish);
    initHttpHandlers();
    ui->err_tip->clear();

    connect(ui->user_edit,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });
    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });
    connect(ui->pwd_edit, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });
    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, [this](){
        checkConfirmValid();
    });
    connect(ui->varify_edit, &QLineEdit::editingFinished, this, [this](){
        checkVarifyValid();
    });
//    ui->pwd_visible->setCursor(Qt::PointingHandCursor);
//    ui->confirm_visible->setCursor(Qt::PointingHandCursor);
    ui->pwd_visible->SetState("unvisible","unvisible_hover","","visible",
                                "visible_hover","");

    ui->confirm_visible->SetState("unvisible","unvisible_hover","","visible",
                                    "visible_hover","");
    //连接点击事件

    connect(ui->pwd_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->pwd_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->pwd_edit->setEchoMode(QLineEdit::Password);
        }else{
                ui->pwd_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });

    connect(ui->confirm_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->confirm_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->confirm_edit->setEchoMode(QLineEdit::Password);
        }else{
                ui->confirm_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });

    // 创建定时器
    _countdown_timer = new QTimer(this);
    // 连接信号和槽
    connect(_countdown_timer, &QTimer::timeout, [this](){
        if(_countdown==0){
            _countdown_timer->stop();
            _countdown = 5;
            emit sigSwitchLogin();
            return;
        }
        _countdown--;
        auto str = QString("注册成功，%1秒后返回登录").arg(_countdown);
        ui->tip_lb->setText(str);
    });

}


RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_get_code_PB_clicked()
{
    auto email = ui->email_edit->text();
    if(checkEmailValid()){
       QJsonObject json_obj;
       json_obj["email"] = email;
       HttpMgr::GetInstance()->PostHttpReq(QUrl
                (gate_url_prefix + "/get_varifycode"),
                json_obj,ReqId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"),false);
        return;
    }
    //解析json  字符串，res转化为QByteArray
    QJsonDocument jsonDoc =  QJsonDocument::fromJson(res.toUtf8());//将字符串转换为json文件
    //qDebug()<<jsonDoc;
    if(jsonDoc.isNull()){
        qDebug()<<"jsonDoc解析失败";
        return;
    }
    //解析不是json对象
    if(!jsonDoc.isObject()){
        qDebug()<<"jsonDoc无法解析为json对象";
        return;
    }

    _handlers[id](jsonDoc.object());
    return ;
}

void RegisterDialog::initHttpHandlers()
{
    //注册获取验证码回包的逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE,[this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }

        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已经发送到邮箱，请注意查收"),true);
        qDebug()<<"email is : "<<email;
    });

    //注册注册用户回包逻辑
        _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj){
            int error = jsonObj["error"].toInt();
            if(error != ErrorCodes::SUCCESS){
                showTip(tr("参数错误"),false);
                return;
            }
            auto email = jsonObj["email"].toString();
            showTip(tr("用户注册成功"), true);

            qDebug()<<"user uuid is "<<jsonObj["uid"].toString();
            qDebug()<< "email is " << email ;
            ChangeTipPage();
        });
}

void RegisterDialog::showTip(QString str, bool b_ok)
{
    if(b_ok){
        ui->err_tip->setProperty("state","normal");
    }else{
        ui->err_tip->setProperty("state","err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

void RegisterDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void RegisterDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
        if(_tip_errs.empty()){
          ui->err_tip->clear();
          return;
        }

        showTip(_tip_errs.first(), false);
}

bool RegisterDialog::checkUserValid()
{
    if(ui->user_edit->text() == ""){
       AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
       return false;
   }

   DelTipErr(TipErr::TIP_USER_ERR);
   return true;
}

bool RegisterDialog::checkPassValid()
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

bool RegisterDialog::checkEmailValid()
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

bool RegisterDialog::checkVarifyValid()
{
   auto pass = ui->varify_edit->text();
   if(pass.isEmpty()){
       AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
       return false;
   }

   DelTipErr(TipErr::TIP_VARIFY_ERR);
   return true;
}

bool RegisterDialog::checkConfirmValid()
{
    auto confirm = ui->confirm_edit->text();
    if(confirm == ""){
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("确认密码不能为空"));
        return false;
    }
    if(confirm != ui->pwd_edit->text()){
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("两次输入的密码不同"));
        return false;
    }
    DelTipErr(TipErr::TIP_CONFIRM_ERR);
    return true;
}

void RegisterDialog::ChangeTipPage()
{
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);
    _countdown_timer->start(1000);
}


void RegisterDialog::on_OK_PB_clicked()
{

    bool valid = checkUserValid();
    if(!valid){
        return;
    }

    valid = checkEmailValid();
    if(!valid){
        return;
    }

    valid = checkPassValid();
    if(!valid){
        return;
    }

    valid = checkConfirmValid();
    if(!valid){
        return;
    }

    valid = checkVarifyValid();
    if(!valid){
        return;
    }
    //day11 发送http请求注册用户
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = xorString(ui->pwd_edit->text());
    json_obj["confirm"] = xorString(ui->confirm_edit->text());
    json_obj["varifycode"] = ui->varify_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"),
                 json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);

}

void RegisterDialog::on_return_PB_2_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

void RegisterDialog::on_return_PB_clicked()
{
    //_countdown_timer->stop();
    emit sigSwitchLogin();
}
