#ifndef PAINTAREA_H
#define PAINTAREA_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QColor>
#include <QPixmap>
#include <QPoint>
#include <QPalette>
#include "anticollisionarea.h"
#include "pan.h"
#include "cell.h"

class Pan;
class AntiCollisionArea;
class PaintArea : public QWidget
{
    Q_OBJECT
public:
    explicit PaintArea(AntiCollisionArea* pAntiArea,int nWidth,int nHeight);
    virtual ~PaintArea();
protected:
    void InitArea();
    void ZoomInOut(double dZoom, QPoint ptMain);
private:
    void paintEvent(QPaintEvent *e);
    void wheelEvent(QWheelEvent *e);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
public:

public:
    AntiCollisionArea* m_pAntiArea;
    Pan*      m_pPan;
    QPixmap*  m_pPixmap;
    QPainter* m_pPainter;
    QPoint    m_ptPress;
    int       m_nWidth;
    int       m_nHeight;
    int       m_nFlag;
    bool      m_bZoom;
signals:

public slots:

};

#endif // PAINTAREA_H
