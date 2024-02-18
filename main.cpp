#include "Arundos.hpp"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

// Test ECS
#ifdef DECS_DEBUG
#include "ecs_test.hpp"
#endif


// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {

    LOG(INFO) << "My first info log using default logger";

#ifdef DECS_DEBUG
    test_ecs();
#endif


    // Create an instance of this application.
    ae::Arundos app{};

    try {
        // Run the application.
        app.run();
    }
    catch (const std::exception& e) {
        // Print out the errors that did not let the application run.
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}