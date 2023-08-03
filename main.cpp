#include "Arundos.hpp"

// Test ECS
#ifdef DECS_DEBUG
#include "ecs_test.hpp"
#endif


// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {


#ifdef DECS_DEBUG
    test_ecs();
#endif


    ae::Arundos app{};

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}