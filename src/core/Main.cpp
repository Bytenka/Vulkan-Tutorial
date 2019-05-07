#include "Application.hpp"
#include "pch.hpp"

#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
    try {
        Application::run(argc, const_cast<const char**>(argv));

    } catch (const Exception& e) {
        std::cerr << "------ Stopping on exception ------\n"
                  << e.what() << std::endl;
        throw;
    }

    return 0;
}