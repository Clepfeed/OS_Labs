# Вариант 2

1. 
```c++
DWORD WINAPI average(LPVOID data) 
{
    Avg* _avg = (Avg*)data;
    _avg->avg = 0;
    if (_avg->vec.empty()) return 0;

    for (size_t i = 0; i < _avg->vec.size(); i++) 
    {
        _avg->avg += _avg->vec[i];
        Sleep(12);
    }
    _avg->avg /= static_cast<int>(_avg->vec.size());
    std::cout << "Avg: " << _avg->avg << "\n";
    return 0;
}

DWORD WINAPI min_max(LPVOID data) 
{
    MinMax* _min_max = (MinMax*)data;
    if (_min_max->vec.empty()) return 0;

    _min_max->min = _min_max->vec[0];
    _min_max->max = _min_max->vec[0];
    for (size_t i = 1; i < _min_max->vec.size(); i++) 
    {
        _min_max->min = min(_min_max->min, _min_max->vec[i]);
        _min_max->max = max(_min_max->max, _min_max->vec[i]);
        Sleep(7);
    }
    std::cout << "Min: " << _min_max->min << " Max: " << _min_max->max << "\n";
    return 0;
}
```

2. Потоком в Windows называется объект ядра, которому операционная система выделяет процессорное время для выполнения приложения.

3. Критическая секция: это участок кода, который может выполняться только одним потоком одновременно, чтобы избежать конфликтов при доступе к общим ресурсам.

4. Семафор: это синхронизирующий объект, который управляет доступом к ресурсам с помощью счетчика, позволяя ограниченное число потоков одновременно выполнять код.

5. Пока не прочувствовал разницу, тк сделал мало лабодаторных с разными стандартами

## Общие вопросы:

1. Объектно-ориентированное программирование (ООП) — это парадигма программирования, основанная на концепции "объектов", которые содержат данные и методы для их обработки.

2. Это теорема, утверждающая, что количество объектов, которые человек может удерживать в рабочей памяти, составляет примерно 7 ± 2. Примеры из IT: количество элементов в меню, количество опций в диалоговых окнах, размер блоков кода, количество полей в формах, количество параметров в функциях, количество пользователей в группе, количество вкладок в браузере.

3. Энтропия ПО — это мера неопределенности и сложности системы. Примеры негэнтропийных мер: использование четких интерфейсов, модульное проектирование, автоматизированное тестирование, документация кода, стандартизация процессов разработки.

4. 5 признаков сложной системы по Гради Бучу:
    1) Многообразие компонентов: В проекте использовались разные базы данных и API.
    2) Непредсказуемость поведения: В тестах возникали неожиданные ошибки при интеграции.
    3) Чувствительность к начальным условиям: Изменение одной переменной влияло на производительность всей системы.
    4) Иерархическая структура: Проект имел модули с зависимостями, что усложняло изменения.
    5) Обратные связи: Пользовательские отзывы приводили к изменениям в функционале, что влияло на архитектуру.

5. Закон Седова утверждает, что при изменениях в одном уровне иерархии, другие уровни должны адаптироваться. Примеры из IT:

    1) Переход от локальных серверов к облачным вычислениям изменил подход к архитектуре приложений.
    2) Внедрение DevOps-методологий изменило процессы разработки и эксплуатации.
    3) Появление микросервисов изменило способы взаимодействия между компонентами.
    4) Разработка на основе API изменила подход к интеграции систем.
    5) Адаптация Agile-методов привела к изменениям в управлении проектами.