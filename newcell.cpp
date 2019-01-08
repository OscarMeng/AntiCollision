#include "newcell.h"

NewCell::NewCell(int nID, bool bStatus, double dCenterX, double dCenterY)
: m_nID(nID), m_bStatus(bStatus), m_dCenterX(dCenterX), m_dCenterY(dCenterY)
{
    m_nCurrentCenSlice=0;
    m_nCurrentEccSlice=0;
    m_dEccDegree=0;
    m_dCenRadian=0;
    m_nCenRadSlice=0;
    m_dEccRadian=0;
    m_nEccRadSlice=0;
    m_nStartRadSlice=0;
    m_nRunRadSlice=0;
    m_nWavePos=0;
    m_nRunStyle=RUN_COM;//一般运行方式
    m_ptEccR1=QPointF(-16.0,0.0);
    m_ptEccR2=QPointF(14.5,0.0);

    for(int i=0;i<WAVE_NUM;i++)
    {
        m_nCenWave[i]=BYTE_NULL;
        m_nEccWave[i]=BYTE_NULL;
    }
}

NewCell::~NewCell()
{

}

//运行的点最终半径和角度
void NewCell::InitCell(NewPan *pPan, double dRadius, double dRadian)
{
    m_pNewPan = pPan;
    m_dRunRadian = dRadian;
    if(dRadius<m_dMinR)
    {
        m_dRadius =m_dMinR;
        m_dStartRadian=acos(0);
    }
    else if(dRadius>m_dMaxR)
    {
        m_dRadius=m_dMaxR;
        m_dStartRadian=acos(1);
    }
    else
    {
        m_dRadius = dRadius;
        m_dStartRadian = acos((m_dRadius*m_dRadius + m_dCen*m_dCen - m_dEcc*m_dEcc)
                              /(2*m_dRadius*m_dCen));//三角形余弦定理
        m_dEccRadian = acos((m_dCen*m_dCen + m_dEcc*m_dEcc - m_dRadius*m_dRadius )
                            /(2*m_dCen*m_dEcc));//三角形余弦定理
    }
    //运行的单元，保证弧度大于0
    if(m_bStatus)
    {
        m_dCenRadian=m_dRunRadian-m_dStartRadian;
        if(m_dCenRadian<0)
        {
            m_dCenRadian+=2*PI;
        }
        m_nCenRadSlice=qRound(m_dCenRadian*SLICE_RATIO);
        m_nEccRadSlice=qRound(m_dEccRadian*SLICE_RATIO);
        m_nStartRadSlice=qRound(m_dStartRadian*SLICE_RATIO);
        m_nRunRadSlice=qRound(m_dRunRadian*SLICE_RATIO);
        //单元中心轴、偏心轴波形赋值,只赋到已有的值,处理碰撞时继续赋值
        for(int i=0;i<m_nCenRadSlice;i++)
        {
            m_nCenWave[i]=BYTE_RUN;
        }

        if(m_nEccRadSlice<=m_nCenRadSlice)
        {
            for(int i=0;i<m_nCenRadSlice-m_nEccRadSlice;i++)
            {
                m_nEccWave[i]=BYTE_STOP;
            }
            for(int j=m_nCenRadSlice-m_nEccRadSlice;j<m_nCenRadSlice;j++)
            {
                m_nEccWave[j]=BYTE_RUN;
            }
        }
        else
        {
            for(int j=0;j<m_nEccRadSlice;j++)
            {
                m_nEccWave[j]=BYTE_RUN;
            }
        }
    }
}

QRect NewCell::CalRect(double dX,double dY,double dR)
{
    return m_pNewPan->Radius2Rect(dX, dY, dR);
}

void NewCell::Draw()
{
    //画圆
    if(m_bStatus)
    {
        DrawArc();//画运行轨迹
        DrawCenEcc();//画展开的中心轴和偏心轴
        DrawTargetPos();//画目标点
    }
    else
    {
        m_pPainter->setPen(*(m_pNewPan->m_pSolidPen[COLOR_DISUSE]));
        m_pPainter->drawEllipse(m_rRect);//画未展开圆
        DrawUnexpanded();
    }
    //写单元数
    m_pPainter->setPen(*(m_pNewPan->m_pSolidPen[COLOR_NUM]));
    QFont font;
    font.setPointSize(10);
    m_pPainter->setFont(font);
    m_pPainter->drawText(CalRect(m_dCenterX,m_dCenterY,STOP_RADIUS),Qt::AlignCenter,QString::number(m_nID,10));
}

void NewCell::DrawArc()
{
    //画最后的落点所在的圆
    m_pPainter->setPen(*(m_pNewPan->m_pSolidPen[COLOR_CIRCLE]));
    m_pPainter->drawArc(m_rRect,0,360*16);
    //画运行的轨迹
    m_pPainter->setPen(*(m_pNewPan->m_pSolidPen[COLOR_RUN]));
    m_pPainter->drawPath(m_pathArc);
}

