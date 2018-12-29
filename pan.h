#ifndef PAN_H
#define PAN_H

#include <QObject>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QFile>
#include <QLineF>
#include <QTimerEvent>
#include "mymethod.h"
#include "coord.h"
#include "cell.h"
#include "paintarea.h"
#include "controlwidget.h"
#include "shapewidget.h"

class PaintArea;
class ControlWidget;
class ShapeWidget;
class Cell;
class Pan : public QObject
{
    Q_OBJECT
public:
    explicit Pan(PaintArea *pArea,int nCenterX,int nCenterY);
    virtual ~Pan();

public:
    Coord*             m_pCoord;
    Cell*              m_pCell[CELL_NUM];
    PaintArea*         m_pPaintArea;
    ControlWidget*     m_pControl;
    ShapeWidget*       m_pShape;

public:
    const double	    m_dRPan = 230;   			//焦面板半径
    const double	    m_dRPanCenter = 28;		    //焦面板中心孔半径
    const double        m_dMaxZoom = 20;            //最大放大倍数
    const double        m_dMinZoom = 1;             //最小缩小倍数
    int	                m_nCellsX;	                //盘正X轴，根据单元坐标点调整
    int             	m_nCellsY;	                //盘正Y轴，根据单元坐标点调整
    int                 m_nRunID[CELL_NUM];         //运行的单元号
    double              m_dRunRadius[CELL_NUM];     //单元的运行半径
    double              m_dRunRadian[CELL_NUM];     //单元运行到的弧度,读取文件的度数°转化为弧度rad,相对于x轴正方向
    QPainterPath        m_pathInsertects[CELL_NUM]; //重叠区域

    int                 m_nRunNum;                  //运行单元的总数量
    int                 m_nSliceIndex;              //弧度片展开索引
    int                 m_nPathIndex;               //每检测一次重叠区域个数
    int                 m_nPlayIndex;               //运行索引
    bool                m_bRunStatus;               //运行状态，在运行为true
    bool                m_bRunEnd;                  //运行完成
    bool                m_bPause;                   //暂停状态
    bool                m_bDealEnd;                 //处理完成
    QString             m_sFilePath;                //读取文件路径
    int                 m_nPosIndex;                //单元波形位置索引
    int                 m_nCellWavePos;             //单元波形的位置
    bool                m_bDealRuned[CELL_NUM];     //单元是否已到达目标位置，不运动的单元默认为已到达目标位置
public:
    QPainter*  m_pPainter;          //传递绘画
    double	   m_dZoom;			    //缩放比
    int		   m_nRow;			    //单元实际总行数
    int        m_nRunTimerID;       //运行定时器ID
    int        m_nSolutionTimerID;  //解决方案定时器ID
    int        m_nDealTimerID;      //测试定时器ID
public:
    QColor     m_cColor[COLOR_AMOUNT];
    QPen*      m_pSolidPen[COLOR_AMOUNT];
    QPen*      m_pDashPen[COLOR_AMOUNT];
    QPen*      m_pDddPen[COLOR_AMOUNT];
    QBrush*    m_bBrush[COLOR_AMOUNT];
    QMap<QString,int> m_mapResult;
public:
    void    InitPan();               //初始化Pan
    void    InitCheckCell();         //初始化Cell
    void	ReadDataFile();			 //读取执行文件
    QColor  GetDefinedColor(int n);  //给定颜色值
    void    InitDrawTools();         //初始化绘图工具
    void    CloseDrawTools();        //删除绘图工具
    QPoint	Op2Vp(const double dx, const double dy);  //原点转换视图点
    QPointF	Op2Vp(const QPointF pointF);              //原点转换视图点
    QPoint	Vp2Op(const QPoint& point);               //视图点转换原点
    void    GetCellID(const QPoint& point,int& nID);  //根据点值找到单元号
    void    CellCenEccValue(QPoint &point);
    QRect   Radius2Rect(double dx,double dy,double dR);//视图点和半径转换rect
    void	Draw();						//绘制单元

