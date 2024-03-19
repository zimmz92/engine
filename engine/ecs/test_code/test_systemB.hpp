#pragma once
#include "ae_ecs.hpp"
#include "test_systemA.hpp"
#include "test_systemE.hpp"

class TestSystemB : public ae_ecs::AeSystem<TestSystemB> {
public:
    TestSystemB() : ae_ecs::AeSystem<TestSystemB>() {
        this->registerSystemDependency(testSystemA.getSystemId());
        this->registerSystemDependency(testSystemE.getSystemId());
        this->enableSystem();
    };
    ~TestSystemB(){};

    void setupSystem() override{};
    void executeSystem() override{};
    void cleanupSystem() override{};
};
inline TestSystemB testSystemB;