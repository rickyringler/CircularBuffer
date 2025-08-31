#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <array>
#include "../../utilities/PCH.h"
#include "../states/BufferState.h"


template<typename T, std::size_t Size>
class CircularBuffer final
{
    public:

    static_assert(Size > 0, "Why would you even attempt to allocate a negative quantity of memory?");

    CircularBuffer(std::size_t ElementSize, std::size_t MaximumElements);
    ~CircularBuffer();

    [[nodiscard]] inline const std::size_t* GetSizeElements()    const {return this->Elements;};
    [[nodiscard]] inline const std::size_t* GetElementSize()     const noexcept;
    [[nodiscard]] inline const std::size_t* GetMaximumElements() const noexcept;
    bool                  Push(const T& Data) noexcept;
    bool                  Pop() noexcept;

    private:

    static constexpr std::size_t Mask = Size - 1;
    std::array<T, Size> Buffer{};
    std::size_t  Head;
    std::size_t  Tail;
    std::size_t  Elements;
    BufferState* State    = nullptr;

    [[nodiscard]] inline constexpr bool BufferEmpty()        const noexcept{ return Head==Tail;};
    [[nodiscard]] inline constexpr bool BufferFull()         const noexcept{ return Buffer.size() == Size;};
    bool                  Initialize(std::size_t ElementSize, std::size_t MaximumElements) noexcept;
};

template<typename T, std::size_t Size>
CircularBuffer<T, Size>::CircularBuffer(const std::size_t ElementSize, const std::size_t MaximumElements)
{
    Initialize(ElementSize, MaximumElements);
}

template<typename T, std::size_t Size>
CircularBuffer<T, Size>::~CircularBuffer()
{
    delete this->State;
}

template<typename T, std::size_t Size>
bool CircularBuffer<T, Size>::Initialize(std::size_t ElementSize, std::size_t MaximumElements) noexcept
{
    this->State = new BufferState;
    this->State->ElementSize = &ElementSize;
    this->State->MaximumElements = &MaximumElements;
    return true;
}

template<typename T, std::size_t Size>
inline const std::size_t* CircularBuffer<T, Size>::GetElementSize() const noexcept
{
    return this->State->GetElementSize();
}

template<typename T, std::size_t Size>
inline const std::size_t* CircularBuffer<T, Size>::GetMaximumElements() const noexcept
{
    return this->State->GetMaximumElements();
}

template<typename T, std::size_t Size>
bool CircularBuffer<T, Size>::Push(const T& Data) noexcept
{
    [[unlikely]]if (BufferFull()) return false;

    Buffer[Head] = Data;
    Head = (Head + 1) & Mask;

    ++Elements;

    return true;
}

template<typename T, std::size_t Size>
bool CircularBuffer<T, Size>::Pop() noexcept
{
    [[unlikely]]if (BufferEmpty()) return false;

    Tail = (Tail + 1) & Mask;

    --Elements;

    return true;
}

