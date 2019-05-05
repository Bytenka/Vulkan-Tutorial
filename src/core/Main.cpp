#include "Application.hpp"

#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
    try {
        Application::run();
    }
    catch (const Exception& e) {
        std::cerr << "------ Stopping on exception ------\n" << e.what() << std::endl;
        throw;
    }

    return 0;
}