#pragma once

#include <QGraphicsScene>
#include "UndoHistory.h"
#include "viewscene.h"

class ImageLabeler
{
public:
    ImageLabeler(const QString& img);
    ~ImageLabeler();

    void setMaxViewSize(int w, int h);
    double viewZoom() const;
    void setViewZoom(double zoom);
    void drawView();
    void saveToFile(const QString& path);

	void Rotate(double angle);
	void CropToSelection();
	void Resize(double zoom);
	void Resize(int w, int h);
	void ConvertToGrayscale(int threshold = -1);

	void Undo();
	void Redo();
	QSize Size();

    ViewScene* viewScene;
//	RubberBand rubberBand;

private:
	double previewZoom = 1;
	UndoHistory<QImage*> history;
//	bool rubberBandInstalled = false;

};
