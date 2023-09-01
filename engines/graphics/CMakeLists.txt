target_sources(mySrcFiles
    PRIVATE
        ae_device.cpp
        ae_device.hpp
        ae_model.cpp
        ae_model.hpp
        ae_pipeline.cpp
        ae_pipeline.hpp
        ae_window.cpp
        ae_window.hpp
        ae_swap_chain.cpp
        ae_swap_chain.hpp
        ae_renderer.cpp
        ae_renderer.hpp
        ae_buffer.cpp
        ae_buffer.hpp
        ae_descriptors.cpp
        ae_descriptors.hpp
        ae_utils.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})