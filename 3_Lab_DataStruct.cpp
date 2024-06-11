// 4_Lab_DataStruct.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <iostream>
#include <list>
#include <chrono>
#include <locale.h>

using namespace std;

int N;  // Количество серых мышей
int M;  // Количество белых мышей
int S;  // Мышь по счёту, которую съедает кошка
int K;  // Число оставшихся серых мышей
int L;  // Число оставшихся белых мышей

void listSTL() {
    // реализация с использованием списка 
    list<int> mice; // список для хранения мышей
    int gray = 0; // количество серых мышей
    int white = 0; // количество белых мышей

    // инициализация списка
    for (int i = 0; i < N; i++) {
        mice.push_back(1); // серая мышь
        gray++;
    }
    for (int i = 0; i < M; i++) {
        mice.push_back(0); // белая мышь
        white++;
    }

    // кошка ходит по кругу
    int index = 0;
    while (gray > K || white > L) {
        list<int>::iterator it = mice.begin();
        advance(it, index);
        if (*it == 1) {
            gray--;
        }
        else {
            white--;
        }
        mice.erase(it);
        index = (index + S - 1) % mice.size();
    }

    // вывод результата
    cout << "Остались мыши в следующем порядке:" << endl;
    for (auto it = mice.begin(); it != mice.end(); ++it) {
        cout << (*it == 1 ? "серая" : "белая") << " ";
    }
    cout << endl;
}

struct Mish {
    bool onaBelaya; // белая - true, серая - false
    Mish* next;     // указатель на следующую мышь в кругу 
};

// Инициализация круга мышей
void Krug(Mish*& head, int N, int M) {
    head = new Mish{ true, nullptr };     // Создал 1-ю серую мышь
    Mish* current = head;
    for (int i = 0; i < M; i++)         // Белые мыши вошли в чат
    {
        current->next = new Mish{ false, nullptr };
        current = current->next;
    }
    for (int i = 0; i < N; i++)         // Серые мыши вошли в чат
    {
        current->next = new Mish{ true, nullptr };
        current = current->next;
    }
    current->next = head;               // Закрываю круг, связывая 1-ю и последнюю мышей
}

// Едим мышей из круга
Mish* emMish(Mish* head, int S) {
    Mish* current = head;
    Mish* prev = nullptr;
    for (int i = 0; i < S - 1; i++)     // Поиск мыши, которую собираемся съесть
    {
        prev = current;
        current = current->next;
    }
    while (current != head)              // Ем(удаляю) мышей из круга
    {
        prev->next = current->next;
        delete current;
        current = prev->next;
        for (int i = 0; i < S - 1; i++)     // Поиск мыши, которую собираемся съесть
        {
            prev = current;
            current = current->next;
        }
    }
    prev->next = current->next;
    delete current;
    return prev->next; // Возвращаем указатель на новую голову круга мышей
}

// Вывод порядка оставшихся мышей
void poryadokMishey(Mish* head, int K, int L) {
    int count = 0;
    Mish* current = head;
    cout << "Остались мыши в следующем порядке: \n";
    do {
        if (count < K + L) {
            cout << (current->onaBelaya ? "белая " : "серая ");
            count++;
        }
        current = current->next;
    } while (current != head);
    cout << endl;
}

void Massiv() {
    int* mice = new int[N + M]; // динамический массив для хранения мышей
    int gray = 0; // количество серых мышей
    int white = 0; // количество белых мышей

    // инициализация массива
    for (int i = 0; i < N; i++) {
        mice[i] = 1; // серая мышь
        gray++;
    }
    for (int i = N; i < N + M; i++) {
        mice[i] = 0; // белая мышь
        white++;
    }

    // кошка ходит по кругу
    int index = 0;
    while (gray > K || white > L) {
        if (mice[index] == 1) {
            gray--;
        }
        else {
            white--;
        }
        for (int i = index; i < N + M - 1; i++) {
            mice[i] = mice[i + 1];
        }
        index = (index + S - 1) % (N + M - 1);
    }

    // вывод результата
    cout << "Остались мыши в следующем порядке:" << endl;
    for (int i = 0; i < N + M - (N + M - K - L); i++) {
        cout << (mice[i] == 1 ? "серая" : "белая") << " ";
    }
    cout << endl;

    delete[] mice; // освобождение памяти
}

int main() {
    int vibor;
    setlocale(LC_ALL, "Russian");

    cout << "Количество белых мышей: ";
    cin >> M;
    cout << "Количество серых мышей: ";
    cin >> N;
    cout << "Шаг кошки: ";
    cin >> S;
    cout << "Количество оставшихся серых мышей: ";
    cin >> K;
    cout << "Количство оставшихся белых мышей: ";
    cin >> L;
    do {
        cout << "\nВыбор способа: " << endl;
        cout << "1) STL" << endl;
        cout << "2) односвязный список" << endl;
        cout << "3) динамический массив" << endl;
        cin >> vibor;
        switch (vibor)
        {
        case 1: {
            auto start = chrono::high_resolution_clock::now();
            listSTL();
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> diff = end - start;
            double flops = (N + M) / diff.count();
            cout << "Время выполнения: " << diff.count() << " секунд" << endl;
            cout << "Производительность: " << flops << " Mflops" << endl;
            break;
        }
        case 2: {
            auto start = chrono::high_resolution_clock::now();
            Mish* head = nullptr;
            Krug(head, N, M);
            head = emMish(head, S);         // Присваиваем возвращаемое значение функции emMish переменной head
            poryadokMishey(head, K, L);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> diff = end - start;
            double flops = (N + M) / diff.count();
            cout << "Время выполнения: " << diff.count() << " секунд" << endl;
            cout << "Производительность: " << flops << " Mflops" << endl;
            break;
        }
        case 3: {
            auto start = chrono::high_resolution_clock::now();
            Massiv();
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> diff = end - start;
            double flops = (N + M) / diff.count();
            cout << "Время выполнения: " << diff.count() << " секунд" << endl;
            cout << "Производительность: " << flops << " Mflops" << endl;
            break;
        }
        }
    } while ((vibor > 0) && (vibor < 4));
    return 0;
}
// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
