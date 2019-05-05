#include <iostream>

#include "Application.hpp"

int main(int argc, char* argv[])
{
    Application::init();

    std::cout << "Hello World!" << std::endl;
    return 0;
}