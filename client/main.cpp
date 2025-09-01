#include "GUI_Client.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    GUI_Client client;
    client.show();
    return app.exec();
}