void NewCell::DrawCenEcc()
{
    //展开痕迹
    m_pPainter->setPen(*(m_pNewPan->m_pDddPen[COLOR_COORD]));
    m_pPainter->drawLine(m_pNewPan->Op2Vp(m_dCenterX,m_dCenterY),m_pNewPan->Op2Vp(m_ptEccR2));
    m_pPainter->drawLine(m_pNewPan->Op2Vp(m_ptEccR2),m_pNewPan->Op2Vp(m_ptEccR1));
    //画偏心圆
    m_pPainter->setPen(*(m_pNewPan->m_pDashPen[COLOR_CIRCLE]));
    m_pPainter->drawEllipse(CalRect(m_ptEccR2.x(),m_ptEccR2.y(),m_dEccR2));
    //偏心轴外形
    m_pPainter->setPen(*(m_pNewPan->m_pSolidPen[COLOR_SHAFT]));
    m_pPainter->drawPath(m_path);
}

void NewCell::DrawUnexpanded()
{
    //展开痕迹
    m_pPainter->setPen(*(m_pNewPan->m_pDddPen[COLOR_DISUSE]));
    m_pPainter->drawLine(m_pNewPan->Op2Vp(m_dCenterX,m_dCenterY),m_pNewPan->Op2Vp(m_ptEccR2));
    m_pPainter->drawLine(m_pNewPan->Op2Vp(m_ptEccR2),m_pNewPan->Op2Vp(m_ptEccR1));
    //画偏心圆
    m_pPainter->setPen(*(m_pNewPan->m_pDashPen[COLOR_DISUSE]));
    m_pPainter->drawEllipse(CalRect(m_ptEccR2.x(),m_ptEccR2.y(),m_dEccR2));
    //偏心轴外形
    m_pPainter->setPen(*(m_pNewPan->m_pSolidPen[COLOR_DISUSE]));
    m_pPainter->drawPath(m_path);
}

void NewCell::DrawTargetPos()
{
    //目标点
    m_pPainter->setPen(*(m_pNewPan->m_pSolidPen[COLOR_TARGET]));
    double dRadian=m_dRunRadian;
    QPointF p=QPointF(m_dCenterX+m_dRadius*cos(dRadian),m_dCenterY-m_dRadius*sin(dRadian));
    double d=1.0;
    QPointF p1=QPointF(p.x()-d,p.y()-d);
    QPointF p2=QPointF(p.x()+d,p.y()-d);
    QPointF p3=QPointF(p.x()-d,p.y()+d);
    QPointF p4=QPointF(p.x()+d,p.y()+d);
    m_pPainter->drawLine(m_pNewPan->Op2Vp(p1),m_pNewPan->Op2Vp(p4));
    m_pPainter->drawLine(m_pNewPan->Op2Vp(p2),m_pNewPan->Op2Vp(p3));
}

double NewCell::LengthByPoint(const QPointF pointF)
{
    return sqrt(pointF.x()*pointF.x()+pointF.y()*pointF.y());
}

double NewCell::LengthByPoint(const QPointF point1, const QPointF point2)
{
    return sqrt((point1.x()-point2.x())*(point1.x()-point2.x())+
                (point1.y()-point2.y())*(point1.y()-point2.y()));
}

void NewCell::CalCurrentRadSlice()
{
    int nCenSlice=0;
    int nEccSlice=0;
    if(m_bStatus)
    {
        for(int i=0;i<m_nWavePos;i++)
        {
            int n=m_nCenWave[i];
            int m=m_nEccWave[i];
            if(n!=BYTE_NULL)
            {
                nCenSlice+=n;
            }
            if(m!=BYTE_NULL)
            {
                nEccSlice+=m;
            }
        }
    }
    m_nCurrentCenSlice=nCenSlice;
    m_nCurrentEccSlice=nEccSlice;
}

QPointF NewCell::CalPointBySlice(int nCenSlice, int nEccSlice)
{
    //中心轴，偏心轴弧度
    double dCenRadian=double(nCenSlice)/SLICE_RATIO;
    double dEccRadian=double(nEccSlice)/SLICE_RATIO;
    //在偏心轴展开组成的三角形中，求展开半径，半径展开弧度
    double dRadius=sqrt(m_dCen*m_dCen+m_dEcc*m_dEcc
                 -2*m_dCen*m_dEcc*cos(dEccRadian));
    double dSpreadRadian=acos((dRadius*dRadius + m_dCen*m_dCen - m_dEcc*m_dEcc)
                              /(2*dRadius*m_dCen));//三角形余弦定理
    return QPointF(m_dCenterX+dRadius*cos(dCenRadian+dSpreadRadian),
                   m_dCenterY-dRadius*sin(dCenRadian+dSpreadRadian));
}

