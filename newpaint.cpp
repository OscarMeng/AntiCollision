#include "newpaint.h"

NewPaint::NewPaint(NewAntiArea *pAnti, int nWidth, int nHeight)
:m_pNewAnti(pAnti),m_nWidth(nWidth),m_nHeight(nHeight)
{
    setPalette(QPalette(Qt:: black));
    setAutoFillBackground(true);
    setMinimumSize(nWidth,nHeight);
    InitArea();
}

NewPaint::~NewPaint()
{
    delete m_pNewPan;
}

void NewPaint::InitArea()
{
    m_pNewPan=new NewPan(this,m_nWidth,m_nHeight);//new时还没有生成m_pPainter
}

void NewPaint::ZoomInOut(double dZoom, QPoint ptMain)
{
    double dOldZoom = m_pNewPan->GetZoom();
    if (dZoom < m_pNewPan->m_dMinZoom)
        dZoom = m_pNewPan->m_dMinZoom;
    if (dZoom > m_pNewPan->m_dMaxZoom)
        dZoom = m_pNewPan->m_dMaxZoom;
    m_pNewPan->SetZoom(dZoom);

    int nOrgX=int(ptMain.x()/dOldZoom*ORIGNAL_ZOOM);
    int nOrgY=int(ptMain.y()/dOldZoom*ORIGNAL_ZOOM);
    int nNewX=int(ptMain.x()/dOldZoom*dZoom);
    int nNewY=int(ptMain.y()/dOldZoom*dZoom);

    int width=m_pNewPan->GetWidth();
    int height=m_pNewPan->GetHeight();

    resize(width,height);	//利用新的长、宽值对图片进行resize()操作
    QPoint ptOrg(nOrgX,nOrgY);
    QPoint ptNew(nNewX,nNewY);
    m_pNewAnti->SetWheelScroll(width,height,ptOrg,ptNew);
}

void NewPaint::paintEvent(QPaintEvent *e)
{
    m_pPainter=new QPainter(this);//必须在此函数中进行，否则无法绘画,在QScrollArea中用viewport()
    m_pNewPan->SetPainter(m_pPainter);
    m_pPainter->setRenderHint(QPainter::Antialiasing,true);//反走样
    m_pPainter->begin(this);
    m_pNewPan->Draw();
    m_pPainter->end();
}

void NewPaint::wheelEvent(QWheelEvent *e)
{
    double dZoom=m_pNewPan->GetZoom()*(1+e->delta()/1200.0);
    ZoomInOut(dZoom,e->pos());
    e->accept();//否则滚动条会自动有所滚动
    update();
}

void NewPaint::mouseMoveEvent(QMouseEvent *event)
{
    if(m_nFlag==Qt::MidButton)
    {
        QPoint ptNow=event->pos();
        int nX=m_ptPress.x()-ptNow.x();
        int nY=m_ptPress.y()-ptNow.y();
        m_pNewAnti->SetMidBtnMove(nX,nY);
    }
}

void NewPaint::mousePressEvent(QMouseEvent *event)
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
        m_pNewPan->CellCenEccValue(m_ptPress);
    }
}

void NewPaint::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(Qt::ArrowCursor);
}
