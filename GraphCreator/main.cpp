#include "pch.h"
#include "CSamFrame.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CSamFrame* w = new CSamFrame();

    app.setApplicationVersion("2.1");
    app.setOrganizationName("SamStyle-01");

    w->show();
    return app.exec();
}
