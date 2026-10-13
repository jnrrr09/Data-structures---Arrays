// Daily Classroom Attendance Register
// Core data structure: a fixed-size 2D array, mark[student][day].
// Build: g++ -std=c++17 -O2 -Wall -Wextra attendance_register.cpp -o register

#include <algorithm>
#include <array>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

constexpr std::size_t kMaxStudents = 30;  // rows
constexpr std::size_t kDays = 10;         // columns (two school weeks)
constexpr double kAlertPercent = 75.0;    // below this, flag the student

constexpr char kPresent = 'P', kLate = 'L', kAbsent = 'A', kUnmarked = '-';

struct Register {
    std::array<std::string, kMaxStudents> names{};
    std::array<std::array<char, kDays>, kMaxStudents> mark{};
    std::size_t count = 0;  // students currently enrolled

    Register() { for (auto& row : mark) row.fill(kUnmarked); }

    bool addStudent(const std::string& name) {
        if (count == kMaxStudents || name.empty()) return false;
        names[count] = name;
        mark[count].fill(kUnmarked);
        ++count;
        return true;
    }
};

// ---- Calculations: each one is a pass over a row or a column ----

// Row scan: attendance percentage for one student (late counts as attended).
double studentPercent(const Register& r, std::size_t s) {
    int taken = 0, attended = 0;
    for (std::size_t d = 0; d < kDays; ++d) {
        if (r.mark[s][d] == kUnmarked) continue;
        ++taken;
        if (r.mark[s][d] != kAbsent) ++attended;
    }
    return taken ? 100.0 * attended / taken : 0.0;
}

struct DayTotals { int present = 0, late = 0, absent = 0, unmarked = 0; };

// Column scan: totals for one day across every student.
DayTotals dayTotals(const Register& r, std::size_t d) {
    DayTotals t;
    for (std::size_t s = 0; s < r.count; ++s) {
        switch (r.mark[s][d]) {
            case kPresent: ++t.present; break;
            case kLate:    ++t.late;    break;
            case kAbsent:  ++t.absent;  break;
            default:       ++t.unmarked; break;
        }
    }
    return t;
}

// ---- Input helpers ----

bool readLine(std::string& out) { return static_cast<bool>(std::getline(std::cin, out)); }

std::size_t askNumber(const std::string& prompt, std::size_t lo, std::size_t hi) {
    std::string line;
    while (true) {
        std::cout << prompt;
        if (!readLine(line)) std::exit(0);
        try {
            std::size_t v = std::stoul(line);
            if (v >= lo && v <= hi) return v;
        } catch (...) {}
        std::cout << "  Please enter a number from " << lo << " to " << hi << ".\n";
    }
}

// ---- Menu actions ----

void markDay(Register& r) {
    if (r.count == 0) { std::cout << "No students enrolled yet.\n"; return; }
    std::size_t d = askNumber("Day number (1-" + std::to_string(kDays) + "): ", 1, kDays) - 1;
    std::cout << "Enter P, L or A for each student (just press Enter for P).\n";
    for (std::size_t s = 0; s < r.count; ++s) {
        while (true) {
            std::cout << "  " << std::setw(2) << s + 1 << ". " << std::left << std::setw(12)
                      << r.names[s] << std::right << "[P/L/A]: ";
            std::string line;
            if (!readLine(line)) std::exit(0);
            char c = line.empty() ? kPresent : static_cast<char>(std::toupper(line[0]));
            if (c == kPresent || c == kLate || c == kAbsent) { r.mark[s][d] = c; break; }
            std::cout << "    Use P, L or A.\n";
        }
    }
    std::cout << "Day " << d + 1 << " recorded.\n";
}

void viewRegister(const Register& r) {
    if (r.count == 0) { std::cout << "No students enrolled yet.\n"; return; }
    std::cout << "\n" << std::left << std::setw(15) << "Student";
    for (std::size_t d = 0; d < kDays; ++d) std::cout << std::setw(3) << d + 1;
    std::cout << "  Att%\n" << std::string(15 + 3 * kDays + 6, '-') << '\n';
    for (std::size_t s = 0; s < r.count; ++s) {
        std::cout << std::left << std::setw(15) << r.names[s];
        for (std::size_t d = 0; d < kDays; ++d) std::cout << std::setw(3) << r.mark[s][d];
        std::cout << std::right << std::fixed << std::setprecision(0) << std::setw(4)
                  << studentPercent(r, s) << "%\n";
    }
    std::cout << "\nP = present, L = late, A = absent, - = not taken\n";
}

