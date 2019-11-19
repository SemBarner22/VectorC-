//
// Created by sem on 16.07.19.
//

#include <memory>
#include <variant>
#include <assert.h>
#include "vector_iterator.h"

#ifndef MY_VECTOR_VECTOR_H
#define MY_VECTOR_VECTOR_H

template <typename T>
class vector {

    struct helper {
        size_t size_ = 0;
        size_t capacity_ = 0;
        size_t amount_ = 0;

        T *next_elem() {
            return reinterpret_cast<T *>(&amount_ + sizeof(T));
        }
    };

    void *alloc_one(size_t n) {
        return operator new(n);
    }

    helper* alloc_many(size_t n) {
        auto new_helper = static_cast<helper *>(alloc_one(sizeof(T) * n + sizeof(helper)));
        new_helper->amount_ = 0;
        new_helper->size_ = 0;
        new_helper->capacity_ = n;
        return new_helper;
    }

    template<typename ...Args>
    void construct(T *p, Args const &...args) {
        assert(p != nullptr);
        new(p) T(args...);
    }

    void construct(T *p, T *copy) {
        assert(p != nullptr && copy != nullptr);
        new(p) T(*copy);
    }

    void destruct(T *p) noexcept {
        p->~T();
    }

    void deallocate(helper *d) {
        operator delete(static_cast<void *>(d));
    }

    void destruct_if_failed(size_t n, T *p) {
        for (size_t i = 0; i < n; ++i) {
            destruct(p + i);
        }
    }

    helper *copy(size_t n) {
        helper *ans = alloc_many(n);
        size_t i = 0;
        try {
            for (; i < std::min(get_size(), n); ++i) {
                construct(ans->next_elem() + i, get_ptr() + i);
            }
        } catch (...) {
            destruct_if_failed(i, ans->next_elem());
            deallocate(ans);
            throw;
        }
        ans->capacity_ = n;
        ans->size_ = get_size();
        ans->amount_ = 0;
        return ans;
    }

    typedef std::variant<helper *, T> var;

    var data_;

    helper *get_variant() const noexcept {
        assert(data_.index() == 0);
        return std::get<0>(data_);
    }

    size_t get_capacity() const noexcept {
        assert(get_variant() != nullptr);
        return get_variant()->capacity_;
    }

    size_t get_size() const noexcept {
        assert(get_variant() != nullptr);
        return get_variant()->size_;
    }

    size_t get_amount() const noexcept {
        assert(get_variant() != nullptr);
        return get_variant()->amount_;
    }

    T *get_ptr() const noexcept {
        assert(get_variant() != nullptr);
        return get_variant()->next_elem();
    }

    bool small() const noexcept {
        return (data_.index() == 1 || get_variant() == nullptr);
    }

    void detach() {
        if (data_.index() == 0 && get_variant() != nullptr && get_variant()->amount_ > 0) {
            helper *new_helper = copy(get_capacity());
            --get_variant()->amount_;
            data_ = new_helper;
        }
    }

public:
    static_assert((sizeof(data_)) <= (sizeof(void *) + std::max(sizeof(T), sizeof(void *))));

    vector() noexcept = default;

    ~vector() noexcept {
        clear();
    }

    size_t size() const noexcept {
        if (data_.index() == 0) {
            return get_variant() == nullptr ? 0 : get_size();
        }
        return 1;
    }

    bool empty() const noexcept {
        return size() == 0;
    }

    void clear() {
        if (!small()) {
            if (get_amount() == 0) {
                assert(data_.index() == 0);
                destruct_if_failed(get_size(), get_ptr());
                deallocate(std::get<0>(data_));
                std::get<0>(data_) = nullptr;
            } else {
                --get_variant()->amount_;
                data_ = nullptr;
            }
        } else {
            data_ = nullptr;
        }
    }

