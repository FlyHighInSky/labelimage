#ifndef IMAGELABELWIDGET_H
#define IMAGELABELWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QLabel>

class ImageLabelWidget : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabelWidget(QWidget *parent = nullptr);
    void setImage(const QImage &img);

private:
    int j, dragging;
    int X[4]={10, 10, 10, 10}, Y[4]={10, 10, 10, 10};
    bool initFlag;
    QImage image;

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

signals:

public slots:
};

#endif // IMAGELABELWIDGET_H
