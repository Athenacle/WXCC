
#ifndef BIT_CONTAINER_H
#define BIT_CONTAINER_H

#include "system.h"

#include <cstdint>
#include <type_traits>

NAMESPACE_BEGIN
namespace utils
{
    template <uint32_t BITS>
    class BitsContainer
    {
        template <uint32_t RAW_COUNT>
        struct __Container {
            uint32_t buffer_[RAW_COUNT];

        public:
        public:
            __Container()
            {
                for (uint32_t i = 0; i < RAW_COUNT; i++) {
                    buffer_[i] = 0;
                }
            }

            ~__Container() {}
        };

        __Container<BITS / 32 + 1> c;

    public:
        bool none() const
        {
            bool ret   = true;
            uint32_t i = 0;
            while (ret && i < BITS / 32 + 1) {
                ret = ret && c.buffer_[i] == 0;
                i   += 1;
            }
            return ret;
        }
        bool all()
        {
            for (size_t i = 0; i < BITS; i++) {
                if (!test(i)) {
                    return false;
                }
            }
            return true;
        }
        bool any() const
        {
            for (size_t i = 0; i < BITS; i++) {
                if (test(i)) {
                    return true;
                }
            }
            return false;
        }
        size_t count() const
        {
            size_t c = 0;
            for (size_t i = 0; i < BITS; i++) {
                if (test(i)) {
                    c++;
                }
            }
            return c;
        }
        constexpr size_t size() const
        {
            return BITS;
        }

        void set(size_t offset, bool value)
        {
            const size_t int_offset = offset / 32;
            const size_t bit_offset = offset % 32;
            const uint32_t mask     = 1 << bit_offset;
            if (value) {
                // set to 1
                *(c.buffer_ + int_offset) |= (mask);
            } else {
                *(c.buffer_ + int_offset) &= (~mask);
            }
        }

        template <size_t offset>
        void set(typename std::enable_if<offset <= BITS, bool>::type value)
        {
            set(offset, value);
        }

        bool test(size_t offset) const
        {
            const size_t int_offset = offset / 32;
            const size_t bit_offset = offset % 32;
            uint32_t v              = *(c.buffer_ + int_offset) >> bit_offset;
            return (v & 1) == 1;
        }

        template <size_t offset>
        typename std::enable_if<offset <= BITS, bool>::type test() const
        {
            return test(offset);
        }
    };

}  // namespace utils
NAMESPACE_END
#endif
