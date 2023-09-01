#pragma once
#include "ae_ecs.hpp"
#include "test_systemA.hpp"

class TestSystemD : public ae_ecs::AeSystem<TestSystemD> {
public:
    TestSystemD() : ae_ecs::AeSystem<TestSystemD>() {
        this->registerSystemDependency(testSystemA.getSystemId());
        this->enableSystem();
    };
    ~TestSystemD(){};

    void setupSystem() override{};
    void executeSystem() override{};
    void cleanupSystem() override{};
};
inline TestSystemD testSystemD;