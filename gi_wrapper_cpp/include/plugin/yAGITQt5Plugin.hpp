#pragma once

#include <QObject>
#include <QDesignerCustomWidgetCollectionInterface>

class yAGITQt5Plugin : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
private:
    QList<QDesignerCustomWidgetInterface*> _widgets;
public:
    explicit yAGITQt5Plugin(QObject* parent = 0);
public:
    QList<QDesignerCustomWidgetInterface*> customWidgets() const override;
};