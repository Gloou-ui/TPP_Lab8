#include "clinic/sqlite_api_demo.hpp"

#include <sqlite3.h>

#include <memory>
#include <stdexcept>
#include <string>

namespace clinic {
namespace {

void checkSqlite(int result, sqlite3* database, const std::string& operation) {
    if (result != SQLITE_OK && result != SQLITE_DONE && result != SQLITE_ROW) {
        const auto message = database != nullptr ? sqlite3_errmsg(database)
                                                 : sqlite3_errstr(result);
        throw std::runtime_error(operation + ": " + message);
    }
}

struct StatementDeleter {
    void operator()(sqlite3_stmt* statement) const noexcept {
        sqlite3_finalize(statement);
    }
};

using Statement = std::unique_ptr<sqlite3_stmt, StatementDeleter>;

Statement prepare(sqlite3* database, const char* sql) {
    sqlite3_stmt* rawStatement = nullptr;
    const int result = sqlite3_prepare_v2(database, sql, -1, &rawStatement, nullptr);
    checkSqlite(result, database, "Не вдалося підготувати SQL-запит");
    return Statement{rawStatement};
}

std::string columnText(sqlite3_stmt* statement, int column) {
    const auto* value = sqlite3_column_text(statement, column);
    return value == nullptr ? std::string{} :
                              reinterpret_cast<const char*>(value);
}

}

struct SqliteApiDemo::Connection {
    sqlite3* database{};

    ~Connection() {
        if (database != nullptr) {
            sqlite3_close(database);
        }
    }
};

SqliteApiDemo::SqliteApiDemo(const std::string& databasePath)
    : connection_(std::make_unique<Connection>()) {
    const int result =
        sqlite3_open_v2(databasePath.c_str(), &connection_->database,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (result != SQLITE_OK) {
        const std::string message =
            connection_->database != nullptr
                ? sqlite3_errmsg(connection_->database)
                : sqlite3_errstr(result);
        throw std::runtime_error("Не вдалося відкрити SQLite: " + message);
    }
}

SqliteApiDemo::~SqliteApiDemo() = default;

int SqliteApiDemo::addDoctor(const Doctor& doctor) {
    auto statement =
        prepare(connection_->database,
                "INSERT INTO doctors (full_name, specialty, room_number, phone) "
                "VALUES (?, ?, ?, ?)");

    checkSqlite(sqlite3_bind_text(statement.get(), 1, doctor.fullName.c_str(), -1,
                                  SQLITE_TRANSIENT),
                connection_->database, "Не вдалося прив'язати ім'я лікаря");
    checkSqlite(sqlite3_bind_text(statement.get(), 2, doctor.specialty.c_str(), -1,
                                  SQLITE_TRANSIENT),
                connection_->database, "Не вдалося прив'язати спеціальність");
    checkSqlite(sqlite3_bind_int(statement.get(), 3, doctor.roomNumber),
                connection_->database, "Не вдалося прив'язати номер кабінету");
    checkSqlite(sqlite3_bind_text(statement.get(), 4, doctor.phone.c_str(), -1,
                                  SQLITE_TRANSIENT),
                connection_->database, "Не вдалося прив'язати телефон");

    checkSqlite(sqlite3_step(statement.get()), connection_->database,
                "Не вдалося додати лікаря через SQLite C API");
    return static_cast<int>(sqlite3_last_insert_rowid(connection_->database));
}

std::optional<Doctor> SqliteApiDemo::getDoctorById(int id) const {
    auto statement =
        prepare(connection_->database,
                "SELECT id, full_name, specialty, room_number, phone "
                "FROM doctors WHERE id = ?");
    checkSqlite(sqlite3_bind_int(statement.get(), 1, id), connection_->database,
                "Не вдалося прив'язати ID лікаря");

    const int result = sqlite3_step(statement.get());
    if (result == SQLITE_DONE) {
        return std::nullopt;
    }
    checkSqlite(result, connection_->database,
                "Не вдалося отримати лікаря через SQLite C API");

    return Doctor{sqlite3_column_int(statement.get(), 0),
                  columnText(statement.get(), 1),
                  columnText(statement.get(), 2),
                  sqlite3_column_int(statement.get(), 3),
                  columnText(statement.get(), 4)};
}

}
