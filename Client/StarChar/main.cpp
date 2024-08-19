/******************************************************************************
 *
 * @file       main.cpp
 * @brief      主函数
 *
 * @author     张明翔
 * @date       2024/07/27
 * @history
 *****************************************************************************/
#include "mainwindow.h"
#include <QFile>
#include <QApplication>
#include "global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //配置窗口style
    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet((style));
        qss.close();
    }else {
        qDebug("Open false");
    }

    QString fileName = "config.ini";
    QString app_path = QCoreApplication::applicationDirPath();
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + fileName);
    QSettings settings(config_path,QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://" + gate_host + ":" + gate_port;
    MainWindow w;
    w.show();
    return a.exec();
}
