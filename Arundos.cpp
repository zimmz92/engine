#include "Arundos.hpp"

namespace ae {

    void Arundos::run() {
        while (!m_aeWindow.shouldClose()) {
            glfwPollEvents();
        }
    }
}  // namespace ae