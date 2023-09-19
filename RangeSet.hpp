#pragma once


#include "impl/RangeContainerBase.hpp"


namespace ns {


/* Set of ranges */
template <typename RangeT = int> class RangeSet : public impl::RangeBase<RangeSet<RangeT>, void, RangeT> {
private:
    friend impl::RangeBase;


public:
    class iterator;


public:
    void insert(Range<RangeT> range) {
        impl::RangeBase<RangeSet<RangeT>, void, RangeT>::add(range, nullptr);
    }


    void erase(Range<RangeT> range) {
        impl::RangeBase<RangeSet<RangeT>, void, RangeT>::remove(range);
    }


    iterator begin() const {
        return iterator{ std::cbegin(impl::RangeBase<RangeSet<RangeT>, void, RangeT>::rangeMap_), std::cend(impl::RangeBase<RangeSet<RangeT>, void, RangeT>::rangeMap_) };
    }
    
    
    iterator end() const {
        return iterator{ std::cend(impl::RangeBase<RangeSet<RangeT>, void, RangeT>::rangeMap_), std::cend(impl::RangeBase<RangeSet<RangeT>, void, RangeT>::rangeMap_) };
    }


private:
    void* defaultValue() {
        throw std::exception{}; // [TODO] specific exception
    }


    bool isDefaultValue(const void* value) {
        return false;
    }


    void* addValue(RangeT start, void* value) {
        return nullptr;
    }


    void* getValue(RangeT start) {
        return nullptr;
    }


    void removeValue(RangeT start) {
    }


    bool areValuesEqual(void* first, void* second) {
        return true;
    }
}; // class RangeSet


template <typename RangeT> class RangeSet<RangeT>::iterator {
private:
    std::map<RangeT, RangeT>::const_iterator iterator_;
    const typename std::map<RangeT, RangeT>::const_iterator end_;
    Range<RangeT> value_;


public:
    iterator(std::map<RangeT, RangeT>::const_iterator&& iterator, std::map<RangeT, RangeT>::const_iterator&& end) : iterator_{ std::move(iterator) }, end_{ std::move(end) } {
        if (iterator_ != end_) {
            value_ = { iterator_->first, iterator_->second };
        }
    }


    iterator& operator++() {
        ++iterator_;
        if (iterator_ != end_) {
            value_ = { iterator_->first, iterator_->second };
        }
        return *this;
    }


    const Range<const RangeT>& operator*() const {
        return reinterpret_cast<const Range<const RangeT>&>(value_);
    }


    const Range<const RangeT>* operator->() const {
        return &value_;
    }


    bool operator!=(const iterator& other) {
        return iterator_ != other.iterator_;
    }
}; // class iterator


} // namespace ns
