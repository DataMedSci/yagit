#include <QDICOMFileSelectionEditPlugin.hpp>
#include <QDICOMFileSelectionEdit.hpp>

QDICOMFileSelectionEditPlugin::QDICOMFileSelectionEditPlugin(QObject *parent)
    : QObject(parent)
{
}

void QDICOMFileSelectionEditPlugin::initialize(QDesignerFormEditorInterface* core)
{
    if (_initialized)
        return;

    _initialized = true;
}

bool QDICOMFileSelectionEditPlugin::isInitialized() const
{
    return _initialized;
}

QWidget* QDICOMFileSelectionEditPlugin::createWidget(QWidget* parent)
{
    return new QDICOMFileSelectionEdit(parent);
}

QString QDICOMFileSelectionEditPlugin::name() const
{
    return QStringLiteral("QDICOMFileSelectionEdit");
}

QString QDICOMFileSelectionEditPlugin::group() const
{
    return QStringLiteral("yAGIT");
}

QIcon QDICOMFileSelectionEditPlugin::icon() const
{
    return QIcon(":/logo.ico");
}

QString QDICOMFileSelectionEditPlugin::toolTip() const
{
    return QString();
}

QString QDICOMFileSelectionEditPlugin::whatsThis() const
{
    return QString();
}

bool QDICOMFileSelectionEditPlugin::isContainer() const
{
    return false;
}

QString QDICOMFileSelectionEditPlugin::includeFile() const
{
    return QStringLiteral("QDICOMFileSelectionEdit.hpp");
}
