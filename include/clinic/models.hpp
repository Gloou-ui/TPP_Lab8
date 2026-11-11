#pragma once

#include <string>

namespace clinic {

struct Doctor {
    int id{};
    std::string fullName;
    std::string specialty;
    int roomNumber{};
    std::string phone;
};

struct AppointmentTicket {
    int id{};
    int doctorId{};
    std::string patientName;
    std::string appointmentTime;
    int queueNumber{};
    int durationMinutes{};
};

}
