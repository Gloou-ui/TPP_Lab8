#include "clinic/demo.hpp"
#include "clinic/sqlite_api_demo.hpp"

#include <sqlite_orm/sqlite_orm.h>

#include <array>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace clinic {

void seedDemoData(ClinicStorage& storage) {
    storage.remove_all<AppointmentTicket>();
    storage.remove_all<Doctor>();

    const std::array<Doctor, 5> doctors{{
        {0, "Олена Коваль", "Терапевт", 101, "+380501110101"},
        {0, "Андрій Мельник", "Кардіолог", 202, "+380502220202"},
        {0, "Ірина Шевченко", "Педіатр", 103, "+380503330303"},
        {0, "Богдан Ткаченко", "Невролог", 204, "+380504440404"},
        {0, "Марія Бондар", "Терапевт", 105, "+380505550505"},
    }};
    std::array<int, 5> doctorIds{};
    std::size_t doctorIndex = 0;
    for (const auto& doctor : doctors) {
        doctorIds[doctorIndex++] = static_cast<int>(storage.insert(doctor));
    }

    const std::array<AppointmentTicket, 15> tickets{{
        {0, 0, "Наталія Іваненко", "2026-11-12 08:30", 1, 20},
        {0, 0, "Петро Петренко", "2026-11-12 08:50", 2, 15},
        {0, 0, "Оксана Лисенко", "2026-11-12 09:05", 3, 25},
        {0, 0, "Віктор Савчук", "2026-11-12 09:00", 1, 30},
        {0, 0, "Людмила Мороз", "2026-11-12 09:30", 2, 25},
        {0, 0, "Денис Кравець", "2026-11-12 09:55", 3, 20},
        {0, 0, "Аліна Романюк", "2026-11-12 10:00", 1, 20},
        {0, 0, "Максим Поліщук", "2026-11-12 10:20", 2, 15},
        {0, 0, "Софія Гриценко", "2026-11-12 10:35", 3, 20},
        {0, 0, "Юрій Климчук", "2026-11-12 11:00", 1, 30},
        {0, 0, "Катерина Дяченко", "2026-11-12 11:30", 2, 25},
        {0, 0, "Роман Остапенко", "2026-11-12 11:55", 3, 20},
        {0, 0, "Валентина Шульга", "2026-11-12 12:00", 1, 15},
        {0, 0, "Ігор Черненко", "2026-11-12 12:15", 2, 20},
        {0, 0, "Дарина Павленко", "2026-11-12 12:35", 3, 25},
    }};
    for (std::size_t index = 0; index < tickets.size(); ++index) {
        auto ticket = tickets[index];
        ticket.doctorId = doctorIds[index / 3];
        storage.insert(ticket);
    }
}

void runRepositoryAndQueryDemo(DoctorRepository& doctors,
                               TicketRepository& tickets,
                               ClinicStorage& storage) {
    std::cout << "\n=== Репозиторій лікарів ===\n";
    const auto allDoctors = doctors.getAll();
    const auto doctorCount = allDoctors.size();
    std::cout << "Лікарів у базі: " << doctorCount << '\n';

    if (allDoctors.empty()) {
        throw std::runtime_error("Початковий набір лікарів порожній.");
    }
    const auto doctor = doctors.getById(allDoctors.front().id);
    if (!doctor) {
        throw std::runtime_error("Першого лікаря не знайдено за його ID.");
    }
    std::cout << "Лікар за ID " << doctor->id << ": " << doctor->fullName
              << ", "
              << doctor->specialty << '\n';

    auto updatedDoctor = *doctor;
    updatedDoctor.phone = "+380501110199";
    doctors.update(updatedDoctor);
    const auto persistedUpdate = doctors.getById(doctor->id);
    if (!persistedUpdate) {
        throw std::runtime_error("Оновленого лікаря не знайдено.");
    }
    std::cout << "Оновлений телефон: " << persistedUpdate->phone << '\n';
    doctors.update(*doctor);

    const auto temporaryDoctorId =
        doctors.add({0, "Тестовий лікар", "Тестова спеціальність", 999,
                     "+380000000000"});
    doctors.remove(temporaryDoctorId);
    std::cout << "Додавання та видалення тестового лікаря: виконано.\n";

    std::cout << "\n=== Фільтрація ===\n";
    const auto therapists =
        storage.get_all<Doctor>(
            sqlite_orm::where(sqlite_orm::c(&Doctor::specialty) ==
                              "Терапевт"));
    for (const auto& therapist : therapists) {
        std::cout << therapist.fullName << " — кабінет "
                  << therapist.roomNumber << '\n';
    }

    std::cout << "\n=== Сортування талонів за часом (останні 5) ===\n";
    const auto latestTickets =
        storage.get_all<AppointmentTicket>(
            sqlite_orm::order_by(&AppointmentTicket::appointmentTime).desc(),
            sqlite_orm::limit(5));
    for (const auto& ticket : latestTickets) {
        std::cout << ticket.appointmentTime << " — "
                  << ticket.patientName << ", талон №"
                  << ticket.queueNumber << '\n';
    }

    std::cout << "\n=== Агрегація ===\n";
    const auto averageDuration = storage.avg(&AppointmentTicket::durationMinutes);
    std::cout << std::fixed << std::setprecision(1)
              << "Середня тривалість прийому: " << averageDuration
              << " хвилин\n";

    std::cout << "\n=== JOIN лікарів і талонів ===\n";
    const auto appointments =
        storage.select(
            sqlite_orm::columns(&Doctor::fullName,
                                &AppointmentTicket::patientName,
                                &AppointmentTicket::appointmentTime,
                                &AppointmentTicket::durationMinutes),
            sqlite_orm::inner_join<AppointmentTicket>(
                sqlite_orm::on(sqlite_orm::c(&Doctor::id) ==
                               &AppointmentTicket::doctorId)),
            sqlite_orm::order_by(&AppointmentTicket::appointmentTime),
            sqlite_orm::limit(5));
    for (const auto& [doctorName, patientName, appointmentTime, duration] :
         appointments) {
        std::cout << doctorName << " → " << patientName << ", "
                  << appointmentTime << ", " << duration << " хв\n";
    }

    std::cout << "Талонів у базі: " << tickets.getAll().size() << '\n';
}