void studentReport(const Register& r) {
    if (r.count == 0) { std::cout << "No students enrolled yet.\n"; return; }
    for (std::size_t s = 0; s < r.count; ++s)
        std::cout << "  " << s + 1 << ". " << r.names[s] << '\n';
    std::size_t s = askNumber("Student number: ", 1, r.count) - 1;
    int p = 0, l = 0, a = 0;
    for (std::size_t d = 0; d < kDays; ++d) {
        if (r.mark[s][d] == kPresent) ++p;
        else if (r.mark[s][d] == kLate) ++l;
        else if (r.mark[s][d] == kAbsent) ++a;
    }
    std::cout << "\n" << r.names[s] << ": present " << p << ", late " << l << ", absent " << a
              << " -> " << std::fixed << std::setprecision(1) << studentPercent(r, s)
              << "% attendance\n";
}

void daySummary(const Register& r) {
    if (r.count == 0) { std::cout << "No students enrolled yet.\n"; return; }
    std::size_t d = askNumber("Day number (1-" + std::to_string(kDays) + "): ", 1, kDays) - 1;
    DayTotals t = dayTotals(r, d);
    if (t.unmarked == static_cast<int>(r.count)) { std::cout << "Day " << d + 1 << " has not been taken yet.\n"; return; }
    int here = t.present + t.late;
    std::cout << "\nDay " << d + 1 << ": " << t.present << " present, " << t.late << " late, "
              << t.absent << " absent -> " << std::fixed << std::setprecision(1)
              << 100.0 * here / static_cast<double>(r.count) << "% in class\n";
    if (t.absent > 0) {
        std::cout << "Absent: ";
        bool first = true;
        for (std::size_t s = 0; s < r.count; ++s)
            if (r.mark[s][d] == kAbsent) { std::cout << (first ? "" : ", ") << r.names[s]; first = false; }
        std::cout << '\n';
    }
}

void alerts(const Register& r) {
    bool any = false;
    std::cout << "\nStudents below " << kAlertPercent << "% attendance:\n";
    for (std::size_t s = 0; s < r.count; ++s) {
        double pct = studentPercent(r, s);
        bool hasData = std::any_of(r.mark[s].begin(), r.mark[s].end(), [](char c) { return c != kUnmarked; });
        if (hasData && pct < kAlertPercent) {
            std::cout << "  ! " << std::left << std::setw(12) << r.names[s] << std::right
                      << std::fixed << std::setprecision(1) << pct << "%\n";
            any = true;
        }
    }
    if (!any) std::cout << "  None. Everyone is on track.\n";
}

void addStudent(Register& r) {
    std::cout << "Student name: ";
    std::string name;
    if (!readLine(name)) std::exit(0);
    if (r.addStudent(name)) std::cout << "Added " << name << " (" << r.count << "/" << kMaxStudents << ").\n";
    else std::cout << "Could not add (empty name or class is full).\n";
}

void loadDemo(Register& r) {
    r = Register{};
    const std::array<std::string, 8> roster = {"Ama", "Kofi", "Efua", "Yaw", "Akosua", "Kwame", "Abena", "Nana"};
    const std::array<double, 8> absentChance = {0.02, 0.10, 0.05, 0.45, 0.00, 0.30, 0.08, 0.15};
    std::mt19937 rng(7);  // fixed seed: same demo every time
    std::uniform_real_distribution<double> roll(0.0, 1.0);
    for (const auto& n : roster) r.addStudent(n);
    for (std::size_t s = 0; s < r.count; ++s)
        for (std::size_t d = 0; d < 6; ++d) {
            double x = roll(rng);
            r.mark[s][d] = x < absentChance[s] ? kAbsent : (x < absentChance[s] + 0.08 ? kLate : kPresent);
        }
    std::cout << "Loaded 8 students with 6 days of sample attendance.\n";
}

int main() {
    Register reg;
    std::cout << "=== Daily Classroom Attendance Register ===\n";
    while (true) {
        std::cout << "\n1) Take attendance for a day   2) View register\n"
                     "3) Student report              4) Day summary\n"
                     "5) Low-attendance alerts       6) Add student\n"
                     "7) Load demo data              0) Exit\n";
        switch (askNumber("Choose: ", 0, 7)) {
            case 1: markDay(reg); break;
            case 2: viewRegister(reg); break;
            case 3: studentReport(reg); break;
            case 4: daySummary(reg); break;
            case 5: alerts(reg); break;
            case 6: addStudent(reg); break;
            case 7: loadDemo(reg); break;
            default: std::cout << "Goodbye.\n"; return 0;
        }
    }
}
