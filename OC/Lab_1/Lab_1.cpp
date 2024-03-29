/*
Вариант 26.
Написать функцию копирования одной строки, введенной
пользователем в другую.
*/

#include <iostream>

char *copy(char *to_copy, int &size)
{
    char *result = new char[size];
    for (int i = 0; i < size; ++i)
        result[i] = to_copy[i];
    return result;
}

void push_back(char *&string, int &size, char symbol, int &i)
{
    if (i >= size)
    {
        char *buff_arr = new char[size * 2];
        for (int i = 0; i < size; ++i)
            buff_arr[i] = string[i];
        delete[] string;
        string = buff_arr;
        size *= 2;
    }
    string[i] = symbol;
    ++i;
}

void input(char *&string, int &size)
{
    std::cout << "Enter string to be copied: ";
    int i = 0;
    string = new char[size];
    char symbol;
    while (std::cin.get(symbol))
    {
        push_back(string, size, symbol, i);
        if (symbol == '\n')
            break;
    }
    size = i;
}

void output(char *&string, int &size)
{
    std::cout << "Copied string: ";
    for (int i = 0; i < size; ++i)
        std::cout << string[i];
}

int main()
{
    char answ;
    do
    {
        char *string;
        int size = 10;
        input(string, size);
        char *second_string = copy(string, size);
        output(second_string, size);
        delete[] string;
        delete[] second_string;
        std::cout << "Do you want to continue? y/n: ";
        std::cin >> answ;
        std::cin.ignore(256, '\n');
        while (answ != 'y' && answ != 'n')
        {
            std::cout << "Enter y or n!\n";
            std::cin >> answ;
            std::cin.ignore(256, '\n');
        }
    } while (answ == 'y');
}
