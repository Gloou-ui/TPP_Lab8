#pragma once

#include "clinic/models.hpp"

#include <sqlite_orm/sqlite_orm.h>

#include <string>
#include <utility>

namespace clinic {

inline auto createStorage(std::string databasePath) {
    using namespace sqlite_orm;

    return make_storage(
        std::move(databasePath),
        make_table("doctors",
                   make_column("id", &Doctor::id, primary_key().autoincrement()),
                   make_column("full_name", &Doctor::fullName),
                   make_column("specialty", &Doctor::specialty),
                   make_column("room_number", &Doctor::roomNumber),
                   make_column("phone", &Doctor::phone)),
        make_table("appointment_tickets",
                   make_column("id", &AppointmentTicket::id,
                               primary_key().autoincrement()),
                   make_column("doctor_id", &AppointmentTicket::doctorId),
                   make_column("patient_name", &AppointmentTicket::patientName),
                   make_column("appointment_time",
                               &AppointmentTicket::appointmentTime),
                   make_column("queue_number", &AppointmentTicket::queueNumber),
                   make_column("duration_minutes",
                               &AppointmentTicket::durationMinutes),
                   foreign_key(&AppointmentTicket::doctorId)
                       .references(&Doctor::id)
                       .on_delete.cascade()));
}

using ClinicStorage = decltype(createStorage(std::string{}));

}
