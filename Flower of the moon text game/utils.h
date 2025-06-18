#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <limits>
#include <iostream>
#include <sstream>
#include <algorithm>

// Разделяет строку s по указанному разделителю delimiter
std::vector<std::string> splitString(const std::string& s, char delimiter);

// Удаляет пробелы с начала и конца строки
std::string trimString(const std::string& str);

// Безопасный ввод целого числа с проверкой диапазона
int getIntInput(const std::string& prompt, int minVal = std::numeric_limits<int>::min(), int maxVal = std::numeric_limits<int>::max());

#endif // UTILS_H
