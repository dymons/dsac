# Собеседования

## Введение

Репозиторий содержит перечень рекомендация для прохождения собеседований в IT компании, такие как: Yandex, Amazon, Kaspersky, Mail.ru,
Avito, Parallels, VK, Sber. Список тем был сформирован для разработчиков на языке С++.

## Обзор тем для подготовки к собеседованию

1. Знание реализации и нотации Big O алгоритмов сортировки
    - Сортировка
      пузырьком ([Bubble sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/bubble_sort.hpp))
    - Сортировка
      перемешиванием ([Shaker sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/shaker_sort.hpp))
    - Сортировка
      вставкой ([Insertion sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/insertion_sort.hpp))
    - Сортировка
      выбором ([Selection sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/selection_sort.hpp))
    - Быстрая
      сортировка ([Quick sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/quick_sort.hpp))
    - Сортировка
      слиянием ([Merge sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/merge_sort.hpp))
    - Сортировка с помощью двоичного
      дерева ([Binary-tree sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/binary_tree_sort.hpp))
    - Сортировка
      Шелла ([Shell sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/shell_sort.hpp))
    - Пирамидальная
      сортировка ([Heapsort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/heap_sort.hpp))
2. Алгоритмы для работы с графами
    - Поиск в ширину ([Breadth-first search](https://github.com/dymons/interviews/blob/master/structures/graph/breadth_first_search.hpp))
    - Поиск в грубину ([Depth-first search](https://github.com/dymons/interviews/blob/master/structures/graph/depth_first_search.hpp))
    - Алгоритм Дейкстры ([Dijkstra’s algorithm](https://github.com/dymons/interviews/blob/master/structures/graph/dijkstra_search.hpp))
    - Задача коммивояжёра ([Travelling salesman problem](https://github.com/dymons/interviews/blob/master/structures/graph/travelling_salesman_problem.hpp))
3. Структуры данных и их алгоритмическая сложность
    - Динамический массив (Dynamic Array)
    - Статический массив (Static Array)
    - Двусвязный список (Linked List)
    - Односвязный список (Forward Linked List)
    - Графы (Graphs)
    - Бинарное дерево поиска (Binary Shortest Three)
    - Красно-черное дерево ([Red-black tree](https://github.com/dymons/interviews/blob/master/structures/tree/rb_tree.hpp))
    - АВЛ-дерево ([AVL-Tree](https://github.com/dymons/interviews/blob/master/structures/tree/avl_tree.hpp))
    - Стеки (Stack)
    - Очереди (Queue)
    - Двусторонняя очередь (Deque)
    - Куча ([Heap](https://github.com/dymons/interviews/blob/master/structures/heap/heap.hpp))
    - Хэш таблица ([Hash Tables](https://github.com/dymons/interviews/blob/master/structures/hashtable/hashtable.hpp))
4. Знание умных указателей и их реализации
    - Shared pointer ([Source](https://github.com/dymons/interviews/blob/master/cpp/pointers/shared_ptr.hpp))
    - Unique pointer
    - Weak pointer
    - Auto pointer
    - Scope pointer
5. Concurrency
    - В чем отличия процесса от потока?
    - Примитивы синхронизации
        1. Mutex (Mutual Exclusion)
        2. Futex
        3. Condition Variable
        4. Critical Section
        5. Barrier
        6. Latch
        7. Event
        8. Semaphore
    - Способы межпроцессорного взаимодействия (IPC), перечислить все 12 способов.
    - Что такое deadlock? Примеры и как избежать подобных ситуаций? (Priority Mutex)
    - Какой способ распараллеливания задач лучше - по процессам или по потокам?
6. Паттерны проектирования и идиомы
    - Концепции RAII
    - Концепции SOLID
    - Набор паттернов проектирования ([22 паттерна](https://refactoring.guru/ru/design-patterns/catalog))
        1. Порождающие паттерны проектирования
        2. Структурные паттерны проектирования
        3. Поведенческие паттерны проектирования

## Что почитать

## Практика