void NewCell::CreatePoint()
{
    m_rRect = CalRect(m_dCenterX,m_dCenterY,m_dRadius);//每重画一次，都要计算一次
    if((m_nRunStyle==RUN_PLAN||m_nRunStyle==RUN_LAST)&&m_bStatus)
    {
        CalCurrentRadSlice();
    }
    //中心轴转动的弧度
    double dCenRadian=double(m_nCurrentCenSlice)/SLICE_RATIO;
    //偏心轴外形矩形4点，中心3点
    m_ptEccR1=CalPointBySlice(m_nCurrentCenSlice,m_nCurrentEccSlice);
    m_ptEccR2=QPointF(m_dCenterX+m_dCen*cos(dCenRadian),
                      m_dCenterY-m_dCen*sin(dCenRadian));
    double dEccFabsRadian=asin(fabs(m_ptEccR2.y()-m_ptEccR1.y())
                               /LengthByPoint(m_ptEccR1,m_ptEccR2));
    double dEccRadian=0;//偏心轴在视图坐标中的弧度
    double dCX1=m_ptEccR1.x();
    double dCY1=m_ptEccR1.y();
    double dCX2=m_ptEccR2.x();
    double dCY2=m_ptEccR2.y();
    //偏心轴在视图坐标中的弧度，根据偏心轴两个圆弧中心点位置
    if(dCX1>=dCX2&&dCY1<=dCY2)
    {
        dEccRadian=dEccFabsRadian;
    }
    else if(dCX1<=dCX2&&dCY1<=dCY2)
    {
        dEccRadian=PI-dEccFabsRadian;
    }
    else if(dCX1<=dCX2&&dCY1>=dCY2)
    {
        dEccRadian=PI+dEccFabsRadian;
    }
    else
    {
        dEccRadian=2*PI-dEccFabsRadian;
    }
    m_dEccDegree=dEccRadian/PI*180.0;
}

void NewCell::CreatePath()
{
    CreatePoint();
    //偏心轴外形
    QPainterPath eccPath;
    //从圆弧的起点开始
    eccPath.arcMoveTo(m_pNewPan->Radius2Rect(m_ptEccR1.x(), m_ptEccR1.y(), m_dEccR1),
                      m_dEccDegree-90.0);
    //逆时针180°
    eccPath.arcTo(m_pNewPan->Radius2Rect(m_ptEccR1.x(), m_ptEccR1.y(), m_dEccR1),
                  m_dEccDegree-90.0,180.0);
    //需要顺时针内圆弧的度数dAngle°，才能与上个圆弧闭合，因为是内圆弧
    double dRadian=acos((m_dEccR2*m_dEccR2 + m_dEccR2*m_dEccR2 - 2*m_dEccR1*2*m_dEccR1)
                        /(2*m_dEccR2*m_dEccR2));//三角形余弦定理
    double dAngle=dRadian/PI*180.0;
    eccPath.arcTo(m_pNewPan->Radius2Rect(m_ptEccR2.x(), m_ptEccR2.y(), m_dEccR2),
                  m_dEccDegree+dAngle/2.0,-dAngle);
    eccPath.closeSubpath();
    m_path=eccPath;
    //画运行轨迹
    CreateArc();
}

void NewCell::CreateArc()
{
    //运行轨迹，中心轴、偏心轴根据碰撞处理进行运行展开
    QPainterPath arcPath;
    int nCenSlice=0;
    int nEccSlice=0;
    QPointF ptFiber=QPointF(m_dCenterX-m_dMinR,m_dCenterY);
    arcPath.moveTo(m_pNewPan->Op2Vp(ptFiber));
    if(m_nRunStyle==RUN_COM)
    {//一般运行状态
        //当前总共运行的弧度片进行分割
        for( ;nCenSlice<=m_nCurrentCenSlice||nEccSlice<=m_nCurrentEccSlice;
            nCenSlice++,nEccSlice++)
        {
            if(m_nCenRadSlice>=m_nEccRadSlice&&m_nCenRadSlice-nCenSlice<m_nEccRadSlice)
            {
                nEccSlice=nCenSlice-m_nCenRadSlice+m_nEccRadSlice;
            }
            else if(m_nCenRadSlice>=m_nEccRadSlice&&m_nCenRadSlice-nCenSlice>m_nEccRadSlice)
            {
                nEccSlice=0;
            }
            if(nCenSlice>m_nCurrentCenSlice)
                nCenSlice=m_nCurrentCenSlice;
            if(nEccSlice>m_nCurrentEccSlice)
                nEccSlice=m_nCurrentEccSlice;
            ptFiber=CalPointBySlice(nCenSlice,nEccSlice);
            arcPath.lineTo(m_pNewPan->Op2Vp(ptFiber));
        }
    }
    else if(m_nRunStyle==RUN_PLAN||m_nRunStyle==RUN_LAST)
    {//处理碰撞状态
        for(int nC=0,nE=0;nC<m_nWavePos&&nE<m_nWavePos;nC++,nE++)
        {
            if(m_nCenWave[nC]!=BYTE_NULL)
                nCenSlice+=m_nCenWave[nC];
            if(m_nEccWave[nE]!=BYTE_NULL)
                nEccSlice+=m_nEccWave[nE];
            if(nCenSlice>m_nCenRadSlice)
                nCenSlice=m_nCenRadSlice;
            if(nEccSlice>m_nEccRadSlice)
                nEccSlice=m_nEccRadSlice;
            ptFiber=CalPointBySlice(nCenSlice,nEccSlice);
            arcPath.lineTo(m_pNewPan->Op2Vp(ptFiber));
        }
    }
    m_pathArc=arcPath;
}

