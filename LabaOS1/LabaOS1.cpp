#include <iostream>
#include <future> // для асинхронних операцій
#include <chrono> // для роботи зі спливальними точками
#include <random>
#include <thread>

std::string calculate(int id) {
    // Для генерації рандомного числа
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_int_distribution<> dist(1, 10);

    // Рандомні обчислення
    int result = dist(eng) * id;

    // Успіх або Таймаут помилка або неможливо обрахувати в інщих випадках
    if (result % 2 == 0) {
        return std::to_string(result);
    }
    else if (result % 3 == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(20)); // Симуляція довгого обрахунку
        return "Critical Error: Timeout";
    }
    else {
        return "Critical Error: Impossible Calculation";
    }
}

// XOR операції
std::string f_g(const std::string& result_f, const std::string& result_g) {
    if (result_f.find("Critical Error") != std::string::npos || result_g.find("Critical Error") != std::string::npos) {
        return "Critical Error: Invalid Input";
    }

    // Обчислення використовуючи XOR
    int value_f = std::stoi(result_f);
    int value_g = std::stoi(result_g);
    int result = value_f ^ value_g;

    return std::to_string(result);
}

// Функція для асинхронних f та g функцій
std::future<std::string> runAsyncFunction(std::function<std::string(int)> function, int id) {
    return std::async(std::launch::async, function, id);
}

int main() {
    auto future_f = runAsyncFunction(calculate, 1);
    auto future_g = runAsyncFunction(calculate, 2);

    // Очікування завершення
    std::chrono::seconds timeout(15);
    if (future_f.wait_for(timeout) == std::future_status::timeout || future_g.wait_for(timeout) == std::future_status::timeout) {
        std::cout << "Critical Error: Timeout" << std::endl;
        return 1;
    }

    std::string result_f = future_f.get();
    std::string result_g = future_g.get();

    std::string result_f_g = f_g(result_f, result_g);

    std::cout << "Result of f_g: " << result_f_g << std::endl;

    return 0;
}
