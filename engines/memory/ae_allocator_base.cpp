/// \file ae_allocator_base.cpp
/// AeAllocatorBase
#include "ae_allocator_base.hpp"

// dependencies

// libraries

// std


namespace ae_memory {

    AeAllocatorBase::AeAllocatorBase(std::size_t t_allocatedMemorySize, void* t_allocatedMemoryPtr) noexcept :
            m_allocatedMemoryPtr{t_allocatedMemoryPtr},
            m_allocatedMemorySize{t_allocatedMemorySize}{};



    AeAllocatorBase::~AeAllocatorBase(){
        m_allocatedMemoryPtr = nullptr;
    };



    std::size_t AeAllocatorBase::getAllocatedMemorySize() const {
        return m_allocatedMemorySize;
    };



    void* AeAllocatorBase::getAllocatedMemoryPtr(){return m_allocatedMemoryPtr;};



    void* AeAllocatorBase::getAlignedAddress(void* t_addressToAlign, std::size_t t_byteAlignment){
        // Let's break this down:
        // 1) A reinterpret cast is done to t_addressToAlign in order to get the address location in a form that
        // math can be nicely done on it.
        // 2) One less than the desired alignment is added to the address. If the address is already aligned to the
        // desired number of bytes this will ensure that the current address is kept.
        // 3) The applied mask ensures to only keep the next aligned address.
        //
        // A more readable version of what this code is doing is as follows:
        //auto addressToAlign = reinterpret_cast<size_t>(t_addressToAlign);
        //auto byteOffset = t_byteAlignment - 1;
        //auto byteMask = (~(t_byteAlignment - 1));
        //return (void*)((addressToAlign + byteOffset) & byteMask);

        return (void*)((reinterpret_cast<std::size_t>(t_addressToAlign) + (t_byteAlignment - 1)) & (~(t_byteAlignment - 1)));
    }



    std::size_t AeAllocatorBase::getAlignmentOffset(void* t_addressToAlign, size_t t_byteAlignment){

        // Calculate the required memory offset to align the supplied address as desired. Mask the LSBs representing
        // the "remainder" of current address that is not aligned as desired. Then subtract that "remainder" from the
        // desired alignment to get the memory offset.
        std::size_t offset = t_byteAlignment - (reinterpret_cast<std::size_t>(t_addressToAlign) & (t_byteAlignment-1));

        if(offset == t_byteAlignment){
            // If the calculated offset is the same as the desired alignment that means that the current address was already
            // aligned and no additional offset is actually needed.
            return 0;
        } else {
            return offset;
        }
    };



} //namespace ae