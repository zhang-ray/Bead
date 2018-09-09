#include "UiGenerator.hpp"
#include <iostream>

int main(void){
    Bead::UiGenerator generator;
    auto viewRoot = generator.parseAndCreate("/tmp/appUI.xml");
    std::cout << viewRoot << std::endl;

    return 0;
}

