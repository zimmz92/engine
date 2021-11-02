#include "first_app.hpp"
#include "Arundos.h"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main() {
    ae::FirstApp app{};

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*
// Class Definition
class ArundosApplication {
public:
	void run() {
		initVulkan();
		mainLoop();
		cleanup();
	}
}

int main()
{
	// Print the version
	//std::cout << argv[0] << " Version " << Arundos_VERSION_MAJOR << "." << Arundos_VERSION_MINOR << std::endl;
	//return 0;

	ArundosApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;

}
*/