#include "labelregion.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QDebug>

// ѡ�о���8����ק��С���εĿ��;
#define STRETCH_RECT_WIDTH 20
#define STRETCH_RECT_HEIGHT 20

LabelRegion::LabelRegion(QWidget *parent)
    : QWidget(parent)
    , m_currentCaptureState(InitCapture)
{
    initWindow();
    initStretchRect();
//    loadBackgroundPixmap();
}

LabelRegion::~LabelRegion()
{

}

void LabelRegion::initWindow()
{
    this->setMouseTracking(true);
    // ���ڴ�������QQ��������ʼ����ʾ����㣬�������� Qt::WindowStaysOnTopHint;
//    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
//    setWindowState(Qt::WindowActive | Qt::WindowFullScreen);
    this->setMouseTracking(true);
}

void LabelRegion::initStretchRect()
{
    m_stretchRectState = NotSelect;
    m_topLeftRect = QRect(0 , 0 , 0 , 0);
    m_topRightRect = QRect(0, 0, 0, 0);
    m_bottomLeftRect = QRect(0, 0, 0, 0);
    m_bottomRightRect = QRect(0, 0, 0, 0);

    m_leftCenterRect = QRect(0, 0, 0, 0);
    m_topCenterRect = QRect(0, 0, 0, 0);
    m_rightCenterRect = QRect(0, 0, 0, 0);
    m_bottomCenterRect = QRect(0, 0, 0, 0);
}

// ��ȡ��ǰ��ĻͼƬ;
void LabelRegion::loadBackgroundPixmap()
{
    m_loadPixmap = QPixmap::grabWindow(QApplication::desktop()->winId()); //ץȡ��ǰ��Ļ��ͼƬ;
    m_screenwidth = m_loadPixmap.width();
    m_screenheight = m_loadPixmap.height();
}

void LabelRegion::mousePressEvent(QMouseEvent *event)
{
    m_stretchRectState = getStrethRectState(event->pos());
	if (event->button() == Qt::LeftButton)
	{
		if (m_currentCaptureState == InitCapture)
		{
			m_currentCaptureState = BeginCaptureImage;
			m_beginPoint = event->pos();
		}
		// �Ƿ��������С������;
		else if (m_stretchRectState != NotSelect)
		{
			m_currentCaptureState = BeginMoveStretchRect;
			// ��ǰ������϶�ѡ��������ʱ,������굱ǰ״̬;
			setStretchCursorStyle(m_stretchRectState);
			m_beginMovePoint = event->pos();
		}
		// �Ƿ���ѡ�еľ�����;
		else if (isPressPointInSelectRect(event->pos()))
		{
			m_currentCaptureState = BeginMoveCaptureArea;
			m_beginMovePoint = event->pos();
		}
	}

    return QWidget::mousePressEvent(event);
}

void LabelRegion::mouseMoveEvent(QMouseEvent* event)
{
    if (m_currentCaptureState == BeginCaptureImage)
    {
        m_endPoint = event->pos();
        update();
    }
    else if (m_currentCaptureState == BeginMoveCaptureArea)
    {
        m_endMovePoint = event->pos();
        update();
    }
	else if (m_currentCaptureState == BeginMoveStretchRect)
	{
		m_endMovePoint = event->pos();
		update();
		// ��ǰ������϶�ѡ��������ʱ,�����δֹͣ�ƶ�ǰ��һֱ������굱ǰ״̬;
		return QWidget::mouseMoveEvent(event);
	}


    // ��������Ƿ���ѡ�����������������ʽ;
    StretchRectState stretchRectState = getStrethRectState(event->pos());
    if (stretchRectState != NotSelect)
    {
        setStretchCursorStyle(stretchRectState);
    }
    else if (isPressPointInSelectRect(event->pos()))
    {
        setCursor(Qt::SizeAllCursor);
    }
    else if (!isPressPointInSelectRect(event->pos()) && m_currentCaptureState != BeginMoveCaptureArea)
    {
        setCursor(Qt::ArrowCursor);
    }

    return QWidget::mouseMoveEvent(event);
}

void LabelRegion::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_currentCaptureState == BeginCaptureImage)
    {
        m_currentCaptureState = FinishCaptureImage;
        m_endPoint = event->pos();
        update();
    }
    else if (m_currentCaptureState == BeginMoveCaptureArea)
    {
        m_currentCaptureState = FinishMoveCaptureArea;
        m_endMovePoint = event->pos();
        update();
    }
    else if (m_currentCaptureState == BeginMoveStretchRect)
    {
        m_currentCaptureState = FinishMoveStretchRect;
        m_endMovePoint = event->pos();
        update();
    }

    return QWidget::mouseReleaseEvent(event);
}

