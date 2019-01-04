#include "coord.h"

Coord::Coord(QObject *parent) : QObject(parent)
{
    m_nRowNum[0] = m_nRowNum[18] = 4;
    m_nRowNum[1] = m_nRowNum[17] = 9;
    m_nRowNum[2] = m_nRowNum[16] = 10;
    m_nRowNum[3] = m_nRowNum[15] = 13;
    m_nRowNum[4] = m_nRowNum[14] = 14;
    m_nRowNum[5] = m_nRowNum[13] = 15;
    m_nRowNum[6] = m_nRowNum[12] = 16;
    m_nRowNum[7] = m_nRowNum[11] = 15;
    m_nRowNum[8] = m_nRowNum[10] = 16;
    m_nRowNum[9] = 17;

    m_nNewRow[0]=m_nNewRow[20]=7;
    m_nNewRow[1]=m_nNewRow[19]=10;
    m_nNewRow[2]=m_nNewRow[18]=13;
    m_nNewRow[3]=m_nNewRow[17]=14;
    m_nNewRow[4]=m_nNewRow[16]=15;
    m_nNewRow[5]=m_nNewRow[15]=16;
    m_nNewRow[6]=m_nNewRow[14]=17;
    m_nNewRow[7]=m_nNewRow[13]=18;
    m_nNewRow[8]=m_nNewRow[12]=19;
    m_nNewRow[9]=m_nNewRow[11]=18;
    m_nNewRow[10]=19;
    for(int i=0;i<CELL_NUM;i++)
    {
        m_dCoord[i][0]=0;
        m_dCoord[i][1]=0;
    }
    for(int j=0;j<NEW_NUM;j++)
    {
        m_dNewCoord[j][0]=0;
        m_dNewCoord[j][1]=0;
    }
    CalCoord();
    CalNewCoord();
}

void Coord::CalCoord()
{
    double dV = CELL_SPACE*sin(PI / 3);
    double dH = CELL_SPACE/2.0;
    int nCellNum=0;

    //整个图形中心点为（0，0)
    //共19行,顺序编号m_nCellNum++
    for (int i = 0; i < CELL_ROW;++i)
    {
        for (int j = 0; j < m_nRowNum[i];++j)
        {
            double dx = 2 * j * dH - (m_nRowNum[i]-1) * dH;
            double dy = i * dV - 9 * dV;
            //去除中心的7个单元
            if (!(
                ((int)dx == -(int)dH && (int)dy == (int)dV)
                || ((int)dx == (int)dH && (int)dy == (int)dV)
                || ((int)dx == int(-2 * dH) && (int)dy == 0)
                || ((int)dx == 0 && (int)dy == 0)
                || ((int)dx == int(2 * dH) && (int)dy == 0)
                || ((int)dx == -(int)dH && (int)dy == -(int)dV)
                || ((int)dx == (int)dH && (int)dy == -(int)dV)
                ))
            {
                m_dCoord[nCellNum][0] = dx;
                m_dCoord[nCellNum][1] = dy;
                nCellNum++;
            }
        }
    }
}

void Coord::CalNewCoord()
{
    double dV = NEW_SPACE*sin(PI / 3);
    double dH = NEW_SPACE/2.0;
    int nNewNum=0;

    //整个图形中心点为（0，0)
    //共21行,顺序编号m_nNewNum++
    for (int i = 0; i < NEW_ROW;i++)
    {
        for (int j = 0; j < m_nNewRow[i];j++)
        {
            double dx = 2 * j * dH - (m_nNewRow[i]-1) * dH;
            double dy = i * dV - 10 * dV;
            m_dNewCoord[nNewNum][0] = dx;
            m_dNewCoord[nNewNum][1] = dy;
            nNewNum++;
        }
    }
}
