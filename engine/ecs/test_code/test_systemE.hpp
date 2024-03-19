#pragma once
#include "ae_ecs.hpp"
#include "test_systemA.hpp"
#include "test_systemC.hpp"
#include "test_systemD.hpp"

class TestSystemE : public ae_ecs::AeSystem<TestSystemE> {
public:
    TestSystemE() : ae_ecs::AeSystem<TestSystemE>() {
        this->registerSystemDependency(testSystemA.getSystemId());
        this->registerSystemDependency(testSystemC.getSystemId());
        this->registerSystemDependency(testSystemD.getSystemId());
        this->enableSystem();
    };
    ~TestSystemE(){};

    void setupSystem() override{};
    void executeSystem() override{};
    void cleanupSystem() override{};
};
inline TestSystemE testSystemE;