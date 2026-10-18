# Лабораторные работы по курсу «Дискретный анализ» в МАИ

В репозитории собраны решения на C++ и отчёты с условиями, описанием алгоритмов и результатами экспериментов

## Лабораторные работы

### Первый семестр — весна 2026

| № | Тема | Решение | Отчёт |
|---|---|---|---|
| 1 | **Карманная сортировка** | [C++](semester_1_spring_2026/lab1/lab1.cpp) | [PDF](semester_1_spring_2026/lab1/lab1.pdf), [LaTeX](semester_1_spring_2026/lab1/lab1.tex) |
| 2 | **Красно-чёрное дерево** | [C++](semester_1_spring_2026/lab2/lab2.cpp) | [PDF](semester_1_spring_2026/lab2/lab2.pdf), [LaTeX](semester_1_spring_2026/lab2/lab2.tex) |
| 3 | **Профилирование и проверка памяти(gprof и dmalloc)** | [Команды](semester_1_spring_2026/lab3/commands.txt) | [PDF](semester_1_spring_2026/lab3/lab3.pdf) |
| 4 | **Алгоритм Ахо–Корасик** | [C++](semester_1_spring_2026/lab4/lab4.cpp) | [PDF](semester_1_spring_2026/lab4/lab4.pdf), [LaTeX](semester_1_spring_2026/lab4/lab4.tex) |
| 5 | **Суффиксный массив** | [C++](semester_1_spring_2026/lab5/lab5.cpp) | [PDF](semester_1_spring_2026/lab5/lab5.pdf), [LaTeX](semester_1_spring_2026/lab5/lab5.tex) |

## Сборка и запуск

Каждое решение - отдельная консольная программа. Нужен компилятор с поддержкой C++17. Ввод и вывод выполняются через стандартные потоки

Пример для ЛР №5 из корня репозитория:

```bash
g++ -std=c++17 -O2 semester_1_spring_2026/lab5/lab5.cpp -o lab5
printf '%s\n' 'banana' 'ana' 'na' | ./lab5
```

Результат:

```text
1: 2, 4
2: 3, 5
```

### Особенности:
- В ЛР №1 есть автоматические бенчмарки RunBenchmark, которые можно раскомментировать
- Команды в ЛР №3 рассчитаны на Linux пути
