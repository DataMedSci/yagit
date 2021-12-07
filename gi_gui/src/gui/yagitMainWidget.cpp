#include <numeric>
#include <algorithm>
#include <utility>
#include <cmath>

#include <QMap>
#include <QDebug>

#include <wrapper.h>
#include <imebra/imebra.h>

#include <QDICOMFileSelectionEdit.hpp>

#include <yagitMainWidget.hpp>

#include <ui_yagitMainWidget.h>

using std::make_pair;

static const QMap<YAGITMainWidget::Dimension, QString> dimensionNameMapping =
{
    make_pair(YAGITMainWidget::Dimension::d1D, QString("1D")),
    make_pair(YAGITMainWidget::Dimension::d2D, QString("2D")),
    make_pair(YAGITMainWidget::Dimension::d3D, QString("3D")),
    make_pair(YAGITMainWidget::Dimension::d2_5D, QString("2.5D")),
    make_pair(YAGITMainWidget::Dimension::d2Df3D, QString("2D from 3D")),
};

static const QList<YAGITMainWidget::Dimension> dimensionOptions =
{
    YAGITMainWidget::Dimension::d1D,
    YAGITMainWidget::Dimension::d2D,
    YAGITMainWidget::Dimension::d3D,
    YAGITMainWidget::Dimension::d2_5D,
    YAGITMainWidget::Dimension::d2Df3D,
};

static const QMap<YAGITMainWidget::Dimension, QList<YAGITMainWidget::Plane>> dimensionPlanesMapping =
{
    make_pair(YAGITMainWidget::Dimension::d1D, QList<YAGITMainWidget::Plane>{}),
    make_pair(YAGITMainWidget::Dimension::d2D, QList<YAGITMainWidget::Plane>{}),
    make_pair(YAGITMainWidget::Dimension::d3D, QList<YAGITMainWidget::Plane>{}),
    make_pair(YAGITMainWidget::Dimension::d2_5D, QList<YAGITMainWidget::Plane>{YAGITMainWidget::Plane::XY, YAGITMainWidget::Plane::XZ, YAGITMainWidget::Plane::YZ}),
    make_pair(YAGITMainWidget::Dimension::d2Df3D, QList<YAGITMainWidget::Plane>{YAGITMainWidget::Plane::XY, YAGITMainWidget::Plane::XZ, YAGITMainWidget::Plane::YZ}),
};

static const QMap<YAGITMainWidget::Plane, QString> planeNameMapping =
{
    make_pair(YAGITMainWidget::Plane::XY, QString("XY")),
    make_pair(YAGITMainWidget::Plane::XZ, QString("XZ")),
    make_pair(YAGITMainWidget::Plane::YZ, QString("YZ")),
};

static const QMap<YAGITMainWidget::SaveMode, QString> saveModeNameMapping =
{
    make_pair(YAGITMainWidget::SaveMode::AllTags, QString("All tags")),
    make_pair(YAGITMainWidget::SaveMode::AnyTag, QString("Any tag")),
    make_pair(YAGITMainWidget::SaveMode::AllExceptPrivate, QString("All except private")),
};

static const QList<YAGITMainWidget::SaveMode> saveModeOptions =
{
    YAGITMainWidget::SaveMode::AllTags,
    YAGITMainWidget::SaveMode::AnyTag,
    YAGITMainWidget::SaveMode::AllExceptPrivate,
};

Ui::yagitMainWidget* d(void* _d)
{
    return static_cast<Ui::yagitMainWidget*>(_d);
}

YAGITMainWidget::YAGITMainWidget(QWidget* parent)
    : QWidget(parent)
    , _d(new Ui::yagitMainWidget())
    , referenceFilePreviewScene(new QGraphicsScene())
    , targetFilePreviewScene(new QGraphicsScene())
    , referenceFilePreviewPixmap(new QGraphicsPixmapItem())
    , targetFilePreviewPixmap(new QGraphicsPixmapItem())
{
    d(_d)->setupUi(this);

    _precisionDependentSpinBoxes
    << d(_d)->distanceToAgreementSpinBox
    << d(_d)->doseDifferenceSpinBox
    << d(_d)->referenceValueSpinBox
    << d(_d)->initialGammaIndexValueSpinBox
    << d(_d)->referenceScaleSpinBox
    << d(_d)->referenceBiasSpinBox
    << d(_d)->targetScaleSpinBox
    << d(_d)->targetBiasSpinBox
    << d(_d)->filterSpinBox;

    using std::numeric_limits;

    d(_d)->precisionSpinBox->setValue(3);

    std::for_each(
        std::cbegin(dimensionOptions),
        std::cend(dimensionOptions),
        [this](const Dimension& dimension)
        {
            d(_d)->dimensionComboBox->addItem(dimensionNameMapping[dimension]);
        }
    );
    d(_d)->dimensionComboBox->setCurrentIndex(0);

    std::for_each(
        std::cbegin(saveModeOptions),
        std::cend(saveModeOptions),
        [this](const SaveMode& saveMode)
        {
            d(_d)->saveModeComboBox->addItem(saveModeNameMapping[saveMode]);
        }
    );
    d(_d)->saveModeComboBox->setCurrentIndex(0);

    d(_d)->referenceFileSelectionEdit->previewView()->setScene(referenceFilePreviewScene);
    d(_d)->targetFileSelectionEdit->previewView()->setScene(targetFilePreviewScene);

    referenceFilePreviewScene->addItem(referenceFilePreviewPixmap);
    targetFilePreviewScene->addItem(targetFilePreviewPixmap);
}

