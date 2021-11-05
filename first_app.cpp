#include "first_app.hpp"

namespace lve {

    void FirstApp::run() {
        while (!aeWindow.shouldClose()) {
            glfwPollEvents();
        }
    }
}  // namespace lve