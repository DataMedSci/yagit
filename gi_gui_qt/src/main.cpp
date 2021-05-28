#include <QApplication>
#include <example.hpp>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    ExampleWidget widget;
    widget.show();

    return app.exec();
}