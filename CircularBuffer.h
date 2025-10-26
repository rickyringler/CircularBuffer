#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

typedef unsigned int MemorySize;

template<typename T, MemorySize Size>
class CircularBuffer final
{
    public:

    static_assert(Size > 0, "Why would you even attempt to allocate a negative quantity of memory?");
    static constexpr unsigned char SizeLSB = Size & (Size-1);
    static_assert(SizeLSB == 0, "Size must be a power of two.");


    CircularBuffer(MemorySize SizeOfElement, unsigned int MaximumElements);
    ~CircularBuffer()=default;
    CircularBuffer(const CircularBuffer&)=delete;
    CircularBuffer& operator=(const CircularBuffer&)=delete;

    [[nodiscard]] inline const unsigned int GetCountOfElements() const {return ((Head - Tail) & Mask);};
    [[nodiscard]] inline const unsigned int GetMaximumElements() const noexcept;
    [[nodiscard]] inline const MemorySize   GetSizeOfElement()   const noexcept;
    [[gnu::hot]]         const bool         Push(const T* Data) noexcept;
    [[gnu::hot]]         const bool         Pop() noexcept;
    [[gnu::hot]]         const bool         Flush();
    [[gnu::hot]]         const T* const     GetElement(unsigned int Index) const noexcept;

    private:

    const T* Buffer[Size] = {};
    unsigned int Head = 0;
    unsigned int Tail = 0;
    unsigned int MaximumElements = 0;
    MemorySize   SizeOfElement = 0;
    static constexpr unsigned int Mask = Size - 1;

    [[nodiscard]] inline constexpr bool BufferEmpty()const noexcept{ return Head==Tail;};
    [[nodiscard]] inline constexpr bool BufferFull() const noexcept{ return ((Head+1)& Mask)==Tail;};
    inline __attribute__((always_inline)) void Reset() noexcept{ Tail=0, Head=0;};
    inline __attribute__((always_inline)) void AdvanceTail() noexcept{ Tail = (Tail+1) & Mask;};
    inline __attribute__((always_inline)) void AdvanceHead() noexcept{ Head = (Head+1) & Mask;};
};

template<typename T, MemorySize Size>
CircularBuffer<T, Size>::CircularBuffer(const MemorySize SizeOfElement, const unsigned int MaximumElements)
    : MaximumElements(MaximumElements), SizeOfElement(SizeOfElement)
{
}

template<typename T, MemorySize Size>
inline const MemorySize CircularBuffer<T, Size>::GetSizeOfElement() const noexcept
{
    return SizeOfElement;
}

template<typename T, MemorySize Size>
inline const unsigned int CircularBuffer<T, Size>::GetMaximumElements() const noexcept
{
    return MaximumElements;
}

template<typename T, MemorySize Size>
[[gnu::hot]] const bool CircularBuffer<T, Size>::Push(const T* Data) noexcept
{
    [[unlikely]]if (BufferFull()) return false;
    Buffer[Head] = Data;
    AdvanceHead();
    return true;
}

template<typename T, MemorySize Size>
[[gnu::hot]] const bool CircularBuffer<T, Size>::Pop() noexcept
{
    [[unlikely]]if (BufferEmpty()) return false;
    AdvanceTail();
    return true;
}
template<typename T, MemorySize Size>
[[gnu::hot]] const bool CircularBuffer< T, Size>::Flush()
{
    for (const T* Element : Buffer)
    {
        Element = nullptr;
    }
    Reset();
    return true;
}

template<typename T, MemorySize Size>
[[gnu::hot]] const T* const CircularBuffer<T, Size>::GetElement(const unsigned int Index) const noexcept
{
    [[unlikely]]if (BufferEmpty()) return nullptr;
    [[unlikely]]if (Index >= GetCountOfElements()) return nullptr;
    return Buffer[(Tail + Index) & Mask];
}

#endif

