#include "Arundos.hpp"

namespace ae {

    void Arundos::run() {
        while (!aeWindow.shouldClose()) {
            glfwPollEvents();
        }
    }
}  // namespace ae