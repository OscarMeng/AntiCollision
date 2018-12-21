#include "cell.h"
///
/// \brief Cell::Cell
/// \param nID
/// \param nStatus
/// \param dCenterX
/// \param dCenterY
///
Cell::Cell(int nID, int nStatus, double dCenterX, double dCenterY)
: m_nID(nID), m_nStatus(nStatus), m_dCenterX(dCenterX), m_dCenterY(dCenterY)
{
    m_nCurrentCenSlice=0;
    m_nCurrentEccSlice=0;
    m_dChamferDegree=0;
    m_dCenRadian=0;
    m_nCenRadSlice=0;
    m_dEccRadian=0;
    m_nEccRadSlice=0;
    m_nStartRadSlice=0;
    m_nRunRadSlice=0;
    m_nWavePos=0;
    m_bRunStatus=true;
    m_dREcc=MAX_RADIUS;
    m_ptCenP1=QPointF(-2.75,-3.9);
    m_ptCenP2=QPointF(-2.75,-6.9);
    m_ptCenP3=QPointF(10.25,-6.9);
    m_ptCenP4=QPointF(10.25,-3.9);
    m_ptEccP1=QPointF(0,0);
    m_ptEccP2=QPointF(0,0);
    m_ptEccP3=QPointF(0,0);
    m_ptEccP4=QPointF(0,0);
    m_ptEccR1=QPointF(0,0);
    m_ptEccR2=QPointF(0,0);
    m_ptEccR3=QPointF(0,0);
    m_dLenghtP1=LengthByPoint(m_ptCenP1);
    m_dLenghtP2=LengthByPoint(m_ptCenP2);
    m_dLenghtP3=LengthByPoint(m_ptCenP3);
    m_dLenghtP4=LengthByPoint(m_ptCenP4);
    m_dRadianP1=atan(m_ptCenP1.y()/m_ptCenP1.x())+PI;
    m_dRadianP2=atan(m_ptCenP2.y()/m_ptCenP2.x())+PI;
    m_dRadianP3=atan(m_ptCenP3.y()/m_ptCenP3.x())+2*PI;
    m_dRadianP4=atan(m_ptCenP4.y()/m_ptCenP4.x())+2*PI;
    for(int i=0;i<WAVE_NUM;i++)
    {
        m_nCenWave[i]=BYTE_NULL;
        m_nEccWave[i]=BYTE_NULL;
    }
}

Cell::~Cell()
{

}

//运行的点最终半径和角度
void Cell::InitCell(Pan *pPan, double dRadius, double dRadian)
{
    m_pPan = pPan;
    m_dRunRadian = dRadian;
    if(dRadius<=0)
    {
        m_dRadius =0;
        m_dStartRadian=acos(0);
    }
    else if(dRadius>=2*m_dRunEcc)
    {
        m_dRadius=2*m_dRunEcc;
        m_dStartRadian=acos(1);
    }
    else
    {
        m_dRadius = dRadius;
        m_dStartRadian = acos((m_dRadius*m_dRadius + m_dRunEcc*m_dRunEcc - m_dRunEcc*m_dRunEcc)
                              /(2 * m_dRadius*m_dRunEcc));//三角形余弦定理
    }
    //保证弧度大于0
    if(m_nStatus==RUN_STATUS)
    {
        m_dCenRadian=m_dRunRadian-m_dStartRadian;
        m_nCenRadSlice=qRound(m_dCenRadian*SLICE_RATIO);
        m_dEccRadian=PI-2*m_dStartRadian;
        m_nEccRadSlice=qRound(m_dEccRadian*SLICE_RATIO);
        m_nStartRadSlice=qRound(m_dStartRadian*SLICE_RATIO);
        m_nRunRadSlice=qRound(m_dRunRadian*SLICE_RATIO);
        for(int i=0;i<m_nCenRadSlice;i++)
        {
            m_nCenWave[i]=BYTE_RUN;
        }
        for(int j=0;j<m_nEccRadSlice;j++)
        {
            m_nEccWave[j]=BYTE_RUN;
        }
    }
}

QRect Cell::CalRect()
{
    return m_pPan->Radius2Rect(m_dCenterX, m_dCenterY, m_dRadius);
}

void Cell::Draw()
{
    //画圆
    m_rRect = CalRect();//每重画一次，都要计算一次
    if(m_nStatus==RUN_STATUS)
    {
        DrawArc();//画运行轨迹
        DrawCenEcc();//画展开的中心轴和偏心轴
        DrawTargetPos();//画目标点
    }
    else
    {
        m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_DISUSE]));
        m_pPainter->drawEllipse(m_rRect);//画未展开圆
        DrawUnexpanded();
    }
    //写单元数
    m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_NUM]));
    QFont font;
    font.setPointSize(10);
    m_pPainter->setFont(font);
    m_pPainter->drawText(m_rRect,Qt::AlignCenter,QString::number(m_nID,10));
}

