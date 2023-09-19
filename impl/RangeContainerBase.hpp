#pragma once


#include "Range.hpp"
#include <map>
#include <utility>


namespace ns::impl {


/* Base class, shan't be used directly */
template <class Crtp, typename ValueT, typename RangeT = int> class RangeBase {
protected: // Protected member fields
    std::map<RangeT, RangeT> rangeMap_; // start -> length


protected: // Protected member functions
    /* Add range to set */
    std::pair<typename std::map<RangeT, RangeT>::iterator, ValueT*> add(Range<RangeT> range, ValueT* value) {
        /* Do nothing with zero length ranges */
        if (range.length == 0) {
            return { std::end(rangeMap_), nullptr };
        }

        /* Make sure the length is not negative */
        range = normalizeRange(range);

        /***/
        remove(range);

        /* Don't insert range with default value */
        if (isDefaultValue(value)) {
            return { std::end(rangeMap_), nullptr };
        }

        /***/
        auto valuePtr = addValue(range.start, value);

        /* Add range to map */
        auto [iterator, insertion] = rangeMap_.insert_or_assign(range.start, range.length);

        /* Check if there was an overlap left */
        if (iterator != std::cbegin(rangeMap_)) {
            iterator = tryMerge(std::prev(iterator), iterator);
        }

        /* Check if there was an overlap right */
        if (std::next(iterator) != std::cend(rangeMap_)) {
            iterator = tryMerge(iterator, std::next(iterator));
        }

        /***/
        return { iterator, valuePtr };
    }


    /* Get value at point */
    ValueT* get(RangeT point) {
        /* First rightmost range */
        auto iterator = rangeMap_.upper_bound(point);

        /* Range to the left might contain point */
        if (iterator != std::cbegin(rangeMap_)) {
            /***/
            const auto [start, length] = *std::prev(iterator);
            const auto stop = start + length;

            /* If point is inside range - return associated value */
            if (start <= point && point < stop) {
                return getValue(start);
            }
        }

        /* Point is not inside any range */
        return defaultValue();
    }


    /* Remove range */
    std::map<RangeT, RangeT>::iterator remove(Range<RangeT> range) {
        /* Do nothing with zero length ranges */
        if (range.length == 0) {
            return std::end(rangeMap_);
        }

        /* Make sure the length is not negative */
        range = normalizeRange(range);

        /* current might contain range */
        auto current = rangeMap_.upper_bound(range.start);
        if (current != rangeMap_.begin()) {
            --current;
        }

        /* last is to the right of range */
        const auto last = rangeMap_.lower_bound(range.stop());

        /***/
        while (current != last) {
            /***/
            const auto next = std::next(current);
            const auto [start, length] = *current;
            const auto stop = start + length;

            /***/ // range is x, current is o
            if (stop > range.start) { // If range is within current: x--o----o...
                /***/
                if (stop > range.stop()) { // If current sticks out to the right: x--o----x--o
                    addValue(range.stop(), getValue(start));
                    rangeMap_[range.stop()] = stop - range.stop();
                }

                /***/
                if (start >= range.start) { // range overlaps current on the left: x--o----...
                    removeValue(start);
                    rangeMap_.erase(current);
                }
                else { // current sticks out to the left: o--x----o--x
                    rangeMap_[start] = range.start - start;
                }
            }

            /***/
            current = next;
        }

        /***/
        return current;
    }


private: // Private member functions
    /* Merge ranges in case of an overlap and return iterator to rightmost range */
    std::map<RangeT, RangeT>::iterator tryMerge(const typename std::map<RangeT, RangeT>::iterator& left, const typename std::map<RangeT, RangeT>::iterator& right) {
        /***/
        const auto [leftStart, leftLength] = *left;
        const auto leftStop = leftStart + leftLength;
        const auto [rightStart, rightLength] = *right;
        const auto rightStop = rightStart + rightLength;
        const auto mergedLength = rightStop - leftStart; // Length of range [leftStart, rightStop)

        /* Do nothing if values are different */
        if (!areValuesEqual(getValue(leftStart), getValue(rightStart))) {
            return right;
        }

        /* Do nothing if no overlap */
        if (leftStop < rightStart) {
            return right;
        }

        /* Merge ranges */
        removeValue(rightStart);
        if (mergedLength > leftLength) { // If right is not inside left - extend left
            rangeMap_[leftStart] = mergedLength;
        }
        rangeMap_.erase(right);

        /***/
        return left;
    }


    /* Crtp section */
    ValueT* defaultValue() {
        return static_cast<Crtp*>(this)->defaultValue();
    }


    bool isDefaultValue(const ValueT* value) {
        return static_cast<Crtp*>(this)->isDefaultValue(value);
    }


    ValueT* addValue(RangeT start, ValueT* value) {
        return static_cast<Crtp*>(this)->addValue(start, value);
    }


    ValueT* getValue(RangeT start) {
        return static_cast<Crtp*>(this)->getValue(start);
    }


    void removeValue(RangeT start) {
        static_cast<Crtp*>(this)->removeValue(start);
    }


    bool areValuesEqual(ValueT* first, ValueT* second) {
        return static_cast<Crtp*>(this)->areValuesEqual(first, second);
    }
}; // class RangeBase


} // namespace ns::impl