    void push_back(T const &value) {
        detach();
        if (empty() && get_variant() == nullptr) {
            try {
                data_ = value;
            } catch (...) {
                data_ = nullptr;
                throw;
            }
        } else {
            if (small()) {
                helper *new_helper = alloc_many(4);
                new_helper->size_ = 2;
                try {
                    construct(new_helper->next_elem(), std::get<1>(data_));
                } catch (...) {
                    deallocate(new_helper);
                    throw;
                }
                try {
                    construct(new_helper->next_elem() + 1, value);
                } catch (...) {
                    destruct(new_helper->next_elem());
                    deallocate(new_helper);
                    throw;
                }
                data_ = new_helper;
            } else {
                if (get_size() >= get_capacity()) {
                    helper *new_helper = copy(get_capacity() * 2);
                    try {
                        construct(new_helper->next_elem() + get_size(), value);
                    } catch (...) {
                        destruct_if_failed(get_size(), new_helper->next_elem());
                        deallocate(new_helper);
                        throw;
                    }
                    assert(data_.index() == 0);
                    destruct_if_failed(get_size(), get_ptr());
                    deallocate(std::get<0>(data_));
                    std::get<0>(data_) = nullptr;
                    data_ = new_helper;
                } else {
                    construct(get_ptr() + get_size(), value);
                }
                ++get_variant()->size_;
            }
        }
    }

    void pop_back() {
        assert(size() > 0);
        if (small()) {
            data_ = nullptr;
        } else {
            destruct(get_ptr() + get_size() - 1);
            --get_variant()->size_;
        }
    }

    T &operator[](size_t i) {
        detach();
        if (small()) {
            assert(i == 0);
            return std::get<1>(data_);
        } else {
            assert(data_.index() == 0);
            assert(get_variant() != nullptr);
            assert(i < get_size());
            return *(get_ptr() + i);
        }
    }

    T const &operator[](size_t i) const {
        if (small()) {
            assert(i == 0);
            return std::get<1>(data_);
        } else {
            assert(data_.index() == 0);
            assert(get_variant() != nullptr);
            assert(i < get_size());
            return *(get_ptr() + i);
        }
    }


    vector(const vector &a) : vector() {
        if (&a != this) {
            if (a.small()) {
                if (!a.empty()) {
                    push_back(a[0]);
                }
            } else {
                data_ = a.get_variant();
                ++get_variant()->amount_;
            }
        }
    }

    vector &operator=(const vector &a) {
        if (a != *this) {
            clear();
            data_ = a.data_;
            if (!a.small()) {
                ++get_variant()->amount_;
            }
        }
        return *this;
    }

    T &front() {
        assert(size() > 0);
        return (*this)[0];
    }

    const T &front() const {
        assert(size() > 0);
        return (*this)[0];
    }

    T &back() {
        assert(size() > 0);
        return (*this)[get_variant()->size_ - 1];
    }

    const T &back() const {
        assert(size() > 0);
        return (*this)[get_variant()->size_ - 1];
    }


    void reserve(size_t n) {
        if (empty()) {
            helper *new_helper = alloc_many(n);
            data_ = new_helper;
        } else if (small()) {
            if (n > 0) {
                helper *new_helper = alloc_many(n + 4);
                new_helper->size_ = 1;
                try {
                    construct(new_helper->next_elem(), std::get<1>(data_));
                } catch (...) {
                    deallocate(new_helper);
                    throw;
                }
                data_ = new_helper;
            }
        } else {
            helper *new_helper = copy(get_capacity() + n);
            destruct_if_failed(get_size(), get_ptr());
            deallocate(std::get<0>(data_));
            std::get<0>(data_) = nullptr;
            data_ = new_helper;
        }
    }

    typedef vector_iterator <T> iterator;
    typedef vector_const_iterator <T> const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    iterator end() {
        if (empty()) {
            return iterator(nullptr);
        }
        detach();
        return small() ? iterator(&(std::get<1>(data_))) : iterator(get_ptr() + size());
    }

    iterator begin() {
        if (empty()) {
            return iterator(nullptr);
        }
        detach();
        return small() ? iterator(&(std::get<1>(data_))) : iterator(get_ptr());
    }

