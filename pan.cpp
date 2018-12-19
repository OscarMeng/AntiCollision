#include "pan.h"

Pan::Pan(PaintArea *pArea, int nCenterX, int nCenterY):m_pPaintArea(pArea)
{
    m_nCellsX=nCenterX/2;
    m_nCellsY=nCenterY/2;
    m_nSliceIndex=0;
    m_nPathIndex=0;
    m_nPlayIndex=0;
    m_nPosIndex=0;
    m_nCellWavePos=0;
    m_bRunStatus=false;
    m_bRunEnd=false;
    m_bPauseStatus=false;
    m_bTargetSlice=false;
    m_bSolutionStatus=false;
    m_pControl=m_pPaintArea->m_pAntiArea->m_pControl;
    m_pShape=m_pPaintArea->m_pAntiArea->m_pShape;
    m_sFilePath="/QAntiCollision/File/CheckCenter.txt";
    m_dZoom = ORIGNAL_ZOOM;//2D放大
    for (int i = 0; i < CELL_NUM;i++)
    {
        m_nRunID[i] = -1;
        m_dRunRadius[i] = -1;
        m_dRunRadian[i] = -1;
    }
    m_pCoord = new Coord;
    InitPan();
}

Pan::~Pan()
{
    delete m_pCoord;
    for (int i = 0; i < CELL_NUM;++i)
    {
        if (m_pCell[i]!=NULL)
        {
            delete m_pCell[i];
        }
    }
    CloseDrawTools();
}

void Pan::InitPan()
{
    InitDrawTools();
    InitCheckCell();
}

void Pan::InitCheckCell()
{
    ReadDataFile();
    for (int i = 0; i < CELL_NUM;++i)
    {
        int flag = 0;
        for (int j = 0; j < m_nRunNum;j++)
        {
            if (m_nRunID[j]==i+1)
            {
                m_pCell[i] = new Cell(i + 1, RUN_STATUS, m_pCoord->m_dCoord[i][0], m_pCoord->m_dCoord[i][1]);
                m_pCell[i]->InitCell(this, m_dRunRadius[j], m_dRunRadian[j]);
                flag = 1;
            }
        }
        if (flag==0)
        {
            m_pCell[i] = new Cell(i + 1, STOP_STATUS, m_pCoord->m_dCoord[i][0], m_pCoord->m_dCoord[i][1]);
            m_pCell[i]->InitCell(this,STOP_RADIUS,STOP_RADIAN);
        }
    }
}

void Pan::ReadDataFile()
{
    QFile fp(m_sFilePath);
    if(fp.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        char c[100];
        fp.readLine(c,sizeof(c));
        int i=0;
        while(!fp.atEnd())
        {
            QString strLine=QString(fp.readLine()).trimmed();
            QStringList strList=strLine.split(QRegExp("\\s{1,}"));//把所有的多余的空格为间隔,正则表达式QRegExp("[\\s]+")
            bool b;
            m_nRunID[i]=strList[0].toInt(&b);
            m_dRunRadius[i]=strList[1].toDouble(&b);
            double dA=strList[2].toDouble(&b);
            m_dRunRadian[i]=dA/180*PI;//把度数转化为弧度，度数可读性好，弧度计算方便
            i++;
        }
        m_nRunNum = i;
    }
    fp.close();
}


QColor Pan::GetDefinedColor(int n)
{
    QColor col;
    switch (n)
    {
    case COLOR_NUM:	    col = QColor(255, 255, 255);	break;//白色单元号颜色
    case COLOR_BREAK:	col = QColor(255, 0, 0);		break;//红色单元碰撞区域颜色
    case COLOR_RUN:	    col = QColor(0, 225, 0);	    break;//绿色单元运行过的路径颜色
    case COLOR_COORD:	col = QColor(180, 0, 0);        break;//浅红色坐标轴颜色
    case COLOR_DISUSE:	col = QColor(50, 50, 50);	    break;//浅灰色不展开单元轴外形
    case COLOR_CIRCLE:	col = QColor(80, 80, 80);       break;//浅灰色单元展开最外圆
    case COLOR_TARGET:	col = QColor(245, 255, 110);	break;//浅黄色目标点
    case COLOR_BORDER:	col = QColor(0, 0, 255);    	break;//蓝色边界
    case COLOR_SHAFT:   col = QColor(235,170,255);      break;//浅紫色运行轴外形

    default:			col = QColor(192, 192, 192);
    }
    return col;
}

