#ifndef FORTEST_DB_HPP
#define FORTEST_DB_HPP
#include <sqlite3.h>
#include <string>

class SqliteDb {
public:
    explicit SqliteDb(const std::string& path) {
        if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
            throw std::runtime_error("Failed to open SQLite database");
        }
    }

    // Non-copyable
    SqliteDb(const SqliteDb&) = delete;
    SqliteDb& operator=(const SqliteDb&) = delete;

    // Movable
    SqliteDb(SqliteDb&& other) noexcept : db_(other.db_) {
        other.db_ = nullptr;
    }

    SqliteDb& operator=(SqliteDb&& other) noexcept {
        if (this != &other) {
            close();
            db_ = other.db_;
            other.db_ = nullptr;
        }
        return *this;
    }

    ~SqliteDb() {
        close();
    }

    sqlite3* get() const noexcept {
        return db_;
    }

    void exec(const std::string& sql) const {
        char* err = nullptr;
        if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
            std::string msg = err ? err : "unknown SQLite error";
            sqlite3_free(err);
            throw std::runtime_error(msg);
        }
    }

private:
    sqlite3* db_ = nullptr;

    void close() noexcept {
        if (db_) {
            sqlite3_close(db_);
            db_ = nullptr;
        }
    }
};
class SqliteStmt {
public:
    SqliteStmt(sqlite3* db, const std::string& sql) {
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt_, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare SQLite statement");
        }
    }

    // Non-copyable
    SqliteStmt(const SqliteStmt&) = delete;
    SqliteStmt& operator=(const SqliteStmt&) = delete;

    // Movable
    SqliteStmt(SqliteStmt&& other) noexcept : stmt_(other.stmt_) {
        other.stmt_ = nullptr;
    }

    ~SqliteStmt() {
        if (stmt_) {
            sqlite3_finalize(stmt_);
        }
    }

    bool step() {
        return sqlite3_step(stmt_) == SQLITE_ROW;
    }

    const char* column_text(int i) const {
        return reinterpret_cast<const char*>(sqlite3_column_text(stmt_, i));
    }
    sqlite3_stmt* get() const noexcept { return stmt_; }

    // (optional) reset for reuse
    void reset() {
        sqlite3_reset(stmt_);
        sqlite3_clear_bindings(stmt_);
    }

private:
    sqlite3_stmt* stmt_ = nullptr;
};

#endif //FORTEST_DB_HPP