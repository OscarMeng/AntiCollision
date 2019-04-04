#ifndef COORD_H
#define COORD_H

#include <QObject>
#include "mymethod.h"
#include <qmath.h>

class Coord : public QObject
{
    Q_OBJECT
public:
    explicit Coord(QObject *parent = 0);

signals:

public slots:
public:
    double m_dCoord[CELL_NUM][2];//每个单元的X,Y坐标值
    int    m_nRowNum[CELL_ROW];  //每行的数量
    int    m_nNewRow[NEW_ROW];   //新单元每行数量
    double m_dNewCoord[NEW_NUM][2];//新单元X,Y坐标值
public:
    void CalCoord();
    void CalNewCoord();
};

#endif // COORD_H
