#include "Arundos.hpp"

// Test ECS
#include "ae_ecs.hpp"
#include "ecs_test.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {

        
    test_ecs();

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