#include "clinic/demo.hpp"
#include "clinic/storage.hpp"

#include <iostream>
#include <string>

int main() {
    constexpr const char* databasePath = "clinic_queue.db";

    try {
        auto storage = clinic::createStorage(databasePath);
        storage.sync_schema();
        clinic::seedDemoData(storage);

        clinic::SqliteDoctorRepository doctorRepository(storage);
        clinic::DoctorRepository& doctors = doctorRepository;
        clinic::TicketRepository tickets(storage);

        std::cout << "База даних поліклініки готова: " << databasePath << '\n';
        clinic::runRepositoryAndQueryDemo(doctors, tickets, storage);
        clinic::runTransactionDemo(storage);
        clinic::runSqliteApiComparison(doctors, databasePath);
        std::cout << "\nУсі демонстраційні сценарії завершено успішно.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Помилка роботи застосунку: " << error.what() << '\n';
        return 1;
    }
}
