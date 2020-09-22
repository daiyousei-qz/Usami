#pragma once
#include "usami/common.h"

namespace usami
{
    template <typename T>
    class MemoryBuffer
    {
    private:
        size_t size_               = 0;
        std::unique_ptr<T[]> data_ = nullptr;

    public:
        MemoryBuffer() = default;
        MemoryBuffer(size_t size)
        {
            Initialize(size);
        }

        void Initialize(size_t size)
        {
            this->size_ = size;
            this->data_ = make_unique<T[]>(size);
        }

        bool IsInitialized() const noexcept
        {
            return data_ != nullptr;
        }

        size_t Size() const noexcept
        {
            return size_;
        }

        T* Data() noexcept
        {
            return data_.get();
        }
        const T* Data() const noexcept
        {
            return data_.get();
        }

        void Clear(const T& value = {})
        {
            std::fill_n(data_.get(), size_, value);
        }

        T& At(size_t i) noexcept
        {
            USAMI_ASSERT(i < size_);
            return data_[i];
        }
        const T& At(size_t i) const noexcept
        {
            USAMI_ASSERT(i < size_);
            return data_[i];
        }
    };
} // namespace usami
