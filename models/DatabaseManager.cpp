#include "DatabaseManager.h"

const QString DatabaseManager::DATE_FORMAT = "yyyy-MM-dd HH:mm:ss";

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

    if (!m_db.open()) {
        qDebug() << "DB Open Error:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query;
    QString createSchedulesTable =
        "CREATE TABLE IF NOT EXISTS schedules ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "category_id INTEGER DEFAULT 0, "
        "title TEXT NOT NULL, "
        "content TEXT, "
        "start_time TEXT NOT NULL, "
        "end_time TEXT NOT NULL, "
        "color TEXT, "
        "is_all_day INTEGER DEFAULT 0"
        ")";

    if (!query.exec(createSchedulesTable)) return false;

    // 마이그레이션: 기존 테이블에 is_all_day 컬럼이 없으면 추가
    QSqlQuery checkQuery("PRAGMA table_info(schedules)");
    bool columnExists = false;
    while (checkQuery.next()) {
        if (checkQuery.value("name").toString() == "is_all_day") {
            columnExists = true;
            break;
        }
    }
    if (!columnExists) {
        query.exec("ALTER TABLE schedules ADD COLUMN is_all_day INTEGER DEFAULT 0");
    }

    QString createCategoriesTable =
        "CREATE TABLE IF NOT EXISTS categories ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "color TEXT"
        ")";

    if (!query.exec(createCategoriesTable)) return false;

    // 기본 카테고리 생성 (처음 한 번만)
    query.exec("SELECT COUNT(*) FROM categories");
    if (query.next() && query.value(0).toInt() == 0) {
        addCategory("Work", "#4A90E2");
        addCategory("Personal", "#4CAF50");
        addCategory("Other", "#9E9E9E");
    }

    return true;
}

bool DatabaseManager::addSchedule(int categoryId, const QString& title, const QString& content,
                                  const QDateTime& start, const QDateTime& end, const QString& color, bool isAllDay) {
    QSqlQuery query;
    query.prepare("INSERT INTO schedules (category_id, title, content, start_time, end_time, color, is_all_day) "
                  "VALUES (:cat_id, :title, :content, :start, :end, :color, :is_all_day)");

    query.bindValue(":cat_id", categoryId);
    query.bindValue(":title", title);
    query.bindValue(":content", content);
    query.bindValue(":start", start.toString(DATE_FORMAT));
    query.bindValue(":end", end.toString(DATE_FORMAT));
    query.bindValue(":color", color);
    query.bindValue(":is_all_day", isAllDay ? 1 : 0);

    return query.exec();
}

QList<Schedule> DatabaseManager::getSchedulesForMonth(int year, int month) {
    QList<Schedule> schedules;
    QSqlQuery query;
    QDate firstDay(year, month, 1);
    QDate lastDay(year, month, firstDay.daysInMonth());

    QString startRange = firstDay.toString("yyyy-MM-dd 00:00:00");
    QString endRange   = lastDay.toString("yyyy-MM-dd 23:59:59");

    query.prepare("SELECT * FROM schedules WHERE start_time <= :endRange AND end_time >= :startRange ORDER BY start_time ASC");
    query.bindValue(":startRange", startRange);
    query.bindValue(":endRange",   endRange);

    if (query.exec()) {
        while (query.next()) {
            Schedule item;
            item.id         = query.value("id").toInt();
            item.categoryId = query.value("category_id").toInt();
            item.title      = query.value("title").toString();
            item.content    = query.value("content").toString();
            item.start      = QDateTime::fromString(query.value("start_time").toString(), DATE_FORMAT);
            item.end        = QDateTime::fromString(query.value("end_time").toString(),   DATE_FORMAT);
            item.color      = query.value("color").toString();
            item.isAllDay   = query.value("is_all_day").toInt() == 1;
            schedules.append(item);
        }
    }
    return schedules;
}

