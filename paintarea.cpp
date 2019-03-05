#include "paintarea.h"

PaintArea::PaintArea(AntiCollisionArea* pAntiArea, int nWidth, int nHeight)
:m_pAntiArea(pAntiArea),m_nWidth(nWidth),m_nHeight(nHeight)
{
    setPalette(QPalette(Qt:: white));
    setAutoFillBackground(true);
    setMinimumSize(nWidth,nHeight);
    InitArea();
}

PaintArea::~PaintArea()
{
    delete m_pPixmap;
    delete m_pPan;
}

//e没有用到也不能去除，否则不显示画面
void PaintArea::InitArea()
{
    m_pPainter=new QPainter;
    m_pPixmap=new QPixmap(size());
    m_pPan=new Pan(this,m_nWidth,m_nHeight);//new时还没有生成m_pPainter
    m_pPan->SetPainter(m_pPainter);
    m_bZoom=false;
}

void PaintArea::paintEvent(QPaintEvent *e)
{
    m_pPainter=new QPainter(this);//必须在此函数中进行，否则无法绘画,在QScrollArea中用viewport()
    m_pPan->SetPainter(m_pPainter);
    m_pPainter->setRenderHint(QPainter::Antialiasing,true);//反走样
    m_pPainter->begin(this);
    m_pPan->Draw();
    m_pPainter->end();

//    QPixmap *clearPix =new QPixmap(size());
//    m_pPixmap = clearPix;
//    QPainter *clearPaint=new QPainter;
//    m_pPainter=clearPaint;
//    m_pPan->SetPainter(m_pPainter);
//    m_pPainter->begin(m_pPixmap);
//    m_pPan->Draw();
//    m_pPainter->end();

//    QPainter painter(this);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform,true);//Pixmap反走样
//    painter.drawPixmap(QPoint(0,0),*m_pPixmap);
}


void PaintArea::ZoomInOut(double dZoom, QPoint ptMain)
{
    double dOldZoom = m_pPan->GetZoom();
    if (dZoom < m_pPan->m_dMinZoom)
        dZoom = m_pPan->m_dMinZoom;
    if (dZoom > m_pPan->m_dMaxZoom)
        dZoom = m_pPan->m_dMaxZoom;
    m_pPan->SetZoom(dZoom);

    int nOrgX=int(ptMain.x()/dOldZoom*ORIGNAL_ZOOM);
    int nOrgY=int(ptMain.y()/dOldZoom*ORIGNAL_ZOOM);
    int nNewX=int(ptMain.x()/dOldZoom*dZoom);
    int nNewY=int(ptMain.y()/dOldZoom*dZoom);

    int width=m_pPan->GetWidth();
    int height=m_pPan->GetHeight();

    resize(width,height);	//利用新的长、宽值对图片进行resize()操作
    QPoint ptOrg(nOrgX,nOrgY);
    QPoint ptNew(nNewX,nNewY);
    m_pAntiArea->SetWheelScroll(width,height,ptOrg,ptNew);
}

void PaintArea::wheelEvent(QWheelEvent *e)
{
    double dZoom=m_pPan->GetZoom()*(1+e->delta()/1200.0);
    ZoomInOut(dZoom,e->pos());
    e->accept();//否则滚动条会自动有所滚动
    update();
}

void PaintArea::mouseMoveEvent(QMouseEvent *event)
{
    if(m_nFlag==Qt::MidButton)
    {
        QPoint ptNow=event->pos();
        int nX=m_ptPress.x()-ptNow.x();
        int nY=m_ptPress.y()-ptNow.y();
        m_pAntiArea->SetMidBtnMove(nX,nY);
    }
}

void PaintArea::mousePressEvent(QMouseEvent *event)
{
    m_ptPress=event->pos();
    m_nFlag=-1;
    if(event->button()==Qt::MidButton)
    {
        m_nFlag=Qt::MidButton;
        this->setCursor(Qt::ClosedHandCursor);
    }
    if(event->button()==Qt::LeftButton)
    {
        m_nFlag=Qt::LeftButton;
        this->setCursor(Qt::CrossCursor);
        m_pPan->CellCenEccValue(m_ptPress);
    }
}

//释放鼠标时，鼠标形状为原型
void PaintArea::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(Qt::ArrowCursor);
}
