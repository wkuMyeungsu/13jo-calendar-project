#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QList>
#include <QVariantMap>

class DatabaseManager {
public:    static DatabaseManager& instance();

    bool initDatabase(const QString& dbName);
    bool addSchedule(const QString& title, const QString& content,
                     const QDateTime& start, const QDateTime& end, const QString& color);
    bool updateSchedule(int id, const QString& title, const QString& content,
                        const QDateTime& start, const QDateTime& end, const QString& color);
    bool deleteSchedule(int id);

    // 특정 월의 일정을 가져오는 함수 (QList<QVariantMap> 형태로 반환)
    QList<QVariantMap> getSchedulesForMonth(int year, int month);
    QList<QVariantMap> getSchedulesForDay(const QDate& date);
private:
    DatabaseManager() = default;
    ~DatabaseManager();
    QSqlDatabase m_db;
};


#endif // DATABASEMANAGER_H
