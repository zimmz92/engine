target_sources(mySrcFiles
    PRIVATE
        testComponentStuff.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})