YAGITMainWidget::~YAGITMainWidget()
{
    delete d(_d);
}

qreal YAGITMainWidget::distanceToAgreement() const
{
    return d(_d)->distanceToAgreementSpinBox->value();
}

uint YAGITMainWidget::precision() const
{
    return static_cast<uint>(d(_d)->precisionSpinBox->value());
}

YAGITMainWidget::Dimension YAGITMainWidget::dimension() const
{
    return dimensionOptions[d(_d)->dimensionComboBox->currentIndex()];
}

YAGITMainWidget::Plane YAGITMainWidget::plane() const
{
    auto& options = dimensionPlanesMapping[dimension()];
    return options.empty() ? Plane::None : options[d(_d)->planeComboBox->currentIndex()];
}

YAGITMainWidget::SaveMode YAGITMainWidget::saveMode() const
{
    return saveModeOptions[d(_d)->saveModeComboBox->currentIndex()];
}

void YAGITMainWidget::precisionValueChanged(int newPrecision)
{
    std::for_each(
        std::begin(_precisionDependentSpinBoxes),
        std::end(_precisionDependentSpinBoxes),
        [this](QDoubleSpinBox* precisionDependentSpinBox)
        {
            precisionDependentSpinBox->setDecimals(precision());
        }
    );

    d(_d)->distanceToAgreementSpinBox->setMinimum(std::pow(1e-1, precision()));
}

void YAGITMainWidget::dimensionItemIndexChanged(int newItemIndex)
{
    d(_d)->planeComboBox->clear();
    auto& options = dimensionPlanesMapping[dimension()];

    if(options.empty())
    {
        d(_d)->planeComboBox->setEnabled(false);
    }
    else
    {
        d(_d)->planeComboBox->setEnabled(true);
        std::for_each(
            std::cbegin(options),
            std::cend(options),
            [this](const Plane& plane)
            {
                d(_d)->planeComboBox->addItem(planeNameMapping[plane]);
            }
        );
        d(_d)->planeComboBox->setCurrentIndex(0);
    }
}

QPixmap createPixmapFromDoubleArray(const double* data, size_t sizeX, size_t sizeY)
{
    auto dataSetImage = QImage(sizeX, sizeY, QImage::Format_RGB32);

    for(size_t row = 0; row < sizeY; row++)
    {
        for(size_t column = 0; column < sizeX; column++)
        {
            int value = static_cast<int>(std::round(data[row * sizeX + column]));
            dataSetImage.setPixel(column, row, qRgb(value, value, value));
        }
    }

    return QPixmap::fromImage(dataSetImage);
}

void applyDataSetToPreview(QGraphicsPixmapItem* filePreviewPixmap, const std::unique_ptr<imebra::DataSet>& dataSet)
{
    if(dataSet)
    {
        int refNDims, refXNumber, refYNumber, refZNumber;
        double refXStart, refYStart, refZStart, refXSpacing, refYSpacing, refZSpacing;

        auto imageData = acquireImage(*dataSet, refNDims,
                                      refXStart, refXSpacing, refXNumber,
                                      refYStart, refYSpacing, refYNumber,
                                      refZStart, refZSpacing, refZNumber);

        filePreviewPixmap->setPixmap(createPixmapFromDoubleArray(imageData.get(), refXNumber, refYNumber));
    }
    else
    {
        filePreviewPixmap->setPixmap(QPixmap());
    }
}

void YAGITMainWidget::referenceFilePathChanged(std::filesystem::path newFilePath)
{
    referenceDicomDataSet = loadDicom(newFilePath.u8string());

    applyDataSetToPreview(referenceFilePreviewPixmap, referenceDicomDataSet);
}

void YAGITMainWidget::targetFilePathChanged(std::filesystem::path newFilePath)
{
    auto targetDicomDataSet = loadDicom(newFilePath.u8string());

    applyDataSetToPreview(targetFilePreviewPixmap, targetDicomDataSet);
}
