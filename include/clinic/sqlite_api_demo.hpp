#pragma once

#include "clinic/models.hpp"

#include <memory>
#include <optional>
#include <string>

namespace clinic {

class SqliteApiDemo {
public:
    explicit SqliteApiDemo(const std::string& databasePath);
    ~SqliteApiDemo();

    SqliteApiDemo(const SqliteApiDemo&) = delete;
    SqliteApiDemo& operator=(const SqliteApiDemo&) = delete;

    int addDoctor(const Doctor& doctor);
    std::optional<Doctor> getDoctorById(int id) const;

private:
    struct Connection;
    std::unique_ptr<Connection> connection_;
};

}
