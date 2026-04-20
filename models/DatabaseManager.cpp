
#include "DatabaseManager.h"

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) m_db.close();
}

bool DatabaseManager::initDatabase(const QString& dbName) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbName);

    if (!m_db.open()) return false;

    QSqlQuery query;
    QString createTable =
        "CREATE TABLE IF NOT EXISTS schedules ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "category_id INTEGER DEFAULT 0, "
        "title TEXT NOT NULL, "
        "content TEXT, "
        "start_time TEXT NOT NULL, "
        "end_time TEXT NOT NULL, "
        "color TEXT"
        ")";

    return query.exec(createTable);
}

bool DatabaseManager::addSchedule(int categoryId, const QString& title, const QString& content,
                                  const QDateTime& start, const QDateTime& end, const QString& color) {
    QSqlQuery query;
    query.prepare("INSERT INTO schedules (category_id, title, content, start_time, end_time, color) "
                  "VALUES (:cat_id, :title, :content, :start, :end, :color)");

    query.bindValue(":cat_id", categoryId);
    query.bindValue(":title", title);
    query.bindValue(":content", content);
    query.bindValue(":start", start.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":end", end.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":color", color);

    return query.exec();
}

QList<QVariantMap> DatabaseManager::getSchedulesForMonth(int year, int month) {
    QList<QVariantMap> schedules;
    QSqlQuery query;
    QString datePattern = QString("%1-%2").arg(year).arg(month, 2, 10, QChar('0'));

    query.prepare("SELECT * FROM schedules WHERE start_time LIKE :pattern OR end_time LIKE :pattern");
    query.bindValue(":pattern", datePattern + "%");

    if (query.exec()) {
        while (query.next()) {
            QVariantMap item;
            item["id"] = query.value("id");
            item["category_id"] = query.value("category_id");
            item["title"] = query.value("title");
            item["content"] = query.value("content");
            item["start"] = query.value("start_time");
            item["end"] = query.value("end_time");
            item["color"] = query.value("color");
            schedules.append(item);
        }
    }
    return schedules;
}

QList<QVariantMap> DatabaseManager::getSchedulesForDay(const QDate& date) {
    QList<QVariantMap> schedules;
    QSqlQuery query;
    QString dateStr = date.toString("yyyy-MM-dd");

    query.prepare("SELECT * FROM schedules WHERE start_time LIKE :date ORDER BY start_time ASC");
    query.bindValue(":date", dateStr + "%");

    if (query.exec()) {
        while (query.next()) {
            QVariantMap item;
            item["id"] = query.value("id");
            item["category_id"] = query.value("category_id");
            item["title"] = query.value("title");
            item["content"] = query.value("content");
            item["start"] = query.value("start_time");
            item["end"] = query.value("end_time");
            item["color"] = query.value("color");
            schedules.append(item);
        }
    }
    return schedules;
}

bool DatabaseManager::updateSchedule(int id, int categoryId, const QString& title, const QString& content,
                                     const QDateTime& start, const QDateTime& end, const QString& color) {
    QSqlQuery query;
    query.prepare("UPDATE schedules SET category_id = :cat_id, title = :title, content = :content, "
                  "start_time = :start, end_time = :end, color = :color "
                  "WHERE id = :id");

    query.bindValue(":cat_id", categoryId); // 업데이트 항목에 추가
    query.bindValue(":title", title);
    query.bindValue(":content", content);
    query.bindValue(":start", start.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":end", end.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":color", color);
    query.bindValue(":id", id);

    return query.exec();
}

bool DatabaseManager::deleteSchedule(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM schedules WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}