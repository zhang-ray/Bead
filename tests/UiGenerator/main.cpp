#include "UiGenerator.hpp"
#include <iostream>

int main(void){
    Bead::UiGenerator generator;
    auto viewRoot = generator.parseAndCreate("/tmp/app00.xml");
    std::cout << viewRoot << std::endl;

    return 0;
}

