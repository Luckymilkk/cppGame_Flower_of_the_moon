#ifndef FLOWER_OF_THE_MOON_UTILS_H_
#define FLOWER_OF_THE_MOON_UTILS_H_

#include <string>
#include <vector>
#include <limits>
#include <iostream>
#include <sstream>
#include <algorithm>

std::vector<std::string> SplitString(const std::string& s, char delimiter);

std::string TrimString(const std::string& str);

int GetIntInput(const std::string& prompt, int min_val = std::numeric_limits<int>::min(), int max_val = std::numeric_limits<int>::max());

#endif  
