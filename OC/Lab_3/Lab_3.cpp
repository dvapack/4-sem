#include <iostream>
#include <string>
#include <cmath>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

struct FunctionData
{
    double x;
    double y;
};

int pipe_in[2];
int pipe_out[2];
pid_t pid;

void help()
{
    std::cout << "Instruction:\n"
              << "To input numbers start the programm without any keys\n";
}

bool check_double(const std::string &check, bool minus_found)
{
    if (check.empty()) // для отслеживания пустой строки
        return false;
    bool dot_found = false;
    for (int i = 0; i < check.length(); i++)
    {
        char symbol = check[i];
        if (symbol == '-')
        {
            if (minus_found && i != 0)
                return false;
            minus_found = true;
        }
        if (symbol == '.')
        {
            if (dot_found == true)
                return false;
            dot_found = true;
        }
        if (!isdigit(symbol) && symbol != '.' && symbol != '-') // для отслеживания букв
            return false;
        if (isspace(symbol) != 0) // для отслеживания пробелов в строке
            return false;
    }
    return true;
}

bool check_borders(const std::string &check)
{
    return stod(check) > 2;
}

bool check_pi_format(const std::string &check)
{
    if (check.empty() || check.length() < 1) // для отслеживания пустой строки
        return false;
    bool minus_found = false;
    short i = 0;
    if (check[0] == '-')
    {
        minus_found = true;
        i = 1;
    }
    if (!minus_found && check[0] == '0')
        return check_double(check, minus_found);
    if (check.length() < 4 || check[i] != 'p' || check[i + 1] != 'i' || check[i + 2] != '/')
        return false;
    return check_double(minus_found ? check.substr(4, std::string::npos) : check.substr(3, std::string::npos), minus_found) && 
            check_borders(minus_found ? check.substr(4, std::string::npos) : check.substr(3, std::string::npos));
}

bool check_string(const std::string &check, bool type)
{
    return type ? check_pi_format(check) : check_double(check, false);
}

void input(double &x, double &y)
{
    std::string inp;
    do
    {
        std::cout << "Input double x: ";
        std::cin >> inp;
    } while (!check_string(inp, false));
    x = stod(inp);
    do
    {
        std::cout << "Input y ( -pi/2 < y < pi/2 ): ";
        std::cin >> inp;
    } while (!check_string(inp, true));
    y = inp[0] != '0' ? M_PI / stod(inp[0] == '-' ? inp.substr(4, std::string::npos) : inp.substr(3, std::string::npos)) : stod(inp);
}

double calc(const double &x, const double &y)
{
    double result = 0;
    double factorial = x + 1;
    for (int i = 0; i <= 500; ++i)
    {
        result += (sin(pow(x, i)) + tan(y)) / (factorial);
        factorial *= (i + x > 0) ? i + x : 1;
    }
    return result;
}

void backend()
{
    FunctionData data;
    read(pipe_in[0], &data, sizeof(FunctionData));
    double result = calc(data.x, data.y);
    write(pipe_out[1], &result, sizeof(double));
}

void frontend()
{
    FunctionData data;
    input(data.x, data.y);
    write(pipe_in[1], &data, sizeof(FunctionData));
    double result;
    read(pipe_out[0], &result, sizeof(double));
    std::cout << "Result: " << result << std::endl
              << ">_<\n";
    exit(0);
}



int main(int argc, char const *argv[])
{
    setlocale(LC_ALL, 0);
    if (argc == 2 && !strcmp(argv[1], "--help"))
    {
        help();
        exit(0);
    }
    else if (argc != 1)
    {
        std::cout << "Type --help for instruction\n";
        exit(1);
    }
    else
    {
        pipe(pipe_in);
        pipe(pipe_out);
        pid = fork();
        if (pid < 0)
        {
            std::cerr << "Critical issue!\n";
            exit(1);
        }
        else if (pid > 0)
        {
            frontend();
        }
        else
        {
            backend();
        }
        for (int i = 0; i < 2; ++i)
        {
            close(pipe_in[i]);
            close(pipe_out[i]);
        }
        return 0;
    }
}