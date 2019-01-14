#ifndef MYMETHOD_H
#define MYMETHOD_H

#define PI		     3.1415926536
#define STOP_STATUS  0             //是否运行
#define RUN_STATUS   1             //是否运行
#define CELL_SPACE   25.6          //单元中心间距
#define INT_SPACE    25            //单元中心间距整数
#define CELL_ROW     19            //单元行数
#define CELL_NUM     234           //单元总数
#define CELL_RATIO   1.5           //原点放大倍数
#define STOP_RADIUS  12.25         //单元半径
#define STOP_RADIAN  0             //不运行单元的弧度
#define ORIGNAL_ZOOM 1.0           //适应窗口的原始放大倍数
#define WAVE_NUM     2000          //波形总数
#define SLICE_RATIO  100           //弧度rad的系数
#define RUN_TIME     1            //定时器时间间隔
#define MIN_ANGLE    0             //最小度数
#define MAX_ANGLE    360           //最大度数
#define BYTE_RUN     1             //单元运行波值
#define BYTE_STOP    0             //单元停止波值
#define BYTE_BACK    -1            //单元后退波值
#define BYTE_NULL    -2            //单元空时波值
#define DEAL_SLICE   10            //碰撞处理片数
#define NEW_NUM      313           //新单元数
#define NEW_ROW      21            //新行数
#define NEW_SPACE    45            //新单元间距
#define NEW_RATIO    0.8           //新单元原点放大倍数
#define BASIS_NULL   0             //判断碰撞无
#define BASIS_CEN    1             //判断碰撞中心轴
#define BASIS_ECC    2             //判断碰撞偏心轴
#define BASIS_CEC    3             //判断碰撞中偏
#define RUN_NULL     0             //不运行
#define RUN_COM      1             //一般运行
#define RUN_PLAN     2             //处理运行
#define RUN_LAST     3             //结果运行

#define COLOR_AMOUNT 9
#define COLOR_NUM    0
#define COLOR_BREAK  1
#define COLOR_RUN    2
#define COLOR_COORD  3
#define COLOR_DISUSE 4
#define COLOR_CIRCLE 5
#define COLOR_TARGET 6
#define COLOR_BORDER 7
#define COLOR_SHAFT  8


#endif // MYMETHOD_H