// ��ǰ��������Ƿ���ѡȡ�ľ���������;
bool LabelRegion::isPressPointInSelectRect(QPoint mousePressPoint)
{
    QRect selectRect = getRect(m_beginPoint, m_endPoint);
    if (selectRect.contains(mousePressPoint))
    {
        return true;
    }

    return false;
}

void LabelRegion::paintEvent(QPaintEvent *event)
{
    m_painter.begin(this);          //�����ػ�;

    QColor shadowColor = QColor(0, 0, 0, 120);                      //��Ӱ��ɫ����;
    m_painter.setPen(QPen(QColor(0 , 174 , 255), 1, Qt::SolidLine, Qt::FlatCap));    //���û���;
    m_painter.drawPixmap(0, 0, m_loadPixmap);                       //������ͼƬ����������;
    m_painter.fillRect(m_loadPixmap.rect(), shadowColor);           //��Ӱ��Ч��;

    switch (m_currentCaptureState)
    {
    case InitCapture:
        break;
    case BeginCaptureImage:
    case FinishCaptureImage:
    case BeginMoveCaptureArea:
    case FinishMoveCaptureArea:
    case BeginMoveStretchRect:
    case FinishMoveStretchRect:
        m_currentSelectRect = getSelectRect();
        drawCaptureImage();
    default:
        break;
    }

    m_painter.end();  //�ػ����;
}

// ���ݵ�ǰ��ȡ״̬��ȡ��ǰѡ�еĽ�ͼ����;
QRect LabelRegion::getSelectRect()
{
    if (m_currentCaptureState == BeginCaptureImage || m_currentCaptureState == FinishCaptureImage)
    {
        return getRect(m_beginPoint, m_endPoint);
    }
    else if (m_currentCaptureState == BeginMoveCaptureArea || m_currentCaptureState == FinishMoveCaptureArea)
    {
        return getMoveRect();
    }
    else if (m_currentCaptureState == BeginMoveStretchRect || m_currentCaptureState == FinishMoveStretchRect)
    {
        return getStretchRect();
    }

    return QRect(0, 0, 0, 0);
}

// ���Ƶ�ǰѡ�еĽ�ͼ����;
void LabelRegion::drawCaptureImage()
{
    m_capturePixmap = m_loadPixmap.copy(m_currentSelectRect);
    m_painter.drawPixmap(m_currentSelectRect.topLeft(), m_capturePixmap);
    m_painter.drawRect(m_currentSelectRect);
    drawStretchRect();
}

