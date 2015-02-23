#include <QApplication>
#include <QQuickView>
#include "Chat.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    qmlRegisterType<Chat>("ChatModule", 1, 0, "Chat");
    QQuickView view(QUrl("qrc:/chat_window.qml"));
    view.setMinimumSize(QSize(600, 400));
    view.connect((QObject*)(view.engine()), SIGNAL(quit()), &app, SLOT(quit()));
    view.show();
    return app.exec();
}