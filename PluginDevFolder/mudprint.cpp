#include "MudboxVR.h"


//void printMud(QString msg);
void printmud(QString msg);
void printmud(int msg);
void printmud(float msg);
void printmud(double msg);
void printmud(long msg);

void printmud(QString msg)
{
    Kernel()->Interface()->SetStatus(mudbox::Interface::stWarning, msg);
    auto stdmsg=msg.toStdString();
    std::cout<<stdmsg<<"\n";
}

void printmud(int msg)
{
    QString conmsg=QString::number(msg);
    Kernel()->Interface()->SetStatus(mudbox::Interface::stWarning, conmsg);
    std::cout<<msg<<"\n";
}

void printmud(float msg)
{
    QString conmsg=QString::number(msg);
    Kernel()->Interface()->SetStatus(mudbox::Interface::stWarning, conmsg);
    std::cout<<msg<<"\n";
}

void printmud(double msg)
{
    QString conmsg=QString::number(msg);
    Kernel()->Interface()->SetStatus(mudbox::Interface::stWarning, conmsg);
    std::cout<<msg<<"\n";
}

void printmud(long msg)
{
    QString conmsg=QString::number(msg);
    Kernel()->Interface()->SetStatus(mudbox::Interface::stWarning, conmsg);
    std::cout<<msg<<"\n";
}
