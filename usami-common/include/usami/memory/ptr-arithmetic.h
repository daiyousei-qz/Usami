#pragma once
#include <cstdint>

namespace usami
{
    inline void* AdvancePtr(void* ptr, int bytes)
    {
        return reinterpret_cast<std::byte*>(ptr) + bytes;
    }
    inline const void* AdvancePtr(const void* ptr, int bytes)
    {
        return reinterpret_cast<const std::byte*>(ptr) + bytes;
    }
    inline void* AdvancePtr(void* ptr, size_t bytes)
    {
        return reinterpret_cast<std::byte*>(ptr) + bytes;
    }
    inline const void* AdvancePtr(const void* ptr, size_t bytes)
    {
        return reinterpret_cast<const std::byte*>(ptr) + bytes;
    }

    inline ptrdiff_t PtrDistance(const void* p1, const void* p2)
    {
        return reinterpret_cast<const std::byte*>(p2) - reinterpret_cast<const std::byte*>(p1);
    }

    // an alignment should always be a power of 2
    inline constexpr size_t RoundToAlign(size_t sz, size_t alignment) noexcept
    {
        size_t kAlignMask = alignment - 1;

        return (sz + kAlignMask) & (~kAlignMask);
    }
} // namespace usami