#include <QWidget>
#include <QPainter>

enum CaptureState{
	InitCapture = 0,
	BeginCaptureImage,
	FinishCaptureImage,
	BeginMoveCaptureArea,
	FinishMoveCaptureArea,
	BeginMoveStretchRect,
	FinishMoveStretchRect
}; //���н�����״̬;

enum StretchRectState{
	NotSelect = 0,
	TopLeftRect,
	TopRightRect,
	BottomLeftRect,
	BottomRightRect,
	LeftCenterRect,
	TopCenterRect,
	RightCenterRect,
	BottomCenterRect
};// ��ǰ������ڶ���״̬;

class LabelRegion : public QWidget
{
	Q_OBJECT

public:
    LabelRegion(QWidget *parent = 0);
    ~LabelRegion();

Q_SIGNALS:
	// ֪ͨ��ͼ��ϣ�������ȡͼƬ���ݸ�������;
	void signalCompleteCature(QPixmap catureImage);

private:
	void initWindow();
	void initStretchRect();
	void loadBackgroundPixmap();
	QRect getRect(const QPoint &beginPoint, const QPoint &endPoint);
	QRect getMoveRect();
	QRect getStretchRect();
	bool isPressPointInSelectRect(QPoint mousePressPoint);
	QRect getSelectRect();
	QPoint getMovePoint();
	StretchRectState getStrethRectState(QPoint point);
	void setStretchCursorStyle(StretchRectState stretchRectState);

	void drawCaptureImage();
	void drawStretchRect();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void paintEvent(QPaintEvent *event);

private:
	QPixmap m_loadPixmap, m_capturePixmap;
	int m_screenwidth;
	int m_screenheight;
	// ����ȷ��ѡ���������;
	QPoint m_beginPoint, m_endPoint, m_beginMovePoint, m_endMovePoint;
	QPainter m_painter;
	// ���浱ǰ��ͼ״̬;
	CaptureState m_currentCaptureState;
	// ��ǰѡ���������;
	QRect m_currentSelectRect;
	// ѡ�о���8������С����;
	QRect m_topLeftRect, m_topRightRect, m_bottomLeftRect, m_bottomRightRect;
	QRect m_leftCenterRect, m_topCenterRect, m_rightCenterRect, m_bottomCenterRect;
	// ��ǰ������ڶ���״̬;
	StretchRectState m_stretchRectState;
};
