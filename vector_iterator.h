#ifndef MY_VECTOR_VECTOR_ITERATOR_H
#define MY_VECTOR_VECTOR_ITERATOR_H

#include <iterator>

template <typename T>
struct vector_iterator {
    typedef T value_type;
    typedef T& reference;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef std::random_access_iterator_tag iterator_category;

    pointer ptr_;

    template <typename> friend struct vector;

    vector_iterator() = default;

    vector_iterator(const vector_iterator& that) = default;

    vector_iterator& operator=(const vector_iterator& that) = default;

    bool operator==(const vector_iterator& that) const {
        return ptr_ == that.ptr_;
    }

    bool operator!=(const vector_iterator& that) const {
        return ptr_ != that.ptr_;
    }

    bool operator<(const vector_iterator& that) const {
        return ptr_ < that.ptr_;
    }

    bool operator>(const vector_iterator& that) const {
        return ptr_ > that.ptr_;
    }

    bool operator<=(const vector_iterator& that) const {
        return ptr_ <= that.ptr_;
    }

    bool operator>=(const vector_iterator& that) const {
        return ptr_ >= that.ptr_;
    }

    vector_iterator& operator++() {
        ++ptr_;
        return *this;
    }

    vector_iterator& operator++(int) {
        vector_iterator ret(*this);
        ++(*this);
        return ret;
    }

    vector_iterator& operator--() {
        --ptr_;
        return *this;
    }

    vector_iterator& operator--(int) {
        vector_iterator ret(*this);
        --(*this);
        return ret;
    }

    vector_iterator&operator+=(size_t delta) {
        ptr_ += delta;
        return *this;
    }

    vector_iterator&operator-=(size_t delta) {
        ptr_ -= delta;
        return *this;
    }

    reference operator*() const {
        return *ptr_;
    }

    pointer operator->() const {
        return ptr_;
    }

    reference operator[](size_t i) {
        return ptr_[i];
    }

    friend difference_type operator-(const vector_iterator& p1, const vector_iterator& p2) {
        return p1.ptr_ - p2.ptr_;
    }

    friend vector_iterator operator+(vector_iterator p, size_t delta) {
        p += delta;
        return p;
    }

    friend vector_iterator operator-(vector_iterator p, size_t delta) {
        p -= delta;
        return p;
    }

private:
    explicit vector_iterator(pointer ptr) : ptr_(ptr) {}
};

template<typename T>
struct vector_const_iterator{
    typedef T value_type;
    typedef const T& reference;
    typedef ptrdiff_t difference_type;
    typedef const T* pointer;
    typedef std::random_access_iterator_tag iterator_category;

    pointer ptr_;

    template<typename> friend struct vector;

    vector_const_iterator() = default;

    vector_const_iterator(const vector_const_iterator& that) = default;

    vector_const_iterator(const vector_iterator<T>& that) : ptr_(that.ptr_) {};

    vector_const_iterator& operator=(const vector_const_iterator& that) = default;

    bool operator==(const vector_const_iterator& that) const {
        return ptr_ == that.ptr_;
    }

    bool operator!=(const vector_const_iterator& that) const {
        return ptr_ != that.ptr_;
    }

    bool operator<(const vector_const_iterator& that) const {
        return ptr_ < that.ptr_;
    }

    bool operator>(const vector_const_iterator& that) const {
        return ptr_ > that.ptr_;
    }

    bool operator<=(const vector_const_iterator& that) const {
        return ptr_ <= that.ptr_;
    }

    bool operator>=(const vector_const_iterator& that) const {
        return ptr_ >= that.ptr_;
    }

    vector_const_iterator& operator++() {
        ++ptr_;
        return *this;
    }

    const vector_const_iterator operator++(int) {
        vector_const_iterator ret(*this);
        ++*this;
        return ret;
    }

    vector_const_iterator& operator--() {
        --ptr_;
        return *this;
    }

    const vector_const_iterator operator--(int) {
        vector_const_iterator ret(*this);
        --*this;
        return ret;
    }

    vector_const_iterator& operator+=(size_t delta) {
        ptr_ += delta;
        return *this;
    }

    vector_const_iterator& operator-=(size_t delta) {
        ptr_ -= delta;
        return *this;
    }

    reference operator*() const {
        return *ptr_;
    }

    pointer operator->() const {
        return ptr_;
    }

    reference operator[](size_t i) {
        return ptr_[i];
    }

    friend difference_type operator-(const vector_const_iterator& p1, const vector_const_iterator& p2) {
        return p1.ptr_ - p2.ptr_;
    }

    friend vector_const_iterator operator+(vector_const_iterator p, size_t delta) {
        p += delta;
        return p;
    }

    friend vector_const_iterator operator-(vector_const_iterator p, size_t delta) {
        p -= delta;
        return p;
    }

private:
    explicit vector_const_iterator(pointer ptr) : ptr_(ptr) {}
};


#endif //MY_VECTOR_VECTOR_ITERATOR_H