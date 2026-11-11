#include "clinic/doctor_repository.hpp"

#include <stdexcept>

namespace clinic {

SqliteDoctorRepository::SqliteDoctorRepository(ClinicStorage& storage)
    : storage_(storage) {}

int SqliteDoctorRepository::add(Doctor doctor) {
    return static_cast<int>(storage_.insert(doctor));
}

std::optional<Doctor> SqliteDoctorRepository::getById(int id) const {
    return storage_.get_optional<Doctor>(id);
}

std::vector<Doctor> SqliteDoctorRepository::getAll() const {
    return storage_.get_all<Doctor>();
}

void SqliteDoctorRepository::update(const Doctor& doctor) {
    if (!getById(doctor.id)) {
        throw std::runtime_error("Лікаря з таким ID не знайдено.");
    }
    storage_.update(doctor);
}

void SqliteDoctorRepository::remove(int id) {
    if (!getById(id)) {
        throw std::runtime_error("Лікаря з таким ID не знайдено.");
    }
    storage_.remove<Doctor>(id);
}

} // namespace clinic