// ����ѡ�о��θ���ק��С����;
void LabelRegion::drawStretchRect()
{
    QColor color = QColor(0, 174, 255);
    // �ĸ�������;
    QPoint topLeft = m_currentSelectRect.topLeft();
    QPoint topRight = m_currentSelectRect.topRight();
    QPoint bottomLeft = m_currentSelectRect.bottomLeft();
    QPoint bottomRight = m_currentSelectRect.bottomRight();
    // �������м������;
    QPoint leftCenter = QPoint(topLeft.x(), (topLeft.y() + bottomLeft.y()) / 2);
    QPoint topCenter = QPoint((topLeft.x() + topRight.x()) / 2, topLeft.y());
    QPoint rightCenter = QPoint(topRight.x(), leftCenter.y());
    QPoint bottomCenter = QPoint(topCenter.x(), bottomLeft.y());

    m_topLeftRect = QRect(topLeft.x() - STRETCH_RECT_WIDTH / 2, topLeft.y() - STRETCH_RECT_HEIGHT / 2, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    m_topRightRect = QRect(topRight.x() - STRETCH_RECT_WIDTH / 2, topRight.y() - STRETCH_RECT_HEIGHT / 2, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    m_bottomLeftRect = QRect(bottomLeft.x() - STRETCH_RECT_WIDTH / 2, bottomLeft.y() - STRETCH_RECT_HEIGHT / 2, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    m_bottomRightRect = QRect(bottomRight.x() - STRETCH_RECT_WIDTH / 2, bottomRight.y() - STRETCH_RECT_HEIGHT / 2, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);

    m_leftCenterRect = QRect(leftCenter.x() - STRETCH_RECT_WIDTH / 2, leftCenter.y() - STRETCH_RECT_HEIGHT / 2, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    m_topCenterRect = QRect(topCenter.x() - STRETCH_RECT_WIDTH / 2, topCenter.y() - STRETCH_RECT_HEIGHT / 2, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    m_rightCenterRect = QRect(rightCenter.x() - STRETCH_RECT_WIDTH / 2, rightCenter.y() - STRETCH_RECT_HEIGHT / 2, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    m_bottomCenterRect = QRect(bottomCenter.x() - STRETCH_RECT_WIDTH / 2, bottomCenter.y() - STRETCH_RECT_HEIGHT / 2, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);

    m_painter.fillRect(m_topLeftRect, color);
    m_painter.fillRect(m_topRightRect, color);
    m_painter.fillRect(m_bottomLeftRect, color);
    m_painter.fillRect(m_bottomRightRect, color);
    m_painter.fillRect(m_leftCenterRect, color);
    m_painter.fillRect(m_topCenterRect, color);
    m_painter.fillRect(m_rightCenterRect, color);
    m_painter.fillRect(m_bottomCenterRect, color);
}

void LabelRegion::keyPressEvent(QKeyEvent *event)
{
    // Esc ���˳���ͼ;
    if (event->key() == Qt::Key_Escape)
    {
        close();
    }
    // Eeter����ɽ�ͼ;
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        signalCompleteCature(m_capturePixmap);
        close();
    }
}

// ����beginPoint , endPoint ��ȡ��ǰѡ�еľ���;
QRect LabelRegion::getRect(const QPoint &beginPoint, const QPoint &endPoint)
{
    int x, y, width, height;
    width = qAbs(beginPoint.x() - endPoint.x());
    height = qAbs(beginPoint.y() - endPoint.y());
    x = beginPoint.x() < endPoint.x() ? beginPoint.x() : endPoint.x();
    y = beginPoint.y() < endPoint.y() ? beginPoint.y() : endPoint.y();

    QRect selectedRect = QRect(x, y, width, height);
    // �������Ϊ��ʱ������ͼ����;
    // ���Կ�QQ��ͼ����ѡȡ��ͼ����Ϊ��ʱĬ��Ϊ2;
    if (selectedRect.width() == 0)
    {
        selectedRect.setWidth(1);
    }
    if (selectedRect.height() == 0)
    {
        selectedRect.setHeight(1);
    }

    return selectedRect;
}

// ��ȡ�ƶ���,��ǰѡ�еľ���;
QRect LabelRegion::getMoveRect()
{
    // ͨ��getMovePoint�����ȼ�鵱ǰ�Ƿ��ƶ�������Ļ;
    QPoint movePoint = getMovePoint();
    QPoint beginPoint = m_beginPoint + movePoint;
    QPoint endPoint = m_endPoint + movePoint;
    // �����ƶ�ѡ��ʱ���µ�ǰm_beginPoint , m_endPoint,��ֹ��һ�β���ʱ��ȡ��ͼƬ������;
    if (m_currentCaptureState == FinishMoveCaptureArea)
    {
        m_beginPoint = beginPoint;
        m_endPoint = endPoint;
        m_beginMovePoint = QPoint(0, 0);
        m_endMovePoint = QPoint(0, 0);
    }
    return getRect(beginPoint, endPoint);
}

QPoint LabelRegion::getMovePoint()
{
    QPoint movePoint = m_endMovePoint - m_beginMovePoint;
    QRect currentRect = getRect(m_beginPoint, m_endPoint);
    // ��鵱ǰ�Ƿ��ƶ�������Ļ;

    //�ƶ�ѡ���Ƿ񳬳���Ļ��߽�;
    if (currentRect.topLeft().x() + movePoint.x() < 0)
    {
        movePoint.setX(0 - currentRect.topLeft().x());
    }
    //�ƶ�ѡ���Ƿ񳬳���Ļ�ϱ߽�;
    if (currentRect.topLeft().y() + movePoint.y() < 0)
    {
        movePoint.setY(0 - currentRect.topLeft().y());
    }
    //�ƶ�ѡ���Ƿ񳬳���Ļ�ұ߽�;
    if (currentRect.bottomRight().x() + movePoint.x() > m_screenwidth)
    {
        movePoint.setX(m_screenwidth - currentRect.bottomRight().x());
    }
    //�ƶ�ѡ���Ƿ񳬳���Ļ�±߽�;
    if (currentRect.bottomRight().y() + movePoint.y() > m_screenheight)
    {
        movePoint.setY(m_screenheight - currentRect.bottomRight().y());
    }

    return movePoint;
}

// ��ȡ��ǰ���λ����һ����ק����;
StretchRectState LabelRegion::getStrethRectState(QPoint point)
{
    StretchRectState stretchRectState = NotSelect;
    if (m_topLeftRect.contains(point))
    {
        stretchRectState = TopLeftRect;
    }
    else if (m_topCenterRect.contains(point))
    {
        stretchRectState = TopCenterRect;
    }
    else if (m_topRightRect.contains(point))
    {
        stretchRectState = TopRightRect;
    }
    else if (m_rightCenterRect.contains(point))
    {
        stretchRectState = RightCenterRect;
    }
    else if (m_bottomRightRect.contains(point))
    {
        stretchRectState = BottomRightRect;
    }
    else if (m_bottomCenterRect.contains(point))
    {
        stretchRectState = BottomCenterRect;
    }
    else if (m_bottomLeftRect.contains(point))
    {
        stretchRectState = BottomLeftRect;
    }
    else if (m_leftCenterRect.contains(point))
    {
        stretchRectState = LeftCenterRect;
    }

    return stretchRectState;
}

// �������ͣ����ק���㴦����ʽ;
void LabelRegion::setStretchCursorStyle(StretchRectState stretchRectState)
{
    switch (stretchRectState)
    {
    case NotSelect:
        setCursor(Qt::ArrowCursor);
        break;
    case TopLeftRect:
    case BottomRightRect:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case TopRightRect:
    case BottomLeftRect:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case LeftCenterRect:
    case RightCenterRect:
        setCursor(Qt::SizeHorCursor);
        break;
    case TopCenterRect:
    case BottomCenterRect:
        setCursor(Qt::SizeVerCursor);
        break;
    default:
        break;
    }
}

// ��ȡ��ק��ľ���ѡ������;
QRect LabelRegion::getStretchRect()
{
    QRect stretchRect;
    QRect currentRect = getRect(m_beginPoint, m_endPoint);
    switch (m_stretchRectState)
    {
    case NotSelect:
        stretchRect = getRect(m_beginPoint, m_endPoint);
        break;
    case TopLeftRect:
        {
            stretchRect = getRect(currentRect.bottomRight(), m_endMovePoint);
        }
        break;
    case TopRightRect:
        {
            QPoint beginPoint = QPoint(currentRect.topLeft().x(), m_endMovePoint.y());
            QPoint endPoint = QPoint(m_endMovePoint.x(), currentRect.bottomRight().y());
            stretchRect = getRect(beginPoint, endPoint);
        }
        break;
    case BottomLeftRect:
        {
            QPoint beginPoint = QPoint(m_endMovePoint.x() , currentRect.topLeft().y());
            QPoint endPoint = QPoint(currentRect.bottomRight().x(), m_endMovePoint.y());
            stretchRect = getRect(beginPoint, endPoint);
        }
        break;
    case BottomRightRect:
        {
            stretchRect = getRect(currentRect.topLeft(), m_endMovePoint);
        }
        break;
    case LeftCenterRect:
        {
            QPoint beginPoint = QPoint(m_endMovePoint.x(), currentRect.topLeft().y());
            stretchRect = getRect(beginPoint, currentRect.bottomRight());
        }
        break;
    case TopCenterRect:
    {
        QPoint beginPoint = QPoint(currentRect.topLeft().x(), m_endMovePoint.y());
        stretchRect = getRect(beginPoint, currentRect.bottomRight());
    }
        break;
    case RightCenterRect:
    {
        QPoint endPoint = QPoint(m_endMovePoint.x(), currentRect.bottomRight().y());
        stretchRect = getRect(currentRect.topLeft(), endPoint);
    }
        break;
    case BottomCenterRect:
    {
        QPoint endPoint = QPoint(currentRect.bottomRight().x(), m_endMovePoint.y());
        stretchRect = getRect(currentRect.topLeft(), endPoint);
    }
        break;
    default:
    {
        stretchRect = getRect(m_beginPoint , m_endPoint );
    }
        break;
    }

    // �϶��������� m_beginPoint , m_endPoint;
    if (m_currentCaptureState == FinishMoveStretchRect)
    {
        m_beginPoint = stretchRect.topLeft();
        m_endPoint = stretchRect.bottomRight();
    }

    return stretchRect;
}
