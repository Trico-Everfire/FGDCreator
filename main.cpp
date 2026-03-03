#include <QApplication>
#include <QPushButton>
#include "src/mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    auto window = new CMainWindow(nullptr);
    window->show();

    return QApplication::exec();
}
