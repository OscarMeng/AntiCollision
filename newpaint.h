#ifndef NEWPAINT_H
#define NEWPAINT_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPainter>
#include "newantiarea.h"
#include "newpan.h"
#include "newcell.h"

class NewPan;
class NewAntiArea;
class NewPaint : public QWidget
{
    Q_OBJECT
public:
    explicit NewPaint(NewAntiArea* pAnti,int nWidth,int nHeight);
    virtual ~NewPaint();
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
    NewAntiArea* m_pNewAnti;
    NewPan*      m_pNewPan;
    QPainter*    m_pPainter;
    int          m_nWidth;
    int          m_nHeight;
    QPoint       m_ptPress;
    int          m_nFlag;
signals:

public slots:
};

#endif // NEWPAINT_H
