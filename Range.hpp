#pragma once


namespace ns {


template <typename T> struct Range {
    T start;
    T length;
    constexpr T stop() const noexcept {
        return start + length;
    }
};


template <typename T> constexpr Range<T> normalizeRange(Range<T> range) noexcept {
    return range.length < 0 ? Range<T>{ range.stop(), -range.length } : range;
}


}
