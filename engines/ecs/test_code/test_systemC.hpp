#pragma once
#include "ae_ecs.hpp"
#include "test_systemA.hpp"
#include "test_systemD.hpp"

class TestSystemC : public ae_ecs::AeSystem<TestSystemC> {
public:
    TestSystemC() : ae_ecs::AeSystem<TestSystemC>() {
        this->registerSystemDependency(testSystemA.getSystemId());
        this->registerSystemDependency(testSystemD.getSystemId());
        this->enableSystem();
    };
    ~TestSystemC(){};

    void setupSystem() override{};
    void executeSystem() override{};
    void cleanupSystem() override{};
};
inline TestSystemC testSystemC;