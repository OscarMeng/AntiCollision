#ifndef MYMETHOD_H
#define MYMETHOD_H

#define PI		     3.1415926
#define STOP_STATUS  0             //是否运行
#define RUN_STATUS   1             //是否运行
#define CELL_SPACE   25.6          //单元中心间距
#define CELL_ROW     19            //单元行数
#define CELL_NUM     234           //单元总数
#define CELL_RATIO   1.5           //原点放大倍数
#define STOP_RADIUS  8.5           //孔半径
#define RUN_ANGLE    0             //单元运行到的角度
#define ORIGNAL_ZOOM 1.0           //适应窗口的原始放大倍数
#define WAVE_NUM     2000          //波形总数
#define SPLIT_DEGREE 16            //1°被16份
#define RUN_TIME     10            //定时器时间间隔
#define MIN_ANGLE    0             //最小度数
#define MAX_ANGLE    360           //最大度数
#define BYTE_RUN     1             //单元运行波值
#define BYTE_STOP    0             //单元停止波值
#define BYTE_BACK    -1            //单元后退波值
#define BYTE_NULL    -2            //单元空时波值
#define MAX_RADIUS   2.3           //偏心轴大半径
#define MIN_RADIUS   2.1           //偏心轴小半径

#define NEW_NUM      313           //新单元数
#define NEW_ROW      21            //新行数
#define NEW_SPACE    45            //新单元间距

#define COLOR_NUM    9
#define COLOR_CELL   0
#define COLOR_BREAK  1
#define COLOR_RUN    2
#define COLOR_COORD  3
#define COLOR_TEXT   4
#define COLOR_CELLBK 5
#define COLOR_TARGET 6
#define COLOR_BORDER 7
#define COLOR_SHAFT  8


#endif // MYMETHOD_H
