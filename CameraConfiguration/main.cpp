#include "mainwindow.h"
#include <QApplication>
#include <Windows.h>
#include <WtsApi32.h>
bool IsAdministrator(){
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID Sid;
    AllocateAndInitializeSid(&NtAuthority,
                             2,
                             SECURITY_BUILTIN_DOMAIN_RID,
                             DOMAIN_ALIAS_RID_ADMINS,
                             0, 0, 0, 0, 0, 0,
                             &Sid);
    int ret;
    CheckTokenMembership(nullptr, Sid, &ret);
    FreeSid(Sid);
    return ret;
}
int main(int argc, char *argv[])
{
    if(!IsAdministrator()){
        MessageBoxA(0,"Please Run As Administrator!","No Privilege",MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION);
        return 0;
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
