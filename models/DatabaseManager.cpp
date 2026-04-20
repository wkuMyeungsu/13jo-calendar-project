#include "./DatabaseManager.h"

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) m_db.close();
}


//데이터 베이스 초기화
bool DatabaseManager::initDatabase(const QString& dbName) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbName);

    if (!m_db.open()) {
        qDebug() << "DB 연결 실패:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query;
    QString createTable =
        "CREATE TABLE IF NOT EXISTS schedules ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "title TEXT NOT NULL, "
        "content TEXT, "
        "start_time TEXT NOT NULL, "
        "end_time TEXT NOT NULL, "
        "color TEXT"
        ")";

    return query.exec(createTable);
}

bool DatabaseManager::addSchedule(const QString& title, const QString& content,
                                  const QDateTime& start, const QDateTime& end, const QString& color) {
    QSqlQuery query;
    query.prepare("INSERT INTO schedules (title, content, start_time, end_time, color) "
                  "VALUES (:title, :content, :start, :end, :color)");

    // ISO 8601 형식(yyyy-MM-ddTHH:mm:ss)으로 저장하면 나중에 정렬하기 매우 쉽습니다.
    query.bindValue(":start", start.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":end", end.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":title", title);
    query.bindValue(":content", content);
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
            item["title"] = query.value("title");
            item["start"] = query.value("start_time");
            item["end"] = query.value("end_time"); // [추가] 종료 시간 매핑
            item["start_time"] = query.value("start_time");
            item["end_time"] = query.value("end_time");
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
    // 모든 컬럼(*)을 가져오도록 쿼리 확인
    query.prepare("SELECT * FROM schedules WHERE start_time LIKE :date "
                  "ORDER BY start_time ASC");
    query.bindValue(":date", dateStr + "%");

    if (query.exec()) {
        while (query.next()) {
            QVariantMap item;
            item["id"] = query.value("id");
            item["title"] = query.value("title");
            item["start"] = query.value("start_time");
            item["end"] = query.value("end_time"); // [추가] 종료 시간 매핑
            item["color"] = query.value("color");
            schedules.append(item);
        }
    }
    return schedules;
}


// [Update] 기존 일정 수정
bool DatabaseManager::updateSchedule(int id, const QString& title, const QString& content,
                                     const QDateTime& start, const QDateTime& end, const QString& color) {
    QSqlQuery query;
    query.prepare("UPDATE schedules SET title = :title, content = :content, "
                  "start_time = :start, end_time = :end, color = :color "
                  "WHERE id = :id");

    query.bindValue(":title", title);
    query.bindValue(":content", content);
    query.bindValue(":start", start.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":end", end.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":color", color);
    query.bindValue(":id", id);

    return query.exec();
}

// [Delete] 일정 삭제
bool DatabaseManager::deleteSchedule(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM schedules WHERE id = :id");
    query.bindValue(":id", id);

    return query.exec();
}