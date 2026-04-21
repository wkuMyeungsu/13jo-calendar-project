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
public:
    static const QString DATE_FORMAT;
    static DatabaseManager& instance();

    // Infrastructure
    bool initDatabase(const QString& dbName);

    // CRUD: Create
    bool addSchedule(int categoryId, const QString& title, const QString& content,
                     const QDateTime& start, const QDateTime& end, const QString& color);

    // CRUD: Read
    QList<QVariantMap> getSchedulesForMonth(int year, int month);
    QList<QVariantMap> getSchedulesForDay(const QDate& date);

    // CRUD: Update
    bool updateSchedule(int id, int categoryId, const QString& title, const QString& content,
                        const QDateTime& start, const QDateTime& end, const QString& color);

    // CRUD: Delete
    bool deleteSchedule(int id);

    // Categories CRUD
    bool addCategory(const QString& name, const QString& color);
    QList<QVariantMap> getCategories();
    bool updateCategory(int id, const QString& name, const QString& color);
    bool deleteCategory(int id);
    bool resetDatabase(); // 모든 일정 및 카테고리 삭제

private:
    DatabaseManager() = default;
    ~DatabaseManager();
    QSqlDatabase m_db;
};


#endif // DATABASEMANAGER_H