void Cell::DrawArc()
{
    //画最后的落点所在的圆
    m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_CIRCLE]));
    m_pPainter->drawArc(m_rRect,0,360*16);
    //画运行的轨迹
    m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_RUN]));
    m_pPainter->drawPath(m_pathArc);
}

void Cell::DrawCenEcc()
{
    //展开痕迹
    m_pPainter->setPen(*(m_pPan->m_pDddPen[COLOR_COORD]));
    m_pPainter->drawLine(m_pPan->Op2Vp(m_dCenterX,m_dCenterY),m_pPan->Op2Vp(m_ptEccR2));
    m_pPainter->drawLine(m_pPan->Op2Vp(m_ptEccR2),m_pPan->Op2Vp(m_ptEccR1));

    //中心轴外形    偏心轴外形
    m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_SHAFT]));
    m_pPainter->drawPath(m_pathCen);
    m_pPainter->drawPath(m_pathEcc);
}

void Cell::DrawUnexpanded()
{
    //展开痕迹
    m_pPainter->setPen(*(m_pPan->m_pDddPen[COLOR_DISUSE]));
    m_pPainter->drawLine(m_pPan->Op2Vp(m_dCenterX,m_dCenterY),m_pPan->Op2Vp(m_ptEccR2));
    m_pPainter->drawLine(m_pPan->Op2Vp(m_ptEccR2),m_pPan->Op2Vp(m_ptEccR1));
    //中心轴外形    偏心轴外形
    m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_DISUSE]));
    m_pPainter->drawPath(m_pathCen);
    m_pPainter->drawPath(m_pathEcc);
}

void Cell::DrawTargetPos()
{
    //目标点
    m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_TARGET]));
    double dRadian=m_dRunRadian;
    QPointF p=QPointF(m_dCenterX+m_dRadius*cos(dRadian),m_dCenterY-m_dRadius*sin(dRadian));
    double d=1.0;
    QPointF p1=QPointF(p.x()-d,p.y()-d);
    QPointF p2=QPointF(p.x()+d,p.y()-d);
    QPointF p3=QPointF(p.x()-d,p.y()+d);
    QPointF p4=QPointF(p.x()+d,p.y()+d);
    m_pPainter->drawLine(m_pPan->Op2Vp(p1),m_pPan->Op2Vp(p4));
    m_pPainter->drawLine(m_pPan->Op2Vp(p2),m_pPan->Op2Vp(p3));
}

double Cell::LengthByPoint(const QPointF pointF)
{
    return sqrt(pointF.x()*pointF.x()+pointF.y()*pointF.y());
}

double Cell::LengthByPoint(const QPointF point1, const QPointF point2)
{
    return sqrt((point1.x()-point2.x())*(point1.x()-point2.x())+
                (point1.y()-point2.y())*(point1.y()-point2.y()));
}

void Cell::CalCurrentRadSlice()
{
    int nCenSlice=0;
    int nEccSlice=0;
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
    m_nCurrentCenSlice=nCenSlice;
    m_nCurrentEccSlice=nEccSlice;
}

QPointF Cell::CalPointBySlice(int nCenSlice, int nEccSlice)
{
    //中心轴，偏心轴弧度
    double dCenRadian=double(nCenSlice)/SLICE_RATIO;
    double dEccRadian=double(nEccSlice)/SLICE_RATIO;
    //在偏心轴展开组成的三角形中，求展开半径，半径展开弧度
    double dRadius=sqrt(m_dRunEcc*m_dRunEcc+m_dRunEcc*m_dRunEcc
                 -2*m_dRunEcc*m_dRunEcc*cos(dEccRadian));
    double dSpreadRadian=(PI-dEccRadian)/2.0;
    return QPointF(m_dCenterX+dRadius*cos(dCenRadian+dSpreadRadian),
                   m_dCenterY-dRadius*sin(dCenRadian+dSpreadRadian));
}