void Pan::InitDrawTools()
{
    QColor col;
    for (int i = 0; i < COLOR_AMOUNT; i++)
    {
        col = GetDefinedColor(i);
        m_cColor[i] = col;
        m_pSolidPen[i] = new QPen(col, 1.2, Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
        m_pDashPen[i] = new QPen(col, 1, Qt::DashLine,Qt::SquareCap,Qt::MiterJoin);
        m_pDddPen[i] = new QPen(col, 1, Qt::DashDotDotLine,Qt::SquareCap,Qt::MiterJoin);
        m_bBrush[i] = new QBrush(col);
    }
}

void Pan::CloseDrawTools()
{
    for (int i = 0; i < COLOR_AMOUNT; i++)
    {
        delete m_pSolidPen[i];
        delete m_pDashPen[i];
        delete m_pDddPen[i];
        delete m_bBrush[i];
    }
}

QPoint Pan::Op2Vp(const double dx, const double dy)
{
    double x = (dx * CELL_RATIO + m_nCellsX)* m_dZoom;
    double y = (dy * CELL_RATIO + m_nCellsY)* m_dZoom;
    int nx, ny;
    if (x - floor(x) < 0.5)
        nx = (int)floor(x);
    else
        nx = (int)floor(x) + 1;
    if (y - floor(y) < 0.5)
        ny = (int)floor(y);
    else
        ny = (int)floor(y) + 1;

    return QPoint(nx, ny);
}

QPointF Pan::Op2Vp(const QPointF pointF)
{
    double x = (pointF.x() * CELL_RATIO + m_nCellsX)* m_dZoom;
    double y = (pointF.y() * CELL_RATIO + m_nCellsY)* m_dZoom;
    return QPointF(x, y);
}

QPoint Pan::Vp2Op(const QPoint &point)
{
    double x = (point.x() / m_dZoom - m_nCellsX) / CELL_RATIO;
    double y = (point.y() / m_dZoom - m_nCellsY) / CELL_RATIO;

    int nx, ny;
    if (x - floor(x) < 0.5)
        nx = (int)floor(x);
    else
        nx = (int)floor(x) + 1;
    if (y - floor(y) < 0.5)
        ny = (int)floor(y);
    else
        ny = (int)floor(y) + 1;

    return QPoint(nx, ny);
}

void Pan::GetCellID(const QPoint &point, int &nID)
{
    QPoint p=Vp2Op(point);
    for(int i=0;i<CELL_NUM;i++)
    {
        double dR=sqrt((m_pCell[i]->GetCenterX()-p.x())*(m_pCell[i]->GetCenterX()-p.x())
                      +(m_pCell[i]->GetCenterY()-p.y())*(m_pCell[i]->GetCenterY()-p.y()));
        if(dR<=m_pCell[i]->GetRadius())
        {
            nID=i+1;
            return;
        }
    }
}

void Pan::CellCenEccValue(QPoint &point)
{
    int nID=0;
    GetCellID(point,nID);
    if(nID>0)
    {
        QString sc="";
        QString se="";
        for(int i=1;i<=WAVE_NUM;i++)
        {
            if(CellCenPosValue(nID,i)!=BYTE_NULL)
            {
                sc+=QString::number(CellCenPosValue(nID,i));
            }
            if(CellEccPosValue(nID,i)!=BYTE_NULL)
            {
                se+=QString::number(CellEccPosValue(nID,i));
            }
        }
        emit SendCenValue(QString::number(nID,10),sc);
        emit SendEccValue(QString::number(nID,10),se);
    }
}

QRect Pan::Radius2Rect(double dx, double dy, double dR)
{
    double left = dx - dR;
    double top = dy - dR;
    double right = dx + dR;
    double bottom = dy + dR;

    QPoint leftTop = Op2Vp(left, top);
    QPoint rightBottom = Op2Vp(right, bottom);
    //矩形，起始点，宽和高
    return QRect(leftTop.x(),leftTop.y(),rightBottom.x()-leftTop.x(),rightBottom.y()-leftTop.y());
}

void Pan::Draw()
{
    //创建单元
    for (int i = 1; i <= CELL_NUM;i++)
    {
        CreateCellPath(i);
        m_bRunEnd=false;
    }
    //检测相交区域
    CheckIntersects();
    //画单元,运行时已经生成单元轮廓轨迹
    for(int i=0;i<CELL_NUM;i++)
    {
        m_pCell[i]->SetPainter(m_pPainter);//只能在画之前设置
        m_pCell[i]->Draw();
    }
    //画中心圆，外圆
    m_pPainter->setPen(*m_pSolidPen[COLOR_BORDER]);
    m_pPainter->drawEllipse(Radius2Rect(0,0,m_dRPanCenter));
    m_pPainter->drawEllipse(Radius2Rect(0,0,m_dRPan));
    //画中心轴线
    m_pPainter->setPen(*m_pDddPen[COLOR_COORD]);
    m_pPainter->drawLine(Op2Vp(0,m_dRPan+20.0),Op2Vp(0,-m_dRPan-20.0));
    m_pPainter->drawLine(Op2Vp(m_dRPan+20.0,0),Op2Vp(-m_dRPan-20.0,0));
    //画碰撞区域
    m_pPainter->setPen(*m_pSolidPen[COLOR_BORDER]);
    for(int i=0;i<m_nPathIndex;i++)
    {
        m_pPainter->drawPath(m_pathInsertects[i]);
        m_pPainter->fillPath(m_pathInsertects[i],*m_bBrush[COLOR_BREAK]);
    }
}

int Pan::CenTargetSlice(int nID)
{
    return m_pCell[nID-1]->GetCenRadSlice();
}

int Pan::EccTargetSlice(int nID)
{
    return m_pCell[nID-1]->GetEccRadSlice();
}

int Pan::CalCenSlice(int nID,int nPos)
{
    int nSlice=0;
    for(int i=1;i<=nPos;i++)
    {
        int nValue=CellCenPosValue(nID,i);
        if(nValue==BYTE_NULL)
            break;
        nSlice+=nValue*SLICE_RATIO;
    }
    return nSlice;
}

int Pan::CalEccSlice(int nID, int nPos)
{
    int nSlice=0;
    for(int i=1;i<=nPos;i++)
    {
        int nValue=int(CellEccPosValue(nID,i));
        if(nValue==BYTE_NULL)
            break;
        nSlice+=nValue*SLICE_RATIO;
    }
    return nSlice;
}

void Pan::SetCenPosValue(int nID, int nPos, int nValue)
{
    m_pCell[nID-1]->m_nCenWave[nPos-1]=nValue;
}

void Pan::SetEccPosValue(int nID, int nPos, int nValue)
{
    m_pCell[nID-1]->m_nEccWave[nPos-1]=nValue;
}

void Pan::PlayRun()
{
    killTimer(m_nRunTimerID);
    switch(m_nPlayIndex)
    {
    case 0:
        m_nSliceIndex++;
        m_bRunEnd=true;
        m_nPosIndex++;
        m_nPlayIndex++;
        break;
    case 1:
        SetCellValue();
        m_nPlayIndex++;
        break;
    case 2:
        if(m_bRunEnd)
        {
            PlayOver();
            return;
        }
        else
        {
            m_nPlayIndex++;
        }
        break;
    case 3:
        CheckIntersects();
        m_nPlayIndex++;
        break;
    case 4:
        if(m_bPauseStatus)
        {
            m_bRunStatus=false;
            return;
        }
        else
        {
            m_nPlayIndex++;
        }
        break;
    case 5:
        m_nPlayIndex=0;
        m_pPaintArea->update();
        break;
    default:
        return;
    }
    m_nRunTimerID=this->startTimer(RUN_TIME);
}

void Pan::PlayOver()
{
    m_nPosIndex=0;
    m_nPlayIndex=0;
    m_bRunStatus=false;
    QString s="运行完成！";
    emit ShowText(s);
    QMessageBox::information(m_pPaintArea, "运行提示", "运行完成！",
                             QMessageBox::Ok | QMessageBox::Cancel);
}

void Pan::PlayDeal()
{
    killTimer(m_nDealTimerID);
    switch(m_nPlayIndex)
    {
    case 0:
        m_nPosIndex++;
        m_nPlayIndex++;
    case 1:
        if(m_nPosIndex<=m_nCellWavePos)
        {
            for(int i=0;i<m_nRunNum;i++)
            {
                SetCellWavePos(m_nRunID[i],m_nPosIndex);
                SetCellRunStatus(m_nRunID[i],false);
                SetCellEccRadius(m_nRunID[i],MIN_RADIUS);
                CreateCellPath(m_nRunID[i]);
            }
        }
        else
        {
            m_nPosIndex=0;
            m_nPlayIndex=0;
            QMessageBox::information(m_pPaintArea, "运行提示", "结果运行完成！",
                                     QMessageBox::Ok | QMessageBox::Cancel);
            return;
        }
        m_nPlayIndex++;
    case 2:
        CheckIntersects();
        m_nPlayIndex++;
    case 3:
        m_pPaintArea->update();
        m_nPlayIndex++;
    case 4:
        m_nPlayIndex=0;
        break;
    default:
        return;
    }
    m_nDealTimerID=this->startTimer(RUN_TIME);
}

void Pan::SetCellValue()
{
    for (int i = 0; i < m_nRunNum;i++)
    {
        //中心轴先展开
        if(m_nSliceIndex <= CenTargetSlice(m_nRunID[i]))
        {
            SetCellCenSlice(m_nRunID[i],m_nSliceIndex);
            m_bRunEnd=false;
        }
        //中心轴剩余展开度数与偏心轴度数相同或小于时，偏心轴开始展开
        if(CenTargetSlice(m_nRunID[i])-m_nSliceIndex<EccTargetSlice(m_nRunID[i]))
        {
            if(CenTargetSlice(m_nRunID[i])>=EccTargetSlice(m_nRunID[i])&&
               m_nSliceIndex<=CenTargetSlice(m_nRunID[i]))
            {
                SetCellEccSlice(m_nRunID[i],m_nSliceIndex-CenTargetSlice(m_nRunID[i])+EccTargetSlice(m_nRunID[i]));
                m_bRunEnd=false;
            }
            else if(CenTargetSlice(m_nRunID[i])<EccTargetSlice(m_nRunID[i])&&
                                   m_nSliceIndex<=EccTargetSlice(m_nRunID[i]))
            {
                SetCellEccSlice(m_nRunID[i],m_nSliceIndex);
                m_bRunEnd=false;
            }
        }
        else
        {
            SetCellEccSlice(m_nRunID[i],0);
        }
        SetCellRunStatus(m_nRunID[i],true);
        SetCellEccRadius(m_nRunID[i],MAX_RADIUS);
        CreateCellPath(m_nRunID[i]);
    }
}

void Pan::CheckIntersects()
{
    //检测可能发生碰撞的两个单元，通过运行半径与所在单元的距离
    m_nPathIndex=0;
    for (int i = 0; i < m_nRunNum;i++)
    {
        for(int j=i+1;j<m_nRunNum;j++)
        {
            QString sTemp;
            QPainterPath path1=m_pCell[m_nRunID[i]-1]->GetCenPath();
            QPainterPath path2=m_pCell[m_nRunID[i]-1]->GetEccPath();
            QPainterPath path3=m_pCell[m_nRunID[j]-1]->GetCenPath();
            QPainterPath path4=m_pCell[m_nRunID[j]-1]->GetEccPath();
            bool b1=path1.intersects(path4);
            bool b2=path2.intersects(path3);
            bool b3=path2.intersects(path4);
            if(b1)
            {
                m_pathInsertects[m_nPathIndex]=path1.intersected(path4);
                m_nPathIndex++;
                sTemp.sprintf("单元%02d的中心轴与单元%02d的偏心轴碰撞！",m_nRunID[i],m_nRunID[j]);
                emit ShowText(sTemp);
            }
            if(b2)
            {
                m_pathInsertects[m_nPathIndex]=path2.intersected(path3);
                m_nPathIndex++;
                sTemp.sprintf("单元%02d的偏心轴与单元%02d的中心轴碰撞！",m_nRunID[i],m_nRunID[j]);
                emit ShowText(sTemp);
            }
            if(b3)
            {
                m_pathInsertects[m_nPathIndex]=path2.intersected(path4);
                m_nPathIndex++;
                sTemp.sprintf("单元%02d的偏心轴与单元%02d的偏心轴碰撞！",m_nRunID[i],m_nRunID[j]);
                emit ShowText(sTemp);
            }
        }
    }
}

int Pan::CenFinalPos(int nID)
{
    int nPos=1;
    for(int i=0;i<WAVE_NUM;i++)
    {
        if(m_pCell[nID-1]->m_nCenWave[i]==BYTE_NULL)
            break;
        nPos++;
    }
    return nPos;
}

int Pan::EccFinalPos(int nID)
{
    int nPos=1;
    for(int i=0;i<WAVE_NUM;i++)
    {
        if(m_pCell[nID-1]->m_nEccWave[i]==BYTE_NULL)
            break;
        nPos++;
    }
    return nPos;
}

void Pan::SetCellWavePos(int nID, int nPos)
{
    m_pCell[nID-1]->SetWavePos(nPos);
}

void Pan::SetCellRunStatus(int nID, bool bStatus)
{
    m_pCell[nID-1]->SetRunStatus(bStatus);
}

void Pan::SetCellCenSlice(int nID, int nSlice)
{
    m_pCell[nID-1]->SetCurrentCenSlice(nSlice);
}

void Pan::SetCellEccSlice(int nID, int nSlice)
{
    m_pCell[nID-1]->SetCurrentEccSlice(nSlice);
}

void Pan::SetCellEccRadius(int nID, double dRadius)
{
    m_pCell[nID-1]->SetEccRadius(dRadius);
}

int Pan::CellCenPosValue(int nID, int nPos)
{
    return m_pCell[nID-1]->m_nCenWave[nPos-1];
}

int Pan::CellEccPosValue(int nID, int nPos)
{
    return m_pCell[nID-1]->m_nEccWave[nPos-1];
}

void Pan::CreateCellPath(int nID)
{
     m_pCell[nID-1]->CreatePath();
}

int Pan::DetectCollision(int nID, int mID)
{
    int nValue=0;
    QPainterPath path1=m_pCell[nID-1]->GetCenPath();
    QPainterPath path2=m_pCell[nID-1]->GetEccPath();
    QPainterPath path3=m_pCell[mID-1]->GetCenPath();
    QPainterPath path4=m_pCell[mID-1]->GetEccPath();
    bool b1=path1.intersects(path4);//n中心轴与m偏心轴
    bool b2=path2.intersects(path3);//n偏心轴与m中心轴
    bool b3=path2.intersects(path4);//n偏心轴与m偏心轴
    if(b1)
    {
        nValue+=100;
    }
    if(b2)
    {
        nValue+=10;
    }
    if(b3)
    {
        nValue+=1;
    }
    return nValue;
}

void Pan::PlaySolution()
{
    killTimer(m_nSolutionTimerID);
    switch(m_nPlayIndex)
    {
    case 0:
        m_bSolutionStatus=true;
        m_nCellWavePos++;
        m_nPlayIndex++;
    case 1:
        if(m_nCellWavePos>WAVE_NUM)
        {
            QMessageBox::information(m_pPaintArea, "处理提示", "处理超过！",
                                     QMessageBox::Ok | QMessageBox::Cancel);
            return;
        }
        m_nPlayIndex++;
    case 2:
        DealSolution();
        emit DealProgress();
        m_nPlayIndex++;
    case 3:
        if(m_bSolutionStatus)
        {
            QMessageBox::information(m_pPaintArea, "处理提示", "处理完成！",
                                     QMessageBox::Ok | QMessageBox::Cancel);
            return;
        }
        else
        {
            CheckCollision();
            m_nPlayIndex++;
        }
    case 4:
        CheckIntersects();
        m_pPaintArea->update();
        m_nPlayIndex++;
    case 5:
        m_nPlayIndex=0;
        break;
    default:
        return;
    }
    m_nSolutionTimerID=this->startTimer(RUN_TIME);
}

void Pan::DealSolution()
{
    //未到位置向前1，到达位置暂停0，然后进行碰撞判断
    for(int i=0;i<m_nRunNum;i++)
    {
//        if(CenTargetSlice(m_nRunID[i])>CalCenSlice(m_nRunID[i]))
//        {
//            SetCenPosValue(m_nRunID[i],m_nCellWavePos,BYTE_RUN);
//            m_bSolutionStatus=false;
//        }
//        else
//        {
//            SetCenPosValue(m_nRunID[i],m_nCellWavePos,BYTE_STOP);
//        }
//        if(CenTargetSlice(m_nRunID[i])-CalCenSlice(m_nRunID[i])
//                <EccTargetSlice(m_nRunID[i]))
//        {
//            if(CalEccSlice(m_nRunID[i])<EccTargetSlice(m_nRunID[i]))
//            {
//                SetEccPosValue(m_nRunID[i],m_nCellWavePos,BYTE_RUN);
//                m_bSolutionStatus=false;
//            }
//            else
//            {
//                SetEccPosValue(m_nRunID[i],m_nCellWavePos,BYTE_STOP);
//            }
//        }
//        else
//        {
//            SetEccPosValue(m_nRunID[i],m_nCellWavePos,BYTE_STOP);
//        }
        SetCellWavePos(m_nRunID[i],m_nCellWavePos);
        SetCellRunStatus(m_nRunID[i],false);
        SetCellEccRadius(m_nRunID[i],MAX_RADIUS);
        CreateCellPath(m_nRunID[i]);
    }
}

void Pan::CheckCollision()
{
    for(int i=0;i<m_nRunNum;i++)
    {
        for(int j=i+1;j<m_nRunNum;j++)
        {
            int nResult= DetectCollision(m_nRunID[i],m_nRunID[j]);
            if(nResult!=0)
            {
                DealCollision(m_nRunID[i],m_nRunID[j],m_nCellWavePos,nResult);
            }
        }
    }
}

void Pan::DealCollision(int nID, int mID, int nPos, int nResult)
{
    int nCE=nResult/100;     //n中心轴与m偏心轴相碰
    int nEC=nResult%100/10;  //n偏心轴与m中心轴相碰
    int nEE=nResult%10;      //n偏心轴与m偏心轴相碰
//    if(nCE)
//    {
//        //偏心轴展开角度大于0时
//        if(CalEccSlice(mID)>0)
//        {
//           DealEcc(mID,nPos);
//        }
//    }
//    if(nEC)
//    {
//        //偏心轴展开角度大于0时
//        if(CalEccSlice(nID)>0)
//        {
//           DealEcc(nID,nPos);
//        }
//    }
//    if(nEE)
//    {
//        //偏心轴展开角度大于0时,展开需要小的处理
//        if(EccTargetSlice(nID)-CalEccSlice(nID)<EccTargetSlice(mID)-CalEccSlice(mID))
//        {
//           DealEcc(nID,nPos);
//        }
//        else
//        {
//           DealEcc(mID,nPos);
//        }
//    }
    SetCellWavePos(nID,nPos);
    SetCellWavePos(mID,nPos);
    CreateCellPath(nID);
    CreateCellPath(mID);
    nResult= DetectCollision(nID,mID);
    if(nResult)
    {
        DealCollision(nID,mID,nPos,nResult);
    }
    else
    {
        DealNearPos(nID,mID,nPos);
        return;
    }
}

void Pan::DealNearPos(int nID, int mID, int nPos)
{
    double dx1=m_pCell[nID-1]->GetCenterX();
    double dy1=m_pCell[nID-1]->GetCenterY();
    double dx2=m_pCell[mID-1]->GetCenterX();
    double dy2=m_pCell[mID-1]->GetCenterY();
    for(int i=0;i<m_nRunNum;i++)
    {
        double dx3=m_pCell[m_nRunID[i]-1]->GetCenterX();
        double dy3=m_pCell[m_nRunID[i]-1]->GetCenterY();
        int nL1=int(sqrt((dx1-dx3)*(dx1-dx3)+(dy1-dy3)*(dy1-dy3)));
        int nL2=int(sqrt((dx2-dx3)*(dx2-dx3)+(dy2-dy3)*(dy2-dy3)));
        if(nL1==int(CELL_SPACE))
        {
           DealEachPos(nID,m_nRunID[i],nPos);
        }
        if(nL2==int(CELL_SPACE))
        {
           DealEachPos(mID,m_nRunID[i],nPos);
        }
    }
}

void Pan::DealEachPos(int nID, int mID, int nPos)
{
    for(int i=1;i<=nPos;i++)
    {
        SetCellWavePos(nID,i);
        SetCellWavePos(mID,i);
        CreateCellPath(nID);
        CreateCellPath(mID);
        int nResult= DetectCollision(nID,mID);
        if(nResult)
        {
            DealCollision(nID,mID,i,nResult);
        }
    }
}

void Pan::DealCen(int nID, int nPos)
{
    while(CellCenPosValue(nID,nPos)==BYTE_STOP&&nPos>1)
    {
        nPos--;
    }
    if(CellCenPosValue(nID,nPos)==BYTE_RUN)
    {
        SetCenPosValue(nID,nPos,BYTE_STOP);
    }
}

void Pan::DealEcc(int nID, int nPos)
{
    while(CellEccPosValue(nID,nPos)==BYTE_STOP&&nPos>1)
    {
        nPos--;
    }
    if(CellEccPosValue(nID,nPos)==BYTE_RUN)
    {
        SetEccPosValue(nID,nPos,BYTE_STOP);
    }
}



void Pan::ShowRun()
{
    m_nPlayIndex=0;
    m_nSliceIndex=0;
    m_bRunStatus=true;
    m_bRunEnd=false;
    m_nRunTimerID=this->startTimer(RUN_TIME);
}

void Pan::PauseRun()
{
    if(m_bPauseStatus)
    {
        m_bRunStatus=true;
        m_bPauseStatus=false;
        m_nPlayIndex=4;
        m_nRunTimerID=this->startTimer(RUN_TIME);
    }
    else
    {
        m_bRunStatus=false;
        m_bPauseStatus=true;
    }
}

void Pan::ResetRun()
{
    killTimer(m_nRunTimerID);
    m_nSliceIndex=0;//转过度数设为0
    m_nPosIndex=0;
    m_nPlayIndex=0;
    m_bRunStatus=false;
    for (int i = 0; i < m_nRunNum;i++)
    {
        SetCellCenSlice(m_nRunID[i],0);
        SetCellEccSlice(m_nRunID[i],0);
        SetCellRunStatus(m_nRunID[i],true);
    }
    m_pPaintArea->update();
}

void Pan::CheckEndStatus()
{
    for(int i=0;i<m_nRunNum;i++)
    {
        SetCellCenSlice(m_nRunID[i],CenTargetSlice(m_nRunID[i]));
        SetCellEccSlice(m_nRunID[i],EccTargetSlice(m_nRunID[i]));
        SetCellRunStatus(m_nRunID[i],true);
        SetCellEccRadius(m_nRunID[i],MAX_RADIUS);
    }
    CheckIntersects();
    m_pPaintArea->update();
}

void Pan::OverInitPan(QString sPath)
{
    m_sFilePath=sPath;
    for (int i = 0; i < CELL_NUM;++i)
    {
        if (m_pCell[i]!=NULL)
        {
            delete m_pCell[i];
        }
    }
    InitCheckCell();
    m_pPaintArea->update();
}

void Pan::CalculateRun()
{
    m_nPlayIndex=0;
    m_nCellWavePos=0;
    for (int i = 0; i < m_nRunNum;i++)
    {
        SetCellWavePos(m_nRunID[i],m_nCellWavePos);
        for(int j=0;j<WAVE_NUM;j++)
        {
            if( m_pCell[m_nRunID[i]-1]->m_nCenWave[j]!=BYTE_NULL)
                m_pCell[m_nRunID[i]-1]->m_nCenWave[j]=BYTE_NULL;
            if( m_pCell[m_nRunID[i]-1]->m_nEccWave[j]!=BYTE_NULL)
                m_pCell[m_nRunID[i]-1]->m_nEccWave[j]=BYTE_NULL;
        }
        SetCellRunStatus(m_nRunID[i],false);
    }
    m_pPaintArea->update();
    m_nSolutionTimerID=this->startTimer(RUN_TIME);
}

void Pan::RunShape()
{
    m_nPosIndex=0;
    m_bRunStatus=false;
    m_nDealTimerID=this->startTimer(RUN_TIME);
}

void Pan::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==m_nRunTimerID)
    {
        PlayRun();
    }
    else if(event->timerId()==m_nDealTimerID)
    {
        PlayDeal();
    }
    else if(event->timerId()==m_nSolutionTimerID)
    {
        PlaySolution();
    }
}
