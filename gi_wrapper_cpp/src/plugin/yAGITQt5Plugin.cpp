#include <yAGITQt5Plugin.hpp>
#include <QDICOMFileSelectionEditPlugin.hpp>

yAGITQt5Plugin::yAGITQt5Plugin(QObject *parent)
    : QObject(parent)
{
    _widgets.append(new QDICOMFileSelectionEditPlugin(this));
}


QList<QDesignerCustomWidgetInterface*> yAGITQt5Plugin::customWidgets() const
{
    return _widgets;
}