void runTransactionDemo(ClinicStorage& storage) {
    std::cout << "\n=== Транзакція: реєстрація лікаря з першим талоном ===\n";
    const auto doctorsBefore = storage.count<Doctor>();
    const auto ticketsBefore = storage.count<AppointmentTicket>();
    std::cout << "До: лікарів " << doctorsBefore << ", талонів "
              << ticketsBefore << '\n';

    const bool committed = storage.transaction([&storage] {
        const auto doctorId = storage.insert(
            Doctor{0, "Транзакційний лікар", "Офтальмолог", 306,
                   "+380507770707"});
        storage.insert(AppointmentTicket{0, static_cast<int>(doctorId),
                                         "Тестовий пацієнт",
                                         "2026-11-13 08:00", 1, 20});
        return true;
    });
    if (!committed) {
        throw std::runtime_error("Транзакція реєстрації не була зафіксована.");
    }

    const auto doctorsAfterSuccess = storage.count<Doctor>();
    const auto ticketsAfterSuccess = storage.count<AppointmentTicket>();
    std::cout << "Після успіху: лікарів " << doctorsAfterSuccess
              << ", талонів " << ticketsAfterSuccess << '\n';
    if (doctorsAfterSuccess != doctorsBefore + 1 ||
        ticketsAfterSuccess != ticketsBefore + 1) {
        throw std::runtime_error("Успішна транзакція не зберегла обидва записи.");
    }

    std::cout << "\n=== Транзакція: навмисне порушення зовнішнього ключа ===\n";
    const auto doctorsBeforeRollback = storage.count<Doctor>();
    const auto ticketsBeforeRollback = storage.count<AppointmentTicket>();
    bool constraintErrorObserved = false;

    try {
        storage.transaction([&storage] {
            storage.insert(Doctor{0, "Лікар для відкату", "Дерматолог", 307,
                                  "+380508880808"});
            storage.insert(AppointmentTicket{
                0, 999999, "Пацієнт без лікаря", "2026-11-13 08:30", 1, 15});
            return true;
        });
    } catch (const std::exception& error) {
        constraintErrorObserved =
            std::string(error.what()).find("FOREIGN KEY constraint failed") !=
            std::string::npos;
        std::cout << "Очікувана помилка БД: " << error.what() << '\n';
    }

    const auto doctorsAfterRollback = storage.count<Doctor>();
    const auto ticketsAfterRollback = storage.count<AppointmentTicket>();
    std::cout << "До помилкової транзакції: лікарів "
              << doctorsBeforeRollback << ", талонів "
              << ticketsBeforeRollback << '\n';
    std::cout << "Після відкату: лікарів " << doctorsAfterRollback
              << ", талонів " << ticketsAfterRollback << '\n';

    if (!constraintErrorObserved ||
        doctorsAfterRollback != doctorsBeforeRollback ||
        ticketsAfterRollback != ticketsBeforeRollback) {
        throw std::runtime_error(
            "Не вдалося підтвердити відкат усіх змін транзакції.");
    }
    std::cout << "Атомарність підтверджено: перший INSERT також скасовано.\n";
}

void runSqliteApiComparison(DoctorRepository& doctors,
                            const std::string& databasePath) {
    std::cout << "\n=== SQLite C API та sqlite_orm ===\n";
    const Doctor sample{0, "Лікар C API", "Діагност", 401,
                        "+380509990909"};

    SqliteApiDemo sqliteApi(databasePath);
    const auto cApiId = sqliteApi.addDoctor(sample);
    const auto cApiDoctor = sqliteApi.getDoctorById(cApiId);
    if (!cApiDoctor) {
        throw std::runtime_error("SQLite C API не повернув доданого лікаря.");
    }
    std::cout << "C API: додано та прочитано лікаря ID " << cApiDoctor->id
              << " — " << cApiDoctor->fullName << '\n';
    doctors.remove(cApiId);

    const auto ormId = doctors.add(sample);
    const auto ormDoctor = doctors.getById(ormId);
    if (!ormDoctor) {
        throw std::runtime_error("sqlite_orm не повернув доданого лікаря.");
    }
    std::cout << "sqlite_orm: додано та прочитано лікаря ID "
              << ormDoctor->id << " — " << ormDoctor->fullName << '\n';
    doctors.remove(ormId);
}

}
