#ifndef NEWCELL_H
#define NEWCELL_H

#include <QObject>

class NewCell : public QObject
{
    Q_OBJECT
public:
    explicit NewCell(QObject *parent = 0);

signals:

public slots:
};

#endif // NEWCELL_H