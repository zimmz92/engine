#pragma once
#include "ae_ecs.hpp"

class TestSystemA : public ae_ecs::AeSystem<TestSystemA> {
public:
    TestSystemA() : ae_ecs::AeSystem<TestSystemA>() {
        this->enableSystem();
    };
    ~TestSystemA(){};

    void setupSystem() override{};
    void executeSystem() override{};
    void cleanupSystem() override{};
};
inline TestSystemA testSystemA;