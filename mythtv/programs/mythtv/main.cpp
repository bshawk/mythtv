#include <qapplication.h>
#include <qsqldatabase.h>
#include <qstring.h>
#include <unistd.h>
#include "tv.h"

#include "libmyth/mythcontext.h"

#include <iostream>
using namespace std;

MythContext *gContext;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    gContext = new MythContext();
    gContext->ConnectServer("localhost", 6543);

    QString themename = gContext->GetSetting("Theme");
    QString themedir = gContext->FindThemeDir(themename);
    if (themedir == "")
    {   
        cerr << "Couldn't find theme " << themename << endl;
        exit(0);
    }
    
    gContext->LoadQtConfig();

    QSqlDatabase *db = QSqlDatabase::addDatabase("QMYSQL3");
    if (!db)
    {
        printf("Couldn't connect to database\n");
        return -1; 
    }       
    if (!gContext->OpenDatabase(db))
    {
        printf("couldn't open db\n");
        return -1;
    }

    TV *tv = new TV(db);
    tv->Init();

    tv->LiveTV();

    qApp->unlock();
    while (tv->GetState() == kState_None)
    {
        usleep(1000);
        qApp->processEvents();
    }

    while (tv->GetState() != kState_None)
    {
        usleep(1000);
        qApp->processEvents();
    }

    sleep(1);
    delete tv;
    delete gContext;

    exit(0);
}