    int     CellDistance(int nID,int mID);                //两单元之间的距离
    int     CenTargetSlice(int nID);                      //中心轴到目标跨度的弧度片
    int     EccTargetSlice(int nID);                      //偏心轴到目标位置的弧度片
    int     CalCenSlice(int nID, int nPos);               //计算单元中心轴已转动过的弧度片
    int     CalEccSlice(int nID, int nPos);               //计算单元偏心轴已转动过的弧度片
    void    SetCenPosValue(int nID, int nPos, int nValue);//设置单元中心轴某位置的值
    void    SetEccPosValue(int nID, int nPos, int nValue);//设置单元偏心轴某位置的值
    void    SetCellWavePos(int nID,int nPos); //设置单元波形当前位置，单元通过位置计算当前角度值
    void    SetCellRunStyle(int nID,bool bStyle);//设置单元运行形式，是运行还是处理
    void    SetCellCenSlice(int nID,int nSlice);    //设置中心轴单元的度数
    void    SetCellEccSlice(int nID,int nSlice);    //设置偏心轴单元的度数
    void    SetCellEccRadius(int nID,double dRadius);//设置偏心轴单元半径
    int     CellCenPosValue(int nID,int nPos);//某单元中心轴波形的某位置值
    int     CellEccPosValue(int nID,int nPos);//某单元中心轴波形的某位置值
    bool    CellRunStatus(int nID);           //某单元是否运行
    int     CellCenFinalPos(int nID);             //中心轴波形被赋值的最终位置值
    int     CellEccFinalPos(int nID);             //偏心轴波形被赋值的最终位置值
    QPainterPath CellCenPath(int nID);            //单元中心轴外形
    QPainterPath CellEccPath(int nID);            //单元偏心轴外形

    void    PlayRun();                 //一般运行
    void    PlayDeal();                //运行处理碰撞之后的波形
    void    PlaySolution();            //碰撞解决方案
    void    SetCellValue();            //一般直接运行时设置单元中心轴偏心值
    void    CreateCellPath(int nID);   //生成单元轮廓和轨迹
    void    CheckIntersects();         //检测碰撞并显示

    void    DealCellPos();             //处理单元位置
    void    DealSolution();            //碰撞检测处理
    int     DetectCollision(int nID,int mID);            //检测碰撞并返回值
    void    DealCollision(int nID, int mID ,int nPos ,int nResult);//处理碰撞
    void    DealCenMethod(int nID, int mID , int nPos);//处理中心轴方法
    void    DealEccMethod(int nID, int mID ,int nPos ,int nBasis);//处理偏心轴方法
    void    SolutionBasis();
    int     DealBasis(int nID, int mID);//判定处理方式的依据,nID单元偏心轴转动与mID目标位置的碰撞情况
    void    DealNearCell(int nID,int mID, int nPos);          //处理周边单元的每个位置是否碰撞
    void    DealEachPos(int nID,int mID,int nPos);  //处理两个单元每个位置是否碰撞
    void    DealCen(int nID,int nPos);              //碰撞时处理中心轴，不能在运行时有后退
    void    DealEcc(int nID,int nPos);              //碰撞时处理偏心轴，不能在运行时有后退
    void    DealCenWave(int nID,int nShare);        //碰撞时处理波形，使波形变成2段式，最终最多3段式波形
    void    DealEccWave(int nID,int nShare);        //碰撞时处理波形，使波形变成2段式，最终最多3段式波形
public:
    inline  void    SetCellCenter(int nX,int nY)            { m_nCellsX=nX;m_nCellsY=nY;}
    inline  void    SetPainter(QPainter* pPainter)          { m_pPainter = pPainter; }
    inline  void	SetZoom(const double& zoom)				{ m_dZoom = zoom; }
    inline  double	GetZoom()								{ return m_dZoom; }
    inline  int		GetWidth()                              { return int(m_nCellsX * 2 * m_dZoom);}
    inline  int		GetHeight()                             { return int(m_nCellsY * 2 * m_dZoom);}
    inline  int     GetWavePos()                            { return m_nCellWavePos;}

public:
    void timerEvent(QTimerEvent *event);
signals:
    void SendCollision(int nID,int mID,bool bn,bool bm);
    void ShowText(QString sTemp);
    void DealProgress();
    void SendCenValue(QString sID,QString sData);
    void SendEccValue(QString sID,QString sData);
public slots:
    void    ShowRun();
    void    PauseRun();
    void    ResetRun();
    void    DealRandom();
    void    CheckEndStatus();
    void    OverInitPan(QString sPath);
    void    PlanSolution();
    void    RunShape();
};

#endif // PANEL_H
