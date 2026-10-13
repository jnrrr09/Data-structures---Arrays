# Data-structures---Arrays
A C++ console app that records daily classroom attendance in a 2D array and reports per-student rates, daily totals and low-attendance alerts.



## ✨ What it does

Record **P**resent, **L**ate or **A**bsent for each student every day, then get instant reports.

| Feature | What you get |
|---|---|
| ✅ Take attendance | Mark a whole class in seconds (Enter = Present) |
| 📊 View register | Full student × day grid with attendance % |
| 👤 Student report | Present, late and absent counts for one student |
| 📅 Day summary | Who was in class, and who was absent |
| 🚨 Alerts | Flags students below **75%** attendance |



## 👀 Preview

```text
Student        1  2  3  4  5  6  7  8  9  10   Att%
---------------------------------------------------
Ama            P  P  P  P  P  P  -  -  -  -   100%
Kofi           A  P  A  P  P  P  -  -  -  -    67%
Yaw            A  P  A  P  A  P  -  -  -  -    50%
Kwame          P  P  P  L  A  P  -  -  -  -    83%

P = present, L = late, A = absent, - = not taken
```

## 🧠 How it works

The whole register is one fixed-size 2D array:

```cpp
std::array<std::array<char, 10>, 30> mark;   // mark[student][day]
```

```text
            day 0  day 1  day 2  ...
student 0 [   P      P      L    ... ]
student 1 [   A      P      A    ... ]   ← row scan  = one student's history
student 2 [   P      P      P    ... ]
              ↑
        column scan = the whole class on one day
```

Any cell is reached in constant time, and both kinds of report are simple loops over a row or a column. That's why an array is the right fit.

## 📁 Files

| File | Purpose |
|---|---|
| `attendance_register.cpp` | The complete program |
| `arrays_assignment_report.pdf` | Report on array applications and this project's design |




