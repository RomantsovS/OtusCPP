#pragma once

#include <sqlite3.h>

#include <iostream>
#include <string>
#include <vector>

class QueryResult {
   public:
    virtual ~QueryResult() = default;

    virtual std::string AsString() const = 0;
};

class QueryResultOK : public QueryResult {
   public:
    QueryResultOK() = default;
    QueryResultOK(int columns, char **data, char **names)
        : m_columns(columns), m_data(columns), m_names(columns) {
        for (int i = 0; i < m_columns; ++i) {
            m_names[i] = names[i];
            m_data[i] = data[i];
        }
    }

    std::string AsString() const override {
        std::ostringstream os;
        os << "OK\n";

        for (int i = 0; i < m_columns; ++i)
            os << m_names[i] << " = " << (!m_data[i].empty() ? m_data[i] : "NULL") << std::endl;

        return os.str();
    }

    int m_columns;
    std::vector<std::string> m_data;
    std::vector<std::string> m_names;
};

class QueryResultError : public QueryResult {
   public:
    QueryResultError(std::string &&msg) : m_msg(msg) {}
    std::string AsString() const {
        std::ostringstream os;
        os << "ERR " << m_msg << std::endl;
        return os.str();
    }

   private:
    std::string m_msg;
};

class DB {
   public:
    DB() {
        if (sqlite3_open(nullptr, &handle)) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(handle) << std::endl;
            sqlite3_close(handle);
            throw std::runtime_error("");
        }
        std::cout << "database opened successfully!" << std::endl;
    }
    ~DB() { sqlite3_close(handle); }

    std::unique_ptr<QueryResult> Exec(std::string &&cmd) {
        std::unique_ptr<QueryResult> qr = std::make_unique<QueryResultOK>();

        exec_internal(cmd, &qr);

        return qr;
    }

   private:
    void exec_internal(std::string cmd, std::unique_ptr<QueryResult>* qr = nullptr) {
        auto print_results = [](void *ptr, int columns, char **data, char **names) -> int {
            for (int i = 0; i < columns; ++i)
                std::cout << names[i] << " = " << (data[i] ? data[i] : "NULL") << std::endl;
            std::cout << std::endl;
            auto res = static_cast<std::unique_ptr<QueryResult> *>(ptr);
            *res = std::make_unique<QueryResultOK>(columns, data, names);
            return 0;
        };

        char *errmsg;
        std::cerr << "executing: " << cmd << std::endl;
        int rc = sqlite3_exec(handle, cmd.c_str(), print_results, static_cast<void*>(qr), &errmsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Can't execute query: " << errmsg << std::endl;
            if(qr)
                *qr = std::make_unique<QueryResultError>(std::string(errmsg));
            sqlite3_free(errmsg);  // <--- обратите внимание не C-style работу с памятью
        } else {
            std::cout << "query ok\n";
        }
    }
    sqlite3 *handle = nullptr;
};