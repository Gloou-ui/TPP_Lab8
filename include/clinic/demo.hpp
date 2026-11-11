#pragma once

#include "clinic/doctor_repository.hpp"
#include "clinic/ticket_repository.hpp"

#include <string>

namespace clinic {

void seedDemoData(ClinicStorage& storage);
void runRepositoryAndQueryDemo(DoctorRepository& doctors,
                               TicketRepository& tickets,
                               ClinicStorage& storage);
void runTransactionDemo(ClinicStorage& storage);
void runSqliteApiComparison(DoctorRepository& doctors,
                            const std::string& databasePath);

}
