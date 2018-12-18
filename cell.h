#ifndef CELL_H
#define CELL_H

#include <QObject>
#include <QRect>
#include <QPainter>
#include <QPointF>
#include "mymethod.h"
#include "pan.h"

class Pan;
class Cell : public QObject
{
    Q_OBJECT
public:
    explicit Cell(int nID, int nStatus, double dCenterX, double dCenterY);
    virtual ~Cell();
public:
    Pan*             m_pPan;
    QPainter*        m_pPainter;          //在此绘制
    QRect            m_rRect;             //单元所在的正方形
    int              m_nCenWave[WAVE_NUM];//中心轴运行的波形数据
    int              m_nEccWave[WAVE_NUM];//偏心轴运行的波形数据
private:
    const double m_dCen = 13;         //中心轴长度
    const double m_dEcc = 13.5;       //偏心轴长度
    const double m_dRunEcc = 8.5;     //偏心轴展开长度
    double m_dREcc; //偏心轴圆弧半径

    QPointF m_ptCenP1;             //中心轴起始时4点
    QPointF m_ptCenP2;             //中心轴起始时4点
    QPointF m_ptCenP3;             //中心轴起始时4点
    QPointF m_ptCenP4;             //中心轴起始时4点
    double m_dLenghtP1;            //中心轴起始时4点相当于坐标原点（圆心）的长度
    double m_dLenghtP2;            //中心轴起始时4点相当于坐标原点（圆心）的长度
    double m_dLenghtP3;            //中心轴起始时4点相当于坐标原点（圆心）的长度
    double m_dLenghtP4;            //中心轴起始时4点相当于坐标原点（圆心）的长度
    double m_dAngleP1;             //中心轴起始时4点相当于坐标x轴的偏转角度
    double m_dAngleP2;             //中心轴起始时4点相当于坐标x轴的偏转角度
    double m_dAngleP3;             //中心轴起始时4点相当于坐标x轴的偏转角度
    double m_dAngleP4;             //中心轴起始时4点相当于坐标x轴的偏转角度

    QPointF m_ptEccP1;             //偏心轴外形矩形点
    QPointF m_ptEccP2;             //偏心轴外形矩形点
    QPointF m_ptEccP3;             //偏心轴外形矩形点
    QPointF m_ptEccP4;             //偏心轴外形矩形点
    QPointF m_ptEccR1;             //偏心轴前端半圆中心点
    QPointF m_ptEccR2;             //偏心轴展开时旋转中心点
    QPointF m_ptEccR3;             //偏心轴后端半圆中心点

    QPainterPath m_pathCen;        //中心轴外形轨迹
    QPainterPath m_pathEcc;        //偏心轴外形轨迹
    QPainterPath m_pathArc;        //运行的圆弧轨迹

private:
    int          m_nID;                 //单元编号
    int          m_nStatus;             //单元是否选中运行的，0否，1是
    double       m_dCenterX;            //单元中心X
    double       m_dCenterY;            //单元中心Y
    double       m_dRadius;             //单元运行半径
    int          m_nRunAngle;           //单元运行到指定点时的度数，°,相对于x轴正方向
    int          m_nRunDegree;          //单元运行到指定点时的度数，°*16,1°分成16份，方便画圆弧,相对于x轴正方向
    double       m_dStartAngle;         //单元根据半径展开后相对于x正轴得到度数，小数
    int          m_nStartDegree;        //单元根据半径展开后相对于x正轴得到度数，°*16
    int          m_nCurrentCenDegree;   //当前中心轴展开角度，相对于x轴负方向，°*16
    int          m_nCurrentEccDegree;   //当前偏心轴展开角度，相对于x轴负方向，°*16
    double       m_dEccRunAngle;        //偏心轴展开需要运行的角度，小数
    int          m_nEccRunDegree;       //偏心轴展开需要运行的角度，°*16
    double       m_dDrawEccAngle;       //画偏心轴外圆弧的起始角度，小数°
    int          m_nWavePos;            //轴波形位置
    bool         m_bRunStatus;          //一般运行状态
public:
    void   InitCell(Pan* pPan, double dRadius = STOP_RADIUS, int nAngle=RUN_ANGLE);
    QRect  CalRect();
    void   Draw();
    void   DrawArc();//运行半径小于中心轴m_dCen+偏心轴m_dEcc，运行要到达的角度从x正轴开始，小于等于2*PI
    void   DrawCenEcc();//画中心轴和偏心轴
    void   DrawUnexpanded();//画未展开的中心轴和偏心轴
    void   DrawTargetPos();//画目标点
    double LengthByPoint(const QPointF pointF);
    void   CalCurrentCenEccDegree();
    void   CreatePoint();
    void   CreatePath();
    void   CreateArc();

public:
    inline void SetPainter(QPainter* pPainter)      {m_pPainter = pPainter;}
    inline void SetCurrentCenDegree(int nDegree)    {m_nCurrentCenDegree=nDegree;}
    inline void SetCurrentEccDegree(int nDegree)    {m_nCurrentEccDegree=nDegree;}
    inline void SetRunStatus(bool bStatus)          {m_bRunStatus=bStatus;}
    inline int  GetCurrentCenDegree()               {return m_nCurrentCenDegree;}
    inline int  GetCurrentEccDegree()               {return m_nCurrentEccDegree;}
    inline int  GetRunDegree()                      {return m_nRunDegree;}
    inline int  GetStartDegree()                    {return m_nStartDegree;}
    inline int  GetEccRunDegree()                   {return m_nEccRunDegree;}
    inline int  GetRadius()                         {return m_dRadius;}
    inline double  GetCenterX()                     {return m_dCenterX;}
    inline double  GetCenterY()                     {return m_dCenterY;}
    inline void SetWavePos(int nWavePos)            {m_nWavePos=nWavePos;}
    inline void SetEccRadius(double dR)             {m_dREcc=dR;}
    inline QPainterPath GetCenPath()                {return m_pathCen;}
    inline QPainterPath GetEccPath()                {return m_pathEcc;}
signals:

public slots:
};

#endif // CELL_H
