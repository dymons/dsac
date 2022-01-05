# Собеседования

## Введение

Репозиторий содержит перечень рекомендация для прохождения собеседований в IT компании, такие как: Yandex, Amazon, Kaspersky, Mail.ru,
Avito, Parallels, VK, Sber. Список тем был сформирован для разработчиков на языке С++.

## Обзор тем для подготовки к собеседованию

1. Анализ учетных стоимостей
    - Примеры моделей вычисления: машина Тьюринга, RAM-машина
    - Сложность в среднем и худшем случаях
    - Анализ мультипликативной схемы для массива переменного размера с помощью банковского метода
    - Анализ учетных стоимостей операций: функция потенциала, истинные и учетные стоимости
2. Порядковая статистика
3. Знание реализации и нотации Big O алгоритмов сортировки
    - Сортировка пузырьком ([Bubble sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/bubble_sort.hpp))
    - Сортировка перемешиванием ([Shaker sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/shaker_sort.hpp))
    - Сортировка вставкой ([Insertion sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/insertion_sort.hpp))
    - Сортировка выбором ([Selection sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/selection_sort.hpp))
    - Быстрая сортировка ([Quick sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/quick_sort.hpp))
    - Сортировка слиянием ([Merge sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/merge_sort.hpp))
    - Сортировка с помощью двоичного
      дерева ([Binary-tree sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/binary_tree_sort.hpp))
    - Сортировка Шелла ([Shell sort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/shell_sort.hpp))
    - Пирамидальная сортировка ([Heapsort](https://github.com/dymons/interviews/blob/master/algorithms/sorts/heap_sort.hpp))
4. Алгоритмы для работы с графами
    - Поиск в ширину ([Breadth-first search](https://github.com/dymons/interviews/blob/master/structures/graph/breadth_first_search.hpp))
    - Поиск в грубину ([Depth-first search](https://github.com/dymons/interviews/blob/master/structures/graph/depth_first_search.hpp))
    - Алгоритм Дейкстры ([Dijkstra’s algorithm](https://github.com/dymons/interviews/blob/master/structures/graph/dijkstra_search.hpp))
    - Задача
      коммивояжёра ([Travelling salesman problem](https://github.com/dymons/interviews/blob/master/structures/graph/travelling_salesman_problem.hpp))
5. Структуры данных и их алгоритмическая сложность
    - Динамический массив (Dynamic Array)
    - Статический массив (Static Array)
    - Двусвязный список (Linked List)
    - Односвязный список (Forward Linked List)
    - Графы (Graphs)
    - Бинарное дерево поиска (Binary Shortest Three)
    - Красно-черное дерево ([Red-black tree](https://github.com/dymons/interviews/blob/master/structures/tree/rb_tree.hpp))
    - АВЛ-дерево ([AVL-Tree](https://github.com/dymons/interviews/blob/master/structures/tree/avl_tree.hpp))
    - Декартовы деревья / Treap / Дуча / Дерамида (Cartesian Tree)
    - B дерево ([B Tree](https://github.com/dymons/interviews/blob/master/structures/tree/b_tree.hpp))
    - B+ дерево (B+ Tree)
    - Стеки (Stack)
    - Очереди (Queue)
    - Двусторонняя очередь (Deque)
    - Binary Heap ([Binary Heap](https://github.com/dymons/interviews/blob/master/structures/heap/binary_heap.hpp))
    - K-ary Heap
    - Meldable Heap
    - Binomial Heap
    - Leftist Heap
    - Хэш таблица ([Hash Tables](https://github.com/dymons/interviews/blob/master/structures/hashtable/hashtable.hpp))
6. Знание умных указателей и их реализации
    - Shared pointer ([Source](https://github.com/dymons/interviews/blob/master/cpp/pointers/shared_ptr.hpp))
    - Unique pointer
    - Weak pointer
    - Auto pointer
    - Scope pointer
    - Intrusive pointer
7. Concurrency
    - В чем отличия процесса от потока?
    - Примитивы синхронизации
        1. Mutex ([Mutual Exclusion](https://github.com/dymons/interviews/blob/master/concurrency/syncing/mutex.hpp))
        2. Futex ([Source](https://github.com/dymons/interviews/blob/master/concurrency/syncing/futex.hpp))
        3. Condition Variable
        4. Critical Section ([Source](https://github.com/dymons/interviews/blob/master/concurrency/syncing/critical_section.hpp))
        5. Barrier ([Source](https://github.com/dymons/interviews/blob/master/concurrency/syncing/barrier.hpp))
        6. Latch
        7. Event
        8. Semaphore ([Source](https://github.com/dymons/interviews/blob/master/concurrency/syncing/semaphore.hpp))
    - Способы межпроцессорного взаимодействия (IPC), перечислить все 12 способов.
    - Что такое deadlock? Примеры и как избежать подобных ситуаций? (Priority Mutex)
    - Какой способ распараллеливания задач лучше - по процессам или по потокам?
    - Пул управляемых
      потоков ([Thread pool](https://github.com/dymons/interviews/blob/master/concurrency/thread_pool/static_thread_pool.hpp))
    - Модель памяти С++
8. Паттерны проектирования и идиомы
    - Концепции RAII
    - Концепции SOLID
    - Набор паттернов проектирования ([22 паттерна](https://refactoring.guru/ru/design-patterns/catalog))
        1. Порождающие паттерны проектирования
        2. Структурные паттерны проектирования
        3. Поведенческие паттерны проектирования
9. Методы обработки TCP соединений
    - select ([Source](https://github.com/dymons/interviews/blob/master/network/server_select.cpp))
    - poll
    - epoll

## Материалы для изучения
- Алгоритмы и структуры данных поиска. Лекции и курсы от Яндекса [Link](https://habr.com/ru/company/yandex/blog/208716/)
- Алгоритмы и структуры данных by Павел Маврин. [YouTube](https://www.youtube.com/c/pavelmavrin/videos)
- Теория и практика многопоточной синхронизации by Роман Липовский. [YouTube](https://www.youtube.com/playlist?list=PL4_hYwCyhAvb7P8guwSTaaUS8EcOaWjxF)