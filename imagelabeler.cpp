#include "imagelabeler.h"
#include <QImage>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

ImageLabeler::ImageLabeler(const QString& path) :
//        viewScene(new QGraphicsScene()),
//	rubberBand(),
	history(5)
{
//    viewScene = new ViewScene(this);
	history.push(new QImage(path));
        if (history.current()->isNull())
            throw -1;
}

ImageLabeler::~ImageLabeler()
{
        delete viewScene;
}

/**
 * @brief Draws image view.
 */
void ImageLabeler::drawView()
{
        viewScene->clear();

	// Actual image layer.
	auto image = new QGraphicsPixmapItem(QPixmap::fromImage(*history.current()));
	image->setAcceptHoverEvents(true);
	image->setScale(previewZoom);
	image->setTransformationMode(Qt::SmoothTransformation);

	// Layer with decorative half-transparent border.
	auto pixmapSize = image->pixmap().size();
	auto border = new QGraphicsRectItem(0, 0, pixmapSize.width(), pixmapSize.height());
	border->setScale(previewZoom);
	border->setPen(QPen(QColor(0, 0, 0, 64), 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));

	// Adds layers to scene.
        viewScene->addItem(image);
        viewScene->addItem(border);

	// Refresh selection.
//	auto viewport = previewScene->views().first()->viewport();
//	if (!rubberBandInstalled)
//	{
//		viewport->installEventFilter(&rubberBand);
//	}

//	rubberBand.SetPixmapItem(image);

	// Reset scene size to make scrollbars disappear.
	QPoint zero(0, 0);
	QRect rect(zero, history.current()->size() * previewZoom);
        viewScene->setSceneRect(rect);
}

/**
 * @brief Saves image to file on disk.
 * @param path Save path.
 */
void ImageLabeler::saveToFile(const QString& path)
{
	history.current()->save(path);
}

/**
 * @brief Rotates image by given angle in degrees.
 */
void ImageLabeler::Rotate(double angle)
{
	QTransform transform;
	transform.rotate(angle);
	history.push(new QImage(history.current()->transformed(transform, Qt::SmoothTransformation)));
        drawView();
}

/**
 * @brief Crops image to current selection.
 */
void ImageLabeler::CropToSelection()
{
//	if (rubberBand.IsSelectionActive())
//	{
//		history.push(new QImage(history.current()->copy(rubberBand.GetSelection())));
//		DrawPreview();
//	}
}

/**
 * @brief Scales image given amount of times.
 */
void ImageLabeler::Resize(double zoom)
{
	history.push(new QImage(
		history.current()->scaled(
			       history.current()->size() * zoom,
			       Qt::IgnoreAspectRatio,
			       Qt::SmoothTransformation
		       )));
        drawView();
}

/**
 * @brief Scales image by fixed size.
 */
void ImageLabeler::Resize(int w, int h)
{
	history.push(new QImage(
		history.current()->scaled(
			       w, h,
			       Qt::IgnoreAspectRatio,
			       Qt::SmoothTransformation
		       )));
        drawView();
}

/**
 * @brief Converts image to greyscale.
 */
void ImageLabeler::ConvertToGrayscale(int threshold)
{
	// Convert to 32bit pixel format
	auto format = history.current()->hasAlphaChannel()
		              ? QImage::Format_ARGB32
		              : QImage::Format_RGB32;

	auto dstImage = history.current()->convertToFormat(format);

	auto data = reinterpret_cast<unsigned int*>(dstImage.bits());
	auto pixelCount = dstImage.width() * dstImage.height();

	// Convert each pixel to grayscale
	for (auto i = 0; i < pixelCount; ++i)
	{
		auto val = qGray(*data);

		if (threshold != -1)
		{
			val = val > threshold ? 255 : 0;
		}

		*data = qRgba(val, val, val, qAlpha(*data));
		++data;
	}


	history.push(new QImage(dstImage));
        drawView();
}

/**
* @brief Undoes last operation.
*/
void ImageLabeler::Undo()
{
	if (history.isUndoPossible())
	{
		history.undo();
                drawView();
	}
}

/**
 * @brief Redoes last operation.
 */
void ImageLabeler::Redo()
{
	if (history.isRedoPossible())
	{
		history.redo();
                drawView();
	}
}

/**
 * @brief Returns current image size in pixels.
 */
QSize ImageLabeler::Size()
{
	return history.current()->size();
}

/**
 * @brief Returns current preview zoom of image.
 */
double ImageLabeler::viewZoom() const
{
	return previewZoom;
}

/**
 * @brief Sets image preview zoom.
 */
void ImageLabeler::setViewZoom(double zoom)
{
	previewZoom = zoom;
        drawView();
}

/**
 * @brief Sets fixed size of image preview.
 */
void ImageLabeler::setMaxViewSize(int w, int h)
{
        auto size = history.current()->size();
	previewZoom = std::min(1.0 * w / size.width(), 1.0 * h / size.height());
        drawView();
}
