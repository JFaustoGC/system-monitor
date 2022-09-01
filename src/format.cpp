#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int h, m, s;
    std::stringstream line;
    h = (seconds / 3600) % 100;
    m = (seconds / 60) % 60;
    s = seconds % 60;
    line << std::setfill('0') << std::setw(2) << h << ':'
         << std::setfill('0') << std::setw(2) << m << ':'
         << std::setfill('0') << std::setw(2) << s;
    return line.str();
 }