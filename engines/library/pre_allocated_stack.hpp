/// \file ##.hpp
/// The ## class is defined.
#pragma once

// dependencies

// libraries

//std
#include <cstdint>
#include <stdexcept>

namespace ae {

    template <typename T, uint64_t stackSize>
    class PreAllocatedStack{
    public:

        PreAllocatedStack(){
            for (uint64_t i = 0; i < stackSize; i++) {
                m_stackValues[m_topOfStack] = stackSize - 1 - i;
                if(m_topOfStack<stackSize-1){
                    m_topOfStack = m_topOfStack + 1;
                };
            };
        };

        ~PreAllocatedStack() = default;

        /// Push the input value onto the top of the stack.
        /// \param t_pushValue Value to be pushed to the top of the stack.
        void push(T t_pushValue){
            unsigned long overflow_test;
            if (__builtin_add_overflow(m_topOfStack, 1, &overflow_test))
            {
                throw std::runtime_error("3D object buffer Stack Overflow! Returning another object buffer positions means "
                                         "there were more objects than should have been able to exist!");
            }
            else
            {
                // No overflow detected, set the new stack top value and put the buffer index back into the stack.
                m_topOfStack = overflow_test;
                m_stackValues[m_topOfStack] = t_pushValue;
            }
        };

        /// Pop the value from the top of the stack.
        /// \return The value that was at the top of the stack.
        T pop() {
            unsigned long overflow_test;
            if (__builtin_sub_overflow(m_topOfStack, 1, &overflow_test))
            {
                throw std::runtime_error("3D object buffer Stack Overflow! No more object buffer positions to give out!");
            }
            else
            {
                // No overflow detected.
                // Get the new entity ID being allocated from the top of the stack and add the popped entity ID to the living
                // entities array then decrement the stack counter.
                T popValue = m_stackValues[m_topOfStack];
                m_topOfStack = overflow_test;;
                return popValue;
            }
        };

    private:
        /// The data stack itself as a pre-allocated array.
        T m_stackValues[stackSize];

        /// Points to what value in pre-allocated array is the current top of the stack.
        uint64_t m_topOfStack = 0;

    protected:

    };

} // namespace ae