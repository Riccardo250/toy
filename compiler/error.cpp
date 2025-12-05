#include "error.h"
#include <iostream>

void Error::error(const std::string& errorMsg) {
    std::cerr << errorMsg << "\n";
    std::exit(-1);
}