void Cell::CreatePoint()
{
    if(!m_bRunStatus&&m_nStatus==RUN_STATUS)
    {
        CalCurrentRadSlice();
    }
    //中心轴外形矩形4点
    //中心轴转动的弧度
    double dCenRadian=double(m_nCurrentCenSlice)/SLICE_RATIO;
    double dPA1=dCenRadian+m_dRadianP1;
    double dPA2=dCenRadian+m_dRadianP2;
    double dPA3=dCenRadian+m_dRadianP3;
    double dPA4=dCenRadian+m_dRadianP4;
    m_ptCenP1=QPointF(m_dCenterX+m_dLenghtP1*cos(dPA1),m_dCenterY-m_dLenghtP1*sin(dPA1));
    m_ptCenP2=QPointF(m_dCenterX+m_dLenghtP2*cos(dPA2),m_dCenterY-m_dLenghtP2*sin(dPA2));
    m_ptCenP3=QPointF(m_dCenterX+m_dLenghtP3*cos(dPA3),m_dCenterY-m_dLenghtP3*sin(dPA3));
    m_ptCenP4=QPointF(m_dCenterX+m_dLenghtP4*cos(dPA4),m_dCenterY-m_dLenghtP4*sin(dPA4));

    //偏心轴外形矩形4点，中心3点
    double dCenDistance=m_dEcc-2*m_dREcc-m_dRunEcc;
    m_ptEccR1=CalPointBySlice(m_nCurrentCenSlice,m_nCurrentEccSlice);
    m_ptEccR2=QPointF(m_dCenterX+m_dRunEcc*cos(dCenRadian),
                      m_dCenterY-m_dRunEcc*sin(dCenRadian));
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
    double dChamferRadian=dEccRadian-PI/2.0;
    m_dChamferDegree=dChamferRadian/PI*180.0;
    m_ptEccR3=QPointF(m_ptEccR2.x()-dCenDistance*cos(dEccRadian),
                      m_ptEccR2.y()+dCenDistance*sin(dEccRadian));
    m_ptEccP1=QPointF(m_ptEccR1.x()-m_dREcc*cos(dChamferRadian),
                      m_ptEccR1.y()+m_dREcc*sin(dChamferRadian));
    m_ptEccP2=QPointF(m_ptEccR1.x()+m_dREcc*cos(dChamferRadian),
                      m_ptEccR1.y()-m_dREcc*sin(dChamferRadian));
    m_ptEccP3=QPointF(m_ptEccR3.x()+m_dREcc*cos(dChamferRadian),
                      m_ptEccR3.y()-m_dREcc*sin(dChamferRadian));
    m_ptEccP4=QPointF(m_ptEccR3.x()-m_dREcc*cos(dChamferRadian),
                      m_ptEccR3.y()+m_dREcc*sin(dChamferRadian));
}

void Cell::CreatePath()
{
    CreatePoint();
    //中心轴外形
    QPainterPath cenPath;
    cenPath.moveTo(m_pPan->Op2Vp(m_ptCenP1));
    cenPath.lineTo(m_pPan->Op2Vp(m_ptCenP2));
    cenPath.lineTo(m_pPan->Op2Vp(m_ptCenP3));
    cenPath.lineTo(m_pPan->Op2Vp(m_ptCenP4));
    cenPath.closeSubpath();
    m_pathCen=cenPath;
    //偏心轴外形
    QPainterPath eccPath;
    //方向为P2,P1,P4,P3
    //从圆弧的P2点开始
    eccPath.arcMoveTo(m_pPan->Radius2Rect(m_ptEccR1.x(), m_ptEccR1.y(), m_dREcc),
                      m_dChamferDegree);
    //顺时针180°从P2到P1
    eccPath.arcTo(m_pPan->Radius2Rect(m_ptEccR1.x(), m_ptEccR1.y(), m_dREcc),
                  m_dChamferDegree,180.0);
    //顺时针180°从P4到P3
    eccPath.arcTo(m_pPan->Radius2Rect(m_ptEccR3.x(), m_ptEccR3.y(), m_dREcc),
                  m_dChamferDegree+180.0,180.0);
    eccPath.closeSubpath();
    m_pathEcc=eccPath;
    //画运行轨迹
    CreateArc();
}

void Cell::CreateArc()
{
    //运行轨迹，中心轴、偏心轴根据碰撞处理进行运行展开
    QPainterPath arcPath;
    int nCenSlice=0;
    int nEccSlice=0;
    QPointF ptFiber=QPointF(m_dCenterX,m_dCenterY);
    arcPath.moveTo(m_pPan->Op2Vp(ptFiber));
    if(m_bRunStatus)
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
            arcPath.lineTo(m_pPan->Op2Vp(ptFiber));
        }
    }
    else
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
            arcPath.lineTo(m_pPan->Op2Vp(ptFiber));
        }
    }
    m_pathArc=arcPath;
}
