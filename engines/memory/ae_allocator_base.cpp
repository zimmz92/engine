/// \file ae_allocator_base.cpp
/// AeAllocatorBase
#include "ae_allocator_base.hpp"

// dependencies

// libraries

// std
#include <stdexcept>
#include <limits>

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



    void* AeAllocatorBase::getAlignedAddressWithStoredOffset(void* t_addressToAlign, std::size_t t_byteAlignment ){

        // Error out if an alignment greater than 256 bytes is requested.
        if(t_byteAlignment>256){
            throw std::runtime_error("getAlignedAddressWithStoredOffset only works with t_byteAlignment inputs up to 256 bytes!");
        }

        // Get the pointer as a uint8_t type for doing the pointer maths.
        auto* alignedAddress = reinterpret_cast<uint8_t*>(t_addressToAlign);

        // Calculate the required offset to align the address.
        std::size_t required_offset = getAlignmentOffset(t_addressToAlign, t_byteAlignment);

        // If no offset is required then offset the returned address by the desired alignment amount to give room for
        // storing the offset.
        if(required_offset==0){

            // Increment the address by the full desired alignment.
            alignedAddress = (uint8_t*)(reinterpret_cast<std::size_t>(alignedAddress) + t_byteAlignment);

            // Store the offset, in this case the full desired alignment, in the byte before the aligned address.
            alignedAddress[-1] = static_cast<uint8_t>(t_byteAlignment & 0xFF);

            // Return the aligned address with the offset stored in the byte preceding it.
            return reinterpret_cast<void*>(alignedAddress);
        }

        // If an offset is required then increment the address accordingly.
        alignedAddress = (uint8_t*)(reinterpret_cast<std::size_t>(alignedAddress) + required_offset);

        // Store the offset in the byte before the aligned address.
        alignedAddress[-1] = static_cast<uint8_t>(required_offset & 0xFF);

        // Return the aligned address with the offset stored in the byte preceding it.
        return reinterpret_cast<void*>(alignedAddress);
    };



    void* AeAllocatorBase::getAlignedAddressWithMinimumOffset(void* t_addressToAlign,
                                                              std::size_t t_minimumOffset,
                                                              std::size_t t_byteAlignment){

        // Add the specified minimum offset to ensure that memory will be available after address alignment.
        auto* minimallyOffsetAddress = (void*)(reinterpret_cast<std::size_t>(t_addressToAlign) + t_minimumOffset);

        // Align the address which accounted for the specified minimum offset.
        return getAlignedAddressWithStoredOffset(minimallyOffsetAddress,t_byteAlignment);
    };



    void* AeAllocatorBase::getAlignedAddressWithVoidPtrOffsets(void* t_addressToAlign,
                                                               std::size_t t_numVoidPtrOffsets,
                                                               std::size_t t_byteAlignment){

        // Align the address which accounted for the specified number of void pointer offsets.
        return getAlignedAddressWithMinimumOffset(t_addressToAlign, t_numVoidPtrOffsets * sizeof(void*), t_byteAlignment);
    };



    void* AeAllocatorBase::getBaseAddressFromAlignedAddress(void* t_alignedAddress, std::size_t t_minimumOffsetUsed){
        // Get the alignment that was stored in the byte just behind the aligned address.
        std::size_t alignmentOffset = reinterpret_cast<std::uint8_t*>(t_alignedAddress)[-1];

        // The base address is the aligned address minus the alignment offset minus any required minimum offset.
        return (void*)(reinterpret_cast<std::size_t>(t_alignedAddress) - alignmentOffset - t_minimumOffsetUsed);
    };

} //namespace ae