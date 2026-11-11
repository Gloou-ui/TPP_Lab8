#include "clinic/ticket_repository.hpp"

namespace clinic {

TicketRepository::TicketRepository(ClinicStorage& storage) : storage_(storage) {}

int TicketRepository::add(AppointmentTicket ticket) {
    return static_cast<int>(storage_.insert(ticket));
}

std::optional<AppointmentTicket> TicketRepository::getById(int id) const {
    return storage_.get_optional<AppointmentTicket>(id);
}

std::vector<AppointmentTicket> TicketRepository::getAll() const {
    return storage_.get_all<AppointmentTicket>();
}

}