QList<Schedule> DatabaseManager::getSchedulesForDay(const QDate& date) {
    QList<Schedule> schedules;
    QSqlQuery query;
    QString dateStart = date.toString("yyyy-MM-dd 00:00:00");
    QString dateEnd   = date.toString("yyyy-MM-dd 23:59:59");

    query.prepare("SELECT * FROM schedules WHERE start_time <= :dateEnd AND end_time >= :dateStart ORDER BY start_time ASC");
    query.bindValue(":dateStart", dateStart);
    query.bindValue(":dateEnd",   dateEnd);

    if (query.exec()) {
        while (query.next()) {
            Schedule item;
            item.id         = query.value("id").toInt();
            item.categoryId = query.value("category_id").toInt();
            item.title      = query.value("title").toString();
            item.content    = query.value("content").toString();
            item.start      = QDateTime::fromString(query.value("start_time").toString(), DATE_FORMAT);
            item.end        = QDateTime::fromString(query.value("end_time").toString(),   DATE_FORMAT);
            item.color      = query.value("color").toString();
            item.isAllDay   = query.value("is_all_day").toInt() == 1;
            schedules.append(item);
        }
    }
    return schedules;
}

bool DatabaseManager::updateSchedule(int id, int categoryId, const QString& title, const QString& content,
                                     const QDateTime& start, const QDateTime& end, const QString& color, bool isAllDay) {
    QSqlQuery query;
    query.prepare("UPDATE schedules SET category_id = :cat_id, title = :title, content = :content, "
                  "start_time = :start, end_time = :end, color = :color, is_all_day = :is_all_day "
                  "WHERE id = :id");

    query.bindValue(":cat_id", categoryId);
    query.bindValue(":title", title);
    query.bindValue(":content", content);
    query.bindValue(":start", start.toString(DATE_FORMAT));
    query.bindValue(":end", end.toString(DATE_FORMAT));
    query.bindValue(":color", color);
    query.bindValue(":is_all_day", isAllDay ? 1 : 0);
    query.bindValue(":id", id);

    return query.exec();
}

bool DatabaseManager::deleteSchedule(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM schedules WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

bool DatabaseManager::addCategory(const QString& name, const QString& color) {
    QSqlQuery query;
    query.prepare("INSERT INTO categories (name, color) VALUES (:name, :color)");
    query.bindValue(":name", name);
    query.bindValue(":color", color);
    return query.exec();
}

QList<Category> DatabaseManager::getCategories() {
    QList<Category> categories;
    QSqlQuery query("SELECT * FROM categories ORDER BY id ASC");
    while (query.next()) {
        Category item;
        item.id    = query.value("id").toInt();
        item.name  = query.value("name").toString();
        item.color = query.value("color").toString();
        categories.append(item);
    }
    return categories;
}

bool DatabaseManager::updateCategory(int id, const QString& name, const QString& color) {
    QSqlQuery query;
    query.prepare("UPDATE categories SET name = :name, color = :color WHERE id = :id");
    query.bindValue(":name", name);
    query.bindValue(":color", color);
    query.bindValue(":id", id);
    return query.exec();
}

bool DatabaseManager::deleteCategory(int id) {
    QSqlQuery updateSchedules;
    updateSchedules.prepare("UPDATE schedules SET category_id = 0 WHERE category_id = :id");
    updateSchedules.bindValue(":id", id);
    updateSchedules.exec();

    QSqlQuery query;
    query.prepare("DELETE FROM categories WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

bool DatabaseManager::resetDatabase() {
    QSqlQuery query;

    if (!query.exec("DELETE FROM schedules")) return false;
    if (!query.exec("DELETE FROM categories")) return false;
    // 시퀀스 초기화 (ID 1부터 다시 시작하게 함)
    query.exec("DELETE FROM sqlite_sequence WHERE name='schedules' OR name='categories'");

    // 기본 카테고리 재생성
    addCategory("Work", "#4A90E2");
    addCategory("Personal", "#4CAF50");
    addCategory("Other", "#9E9E9E");

    return true;
}
