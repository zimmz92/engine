#include "Arundos.hpp"
#include "ae_ecs.hpp"

// Test ECS
#ifdef MY_DEBUG
#include "ecs_test.hpp"
#endif


// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {


#ifdef MY_DEBUG
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