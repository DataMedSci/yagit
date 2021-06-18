#include <QApplication>
#include <iostream>
#include <yagitMainWidget.hpp>

void allMessagesHandler(QtMsgType msgType, const QMessageLogContext& context, const QString& message)
{
    std::cout << message.toStdString() << std::endl;
}

int main(int argc, char** argv)
{
    qInstallMessageHandler(allMessagesHandler);

    QApplication app(argc, argv);

    YAGITMainWidget widget;
    widget.show();

    return app.exec();
}