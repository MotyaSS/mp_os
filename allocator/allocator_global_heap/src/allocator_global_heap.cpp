#include <not_implemented.h>
#include <exception>

#include "../include/allocator_global_heap.h"


allocator_global_heap::allocator_global_heap(
        logger* logger) : _logger(logger)
{
    trace_with_guard("allocator_global_heap::allocator_global_heap(logger*) call/end");
}

allocator_global_heap::~allocator_global_heap()
{
    trace_with_guard("allocator_global_heap::~allocator_global_heap() call/end");
}

allocator_global_heap::allocator_global_heap(
        allocator_global_heap &&other) noexcept
{
    _logger = other._logger;
    other._logger = nullptr;

}

allocator_global_heap &allocator_global_heap::operator=(
        allocator_global_heap &&other) noexcept
{
    _logger = other._logger;
    other._logger = nullptr;
    return *this;
}

[[nodiscard]] void* allocator_global_heap::allocate(
        size_t value_size,
        size_t values_count)
{
    debug_with_guard("void *allocator_global_heap::allocate call");
    void* allocated_memory;
    try
    {
        allocated_memory = ::operator new[](value_size * values_count + sizeof(size_t) + sizeof(allocator*));
        auto** allocator = reinterpret_cast<allocator_global_heap**>(allocated_memory);
        *allocator = this;
        auto* size = reinterpret_cast<size_t*>(allocator + 1);
        *size = values_count * value_size;
        allocated_memory = reinterpret_cast<void*>(size + 1);
    }
    catch (std::bad_alloc &err)
    {
        error_with_guard("allocator_global_hep::allocate: failed to allocate");
        throw err;
    }
    debug_with_guard("void *allocator_global_heap::allocate end call");
    return allocated_memory;

}

allocator const* allocator_global_heap::get_allocator_ptr(void const* memory)
{
    trace_with_guard("allocator_global_heap::get_allocator_ptr(void const*) call");
    return *(reinterpret_cast<allocator_global_heap* const*>(reinterpret_cast<size_t const*>(memory) - 1) - 1);
    trace_with_guard("allocator_global_heap::get_allocator_ptr(void const*) end call");
}

size_t allocator_global_heap::get_allocated_size(void const* memory)
{
    trace_with_guard("allocator_global_heap::~allocator_global_heap call");
    auto const* size = reinterpret_cast<size_t const*> (reinterpret_cast<unsigned char const*> (memory) -
                                                        sizeof(size_t));
    trace_with_guard("allocator_global_heap::~allocator_global_heap end call");
    return *size;
}

std::string allocator_global_heap::get_byte_dump(unsigned char c)
{
    trace_with_guard("allocator_global_heap::get_byte_dump(unsigned char) call");
    std::string dump;
    for (int i = 7; i >= 0; --i)
    {
        char bit = ((c >> i) & 1) ? '1' : '0';
        dump.push_back(bit);
    }
    trace_with_guard("allocator_global_heap::get_byte_dump(unsigned char) end call");
    return dump;
}


void allocator_global_heap::deallocate(
        void* at)
{
    debug_with_guard("allocator_global_heap::deallocate(void *at) call");
    size_t size = get_allocated_size(at);
    std::string dump;
    auto* temp_pointer = reinterpret_cast<unsigned char*> (at);
    for (auto* i = temp_pointer; i < temp_pointer + size; ++i)
    {
        dump += get_byte_dump(*i);
        dump += ' ';
    }

    if (this != get_allocator_ptr(at))
    {
        throw std::logic_error("trying to deallocate memory that doesn't belong to allocator");
    }

    debug_with_guard(dump);
    ::operator delete(temp_pointer - sizeof(size_t) - sizeof(allocator_global_heap**));
    debug_with_guard("allocator_global_heap::deallocate(void *at) end");
}

inline std::string allocator_global_heap::get_typename() const noexcept
{
    error_with_guard("allocator_global_heap::get_typename: why r u using this?");
    throw not_implemented("allocator_global_heap::get_typename", "why r u using this?");
}

inline logger* allocator_global_heap::get_logger() const
{
    return _logger;
}