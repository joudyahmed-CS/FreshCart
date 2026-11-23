#include "mainwindow.h"
#include <QApplication>
#include <QFont>
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("FreshCart");
    app.setOrganizationName("FreshCart Student Project");
    app.setStyle("Fusion");
    app.setFont(QFont("Segoe UI", 10));
    MainWindow window;
    window.show();
    return app.exec();
}
