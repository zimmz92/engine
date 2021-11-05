#pragma once

#include "ae_pipeline.hpp"
#include "ae_window.hpp"

namespace ae {
    class FirstApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run();

    private:
        AeWindow aeWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
        AeDevice aeDevice{ aeWindow };
        VkPipelineLayout pipelineLayout;
        AePipeline aePipeline{
            aeDevice,
            "engines/graphics/shaders/vert.spv",
            "engines/graphics/shaders/frag.spv",
            AePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT) };
    };
}  // namespace ae