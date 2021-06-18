#pragma once

#include <QObject>
#include <QtUiPlugin/QDesignerCustomWidgetInterface>

class QDICOMFileSelectionEditPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
private:
    bool _initialized = false;
public:
    explicit QDICOMFileSelectionEditPlugin(QObject* parent = nullptr);
public:
    void initialize(QDesignerFormEditorInterface* core) override;
    bool isInitialized() const override;

    QWidget* createWidget(QWidget* parent) override;
    QString name() const override;
    QString group() const override;
    QIcon icon() const override;

    QString toolTip() const override;
    QString whatsThis() const override;

    bool isContainer() const override;
    //QString domXml() const override;
    QString includeFile() const override;
};