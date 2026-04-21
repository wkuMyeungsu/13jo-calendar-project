#pragma once
#include <QString>
#include <QDateTime>

struct Schedule {
    int       id         = -1;
    int       categoryId = 0;
    QString   title;
    QString   content;
    QDateTime start;
    QDateTime end;
    QString   color;
};
