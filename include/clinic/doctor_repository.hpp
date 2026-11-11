#pragma once

#include "clinic/storage.hpp"

#include <optional>
#include <vector>

namespace clinic {

class DoctorRepository {
public:
    virtual ~DoctorRepository() = default;

    virtual int add(Doctor doctor) = 0;
    virtual std::optional<Doctor> getById(int id) const = 0;
    virtual std::vector<Doctor> getAll() const = 0;
    virtual void update(const Doctor& doctor) = 0;
    virtual void remove(int id) = 0;
};

class SqliteDoctorRepository final : public DoctorRepository {
public:
    explicit SqliteDoctorRepository(ClinicStorage& storage);

    int add(Doctor doctor) override;
    std::optional<Doctor> getById(int id) const override;
    std::vector<Doctor> getAll() const override;
    void update(const Doctor& doctor) override;
    void remove(int id) override;

private:
    ClinicStorage& storage_;
};

}
