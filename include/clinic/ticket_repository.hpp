#pragma once

#include "clinic/storage.hpp"

#include <optional>
#include <vector>

namespace clinic {

class TicketRepository {
public:
    explicit TicketRepository(ClinicStorage& storage);

    int add(AppointmentTicket ticket);
    std::optional<AppointmentTicket> getById(int id) const;
    std::vector<AppointmentTicket> getAll() const;

private:
    ClinicStorage& storage_;
};

}