    const_iterator end() const noexcept {
        return small() ? const_iterator(&(std::get<1>(data_))) : const_iterator(get_ptr() + size());
    }

    const_iterator begin() const noexcept {
        return small() ? const_iterator(&(std::get<1>(data_))) : const_iterator(get_ptr());
    }

    const_iterator cend() const noexcept {
        return end();
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    reverse_iterator rbegin() {
        detach();
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        detach();
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    void swap(vector &that) {
        if (this == &that) {
            return;
        }
        bool a1 = (data_.index() == 0);
        bool a2 = (that.data_.index() == 0);
        if (a1 && a2) {
            std::swap(data_, that.data_);
        } else if (!a1 && a2) {
            that.swap(*this);
        } else if (a1 && !a2) {
            helper *new_helper = get_variant();
            try {
                data_ = that.data_;
            } catch (...) {
                that.data_ = nullptr;
                data_ = new_helper;
                throw;
            }
            that.data_ = new_helper;
        } else {
            try {
                std::swap(data_, that.data_);
            } catch (...) {
                throw;
            }
        }
    }

    bool operator<(const vector &that) const noexcept {
        for (size_t i = 0; i < std::min(size(), that.size()); i++) {
            if (that[i] < (*this)[i]) {
                return false;
            }
            if (that[i] > (*this)[i]) {
                return true;
            }
        }
        return (this->size() < that.size());
    }

    bool operator>(const vector &that) const noexcept {
        return that < (*this);
    }

    bool operator==(const vector &that) const noexcept {
        return (((*this) <= that) && ((*this) >= that));
    }

    bool operator!=(const vector &that) const noexcept {
        return !((*this) == that);
    }

    bool operator<=(const vector &that) const noexcept {
        return !((*this) > that);
    }

    bool operator>=(const vector &that) const noexcept {
        return !((*this) < that);
    }

    T *data() noexcept {
        if (empty()) {
            return nullptr;
        }
        return small() ? &std::get<1>(data_) : get_ptr();
    }

    const T *data() const noexcept {
        if (empty()) {
            return nullptr;
        }
        return small() ? &std::get<1>(data_) : get_ptr();
    }

    void erase(const_iterator pos) {
        erase(pos, pos + 1);
    }

    void erase(const_iterator L, const_iterator R) {
        detach();
        if (R == end()) {
            for (; L != R; --R) {
                pop_back();
            }
        } else {
            size_t fir = L - cbegin();
            size_t sec = R - cbegin();
            size_t sz = size();
            vector new_v;
            new_v.reserve(sz - (R - L) + 4);
            size_t i = 0;
            try {
                for (; i < fir; ++i) {
                    new_v.push_back((*this)[i]);
                }
            } catch (...) {
                new_v.clear();
                throw;
            }
            i = sec;
            try {
                for (; i < sz; i++) {
                    new_v.push_back((*this)[i]);
                }
            } catch (...) {
                new_v.clear();
                throw;
            }
            *this = new_v;
        }
    }


    template<typename InputIterator>
    void assign(InputIterator first, InputIterator second) {
        clear();
        for (; first != second; ++first) {
            push_back(*first);
        }
    }

    void insert(const_iterator pos, T const val) {
        detach();
        if (empty() || pos == end()) {
            push_back(val);
        } else {
            size_t sz = size();
            vector new_v;
            new_v.reserve(sz + 4);
            size_t i = 0;
            try {
                for (; i < pos - cbegin(); ++i) {
                    new_v.push_back((*this)[i]);
                }
                new_v.push_back(val);
            } catch (...) {
                new_v.clear();
                throw;
            }
            try {
                for (; i < sz; ++i) {
                    new_v.push_back((*this)[i]);
                }
            } catch (...) {
                new_v.clear();
                throw;
            }
            *this = new_v;
        }
    }
};

    template<class T>
    void swap(vector<T>& first, vector<T> & that) {
        first.swap(that);
    }

#endif //MY_VECTOR_VECTOR_H
































