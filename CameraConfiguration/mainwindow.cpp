#include "mainwindow.h"
#include <Windows.h>
#include <winreg.h>
#include <QLabel>
#include <QPushButton>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    cbMirror=new QCheckBox(this);
    cbTrace=new QCheckBox(this);
    cbBlur=new QCheckBox(this);
    cbDebug=new QCheckBox(this);
    cbTransparent=new QCheckBox(this);

    leOpacity=new QLineEdit(this);
    leLeft=new QLineEdit(this);
    leTop=new QLineEdit(this);
    leWidth=new QLineEdit(this);
    leHeight=new QLineEdit(this);
    leDelay=new QLineEdit(this);
    leWidth->setGeometry(70,10,50,20);
    leHeight->setGeometry(220,10,50,20);
    leLeft->setGeometry(70,40,50,20);
    leTop->setGeometry(220,40,50,20);
    leOpacity->setGeometry(70,70,50,20);
    leDelay->setGeometry(220,70,50,20);
    QLabel *lab1=new QLabel(this);
    QLabel *lab2=new QLabel(this);
    QLabel *lab3=new QLabel(this);
    QLabel *lab4=new QLabel(this);
    QLabel *lab5=new QLabel(this);
    QLabel *lab6=new QLabel(this);
    lab1->setText(QString::fromWCharArray(L"窗口透明度"));
    lab2->setText(QString::fromWCharArray(L"左侧距离"));
    lab3->setText(QString::fromWCharArray(L"上方距离"));
    lab4->setText(QString::fromWCharArray(L"窗口宽度"));
    lab5->setText(QString::fromWCharArray(L"窗口高度"));
    lab6->setText(QString::fromWCharArray(L"延时"));
    lab1->setGeometry(0,10,70,20);
    lab2->setGeometry(150,10,70,20);
    lab3->setGeometry(0,40,70,20);
    lab4->setGeometry(150,40,70,20);
    lab5->setGeometry(0,70,70,20);
    lab6->setGeometry(150,70,70,20);
    lab1->show();lab2->show();lab3->show();lab4->show();lab5->show();lab6->show();
    cbMirror->setGeometry(30,130,250,15);
    cbMirror->setText(QString::fromWCharArray(L"镜像（左右翻转）"));
    cbTrace->setGeometry(30,160,250,15);
    cbTrace->setText(QString::fromWCharArray(L"人脸追踪（实验功能，可能不稳定）"));
    cbBlur->setGeometry(30,190,250,15);
    cbBlur->setText(QString::fromWCharArray(L"磨皮效果"));
    cbTransparent->setGeometry(30,220,250,15);
    cbTransparent->setText(QString::fromWCharArray(L"鼠标穿透。"));
    cbDebug->setGeometry(30,250,250,15);
    cbDebug->setText(QString::fromWCharArray(L"调试信息（影响性能）。"));
    pbYes=new QPushButton(this);
    pbReload=new QPushButton(this);
    pbYes->setText(QString::fromWCharArray(L"确认"));
    pbReload->setText(QString::fromWCharArray(L"加载现有值"));
    pbYes->setGeometry(30,280,90,20);
    pbReload->setGeometry(180,280,90,20);
    connect(pbYes,&QPushButton::pressed,this,[=](){
        HKEY hKey;
        RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"Software\\6ziv\\CameraOnScreen",0,KEY_SET_VALUE,&hKey);
        DWORD tmp,tmp1;

        tmp=cbBlur->isChecked();
        RegSetValueExW(hKey,L"Bluring",NULL,REG_DWORD,reinterpret_cast<LPBYTE>(&tmp),sizeof(DWORD));
        tmp=cbTrace->isChecked();
        RegSetValueExW(hKey,L"Tracing",NULL,REG_DWORD,reinterpret_cast<LPBYTE>(&tmp),sizeof(DWORD));
        tmp=cbTransparent->isChecked();
        RegSetValueExW(hKey,L"TransparentForMouse",NULL,REG_DWORD,reinterpret_cast<LPBYTE>(&tmp),sizeof(DWORD));
        tmp=cbMirror->isChecked();
        RegSetValueExW(hKey,L"ShowInMirror",NULL,REG_DWORD,reinterpret_cast<LPBYTE>(&tmp),sizeof(DWORD));
        tmp=cbDebug->isChecked();
        RegSetValueExW(hKey,L"Debug",NULL,REG_DWORD,reinterpret_cast<LPBYTE>(&tmp),sizeof(DWORD));

        bool ok=true;
        tmp=leWidth->text().toULong(&ok);
        if(ok)
            RegSetValueExW(hKey,L"WindowWidth",NULL,REG_DWORD,reinterpret_cast<LPBYTE>(&tmp),sizeof(DWORD));
        else {
            MessageBoxA(0,"An illegal value!","Error",MB_OK);
        }

        tmp=leHeight->text().toULong(&ok);
        if(ok)
            RegSetValueExW(hKey,L"WindowHeight",NULL,REG_DWORD,reinterpret_cast<LPBYTE>(&tmp),sizeof(DWORD));
        else {
            MessageBoxA(0,"An illegal value!","Error",MB_OK);
        }

        tmp=leLeft->text().toULong(&ok);
        if(ok)
            RegSetValueExW(hKey,L"WindowLeft",NULL,REG_DWORD,reinterpret_cast<LPBYTE>(&tmp),sizeof(DWORD));
        else {
            MessageBoxA(0,"An illegal value!","Error",MB_OK);
        }

        tmp=leTop->text().toULong(&ok);
        if(ok)
            RegSetValueExW(hKey,L"WindowTop",NULL,REG_DWORD,reinterpret_cast<LPBYTE>(&tmp),sizeof(DWORD));
        else {
            MessageBoxA(0,"An illegal value!","Error",MB_OK);
        }

        tmp=leOpacity->text().toULong(&ok);
        if(ok)
            RegSetValueExW(hKey,L"WindowOpacity",NULL,REG_DWORD,reinterpret_cast<LPBYTE>(&tmp),sizeof(DWORD));
        else {
            MessageBoxA(0,"An illegal value!","Error",MB_OK);
        }
        tmp=leDelay->text().toULong(&ok);
        if(ok)
            RegSetValueExW(hKey,L"Delay",NULL,REG_DWORD,reinterpret_cast<LPBYTE>(&tmp),sizeof(DWORD));
        else {
            MessageBoxA(0,"An illegal value!","Error",MB_OK);
        }
        pbReload->click();
    });

    connect(pbReload,&QPushButton::pressed,this,[=](){
        HKEY hKey;
        RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"Software\\6ziv\\CameraOnScreen",0,KEY_QUERY_VALUE,&hKey);
        DWORD tmp,tmp1;

        RegQueryValueExW(hKey,L"Bluring",NULL,NULL,reinterpret_cast<LPBYTE>(&tmp),&tmp1);
        cbBlur->setChecked(tmp);
        RegQueryValueExW(hKey,L"Tracing",NULL,NULL,reinterpret_cast<LPBYTE>(&tmp),&tmp1);
        cbTrace->setChecked(tmp);
        RegQueryValueExW(hKey,L"TransparentForMouse",NULL,NULL,reinterpret_cast<LPBYTE>(&tmp),&tmp1);
        cbTransparent->setChecked(tmp);
        RegQueryValueExW(hKey,L"ShowInMirror",NULL,NULL,reinterpret_cast<LPBYTE>(&tmp),&tmp1);
        cbMirror->setChecked(tmp);
        RegQueryValueExW(hKey,L"Debug",NULL,NULL,reinterpret_cast<LPBYTE>(&tmp),&tmp1);
        cbDebug->setChecked(tmp);

        RegQueryValueExW(hKey,L"WindowWidth",NULL,NULL,reinterpret_cast<LPBYTE>(&tmp),&tmp1);
        leWidth->setText(QString::number(tmp));
        RegQueryValueExW(hKey,L"WindowHeight",NULL,NULL,reinterpret_cast<LPBYTE>(&tmp),&tmp1);
        leHeight->setText(QString::number(tmp));
        RegQueryValueExW(hKey,L"WindowLeft",NULL,NULL,reinterpret_cast<LPBYTE>(&tmp),&tmp1);
        leLeft->setText(QString::number(tmp));
        RegQueryValueExW(hKey,L"WindowTop",NULL,NULL,reinterpret_cast<LPBYTE>(&tmp),&tmp1);
        leTop->setText(QString::number(tmp));
        RegQueryValueExW(hKey,L"WindowOpacity",NULL,NULL,reinterpret_cast<LPBYTE>(&tmp),&tmp1);
        leOpacity->setText(QString::number(tmp));
        RegQueryValueExW(hKey,L"Delay",NULL,NULL,reinterpret_cast<LPBYTE>(&tmp),&tmp1);
        leDelay->setText(QString::number(tmp));

        RegCloseKey(hKey);
    });
    this->setMinimumSize(300,300);
    this->setMaximumSize(300,300);
    this->show();
}

MainWindow::~MainWindow()
{

}
