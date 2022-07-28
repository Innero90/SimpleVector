/* Необходимо реализовать move-конструктор и оператор присваивания из временного объекта (ArrayPtr<Type>&&)*/
/* Я реализовал это в simple_vector ниже, или сделал что то не верно и не так надо было? 
SimpleVector& operator=(SimpleVector&& rvalue)  и SimpleVector(SimpleVector&& other)*/



#pragma once

#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <initializer_list>
#include <cstddef>
#include <iostream>
#include <iterator>

#include "array_ptr.h"

using namespace std;

class ReserveProxyObj {
    public:
    
    ReserveProxyObj(size_t capacity) {
        capacity_to_reserve = capacity;
    }
    
    size_t capacity_to_reserve;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) {
        capacity_ = size;
        size_ = size;
        ArrayPtr<Type> tmp(size);
        data.swap(tmp);
        fill(begin(), end(), 0);
    }
    
    SimpleVector(ReserveProxyObj obj) {
        capacity_ = obj.capacity_to_reserve;
        ArrayPtr<Type> tmp(capacity_);
        data.swap(tmp);
        size_ = 0;
        
    }

    SimpleVector(size_t size, const Type& value) {
        capacity_ = size;
        size_ = size;
        ArrayPtr<Type> tmp(size);
        data.swap(tmp);
        fill(begin(), end(), value);
    }

    SimpleVector(initializer_list<Type> init) {
        capacity_ = init.size();
        size_ = init.size();
        ArrayPtr<Type> tmp(size_);
        data.swap(tmp);
        auto it = begin();
        for (auto element : init) {
            *it = element;
            ++it;
        }
        
    }
    
    SimpleVector(const SimpleVector& other) {
        size_ = other.GetSize();
        capacity_ = other.GetCapacity();
        ArrayPtr<Type> tmp(size_);
        data.swap(tmp);
        copy(other.begin(), other.end(), tmp.begin());
    }
    
    SimpleVector(SimpleVector&& other) {
        Resize(other.GetSize());
        size_ = other.GetSize();
        capacity_ = other.GetCapacity();
        move(other.begin(), other.end(), this->begin());
        exchange(other.capacity_, 0);
        exchange(other.size_, 0);
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }
    
    void Reserve(size_t new_capacity) {
        if (capacity_ < new_capacity) {
            capacity_ = new_capacity;
            ArrayPtr<Type> tmp(capacity_);
            for (size_t i = 0; i < size_; ++i) {
                tmp[i] = data[i];
            }
            data.swap(tmp);
        }
    }

    bool IsEmpty() const noexcept {
        if (size_ == 0) {
            return true;
        }
        return false;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return data[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return data[index];
    }

    Type& At(size_t index) {
        if (index >= size_) {
            throw out_of_range("Index out of range");
            }
        return data[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw out_of_range("Index out of range");
            }
        return data[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
        }
        if (new_size <= capacity_) {
            for (size_t i = size_; i < new_size; ++i) {
                data[i] = Type();
            }
            size_ = new_size;
        }
        else {
            size_t new_capacity_ = new_size * 2;
            ArrayPtr<Type> tmp(new_capacity_);
            int a = 0;
            for (size_t i = 0; i < size_; ++i) {
                tmp[a] = move(data[a]);
                ++a;
            }
            capacity_ = new_capacity_;
            size_ = new_size;
            data.swap(tmp);
        }
    }

    Iterator begin() noexcept {
        return data.Get();
    }

    Iterator end() noexcept {
        return data.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return data.Get();
    }

    ConstIterator end() const noexcept {
        return data.Get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return data.Get();
    }

    ConstIterator cend() const noexcept {
        return data.Get() + size_;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        size_ = rhs.GetSize();
        capacity_ = rhs.GetCapacity();
        ArrayPtr<Type> tmp(size_);
        data.swap(tmp);
        copy(rhs.begin(), rhs.end(), data.begin());
        return *this;
    }
    
    SimpleVector& operator=(SimpleVector&& rvalue) {
        size_ = move(rvalue.size_);
        capacity_ = move(rvalue.capacity_);
        ArrayPtr<Type> tmp(size_);
        data.swap(tmp);
        size_t i = 0;
        while (i != size_) {
            *(begin() + i) = move(*(rvalue.begin() + i));
            ++i;
        }
        rvalue.capacity_ = 0;
        rvalue.size_ = 0;
        return *this;
    }

    void PushBack(const Type& item) {
        if (size_ < capacity_) {
            *(end()) = item;
            ++size_;
        } else {
            size_t old_size = size_;
            Resize(size_ + 1);
            size_ = old_size + 1;
            data[old_size] = item;
        }
    }
    
    void PushBack(Type&& item) {
        if (size_ < capacity_) {
            *(end()) = move(item);
            ++size_;
        } else {
            size_t old_size = size_;
            Resize(size_ + 1);
            size_ = old_size + 1;
            data[old_size] = move(item);
        }
    }
    
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        if (capacity_ == 0) {
            capacity_ = 1;
            size_ = 1;
            ArrayPtr<Type> tmp(size_);
            data.swap(tmp);
            *begin() = value;
            return begin();
        } else if (size_ < capacity_) {
            auto it = begin();
            while (it != pos) {
                ++it;
            }
            copy_backward(it, end(), end() + 1);
            *it = value;
            ++size_;
            return it;
        } else {
            size_t count = pos - begin();
            size_t old_size = size_;
            Resize(size_ + 1);
            size_ = old_size;
            auto it = begin() + count;
            copy_backward(it, end(), end() + 1);
            *it = value;
            ++size_;
            return it;
        }
    }
    
    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());
        if (capacity_ == 0) {
            capacity_ = 1;
            size_ = 1;
            ArrayPtr<Type> tmp(size_);
            data.swap(tmp);
            *(begin()) = move(value);
            return begin();
        } else if (size_ < capacity_) {
            auto it = begin();
            while (it != pos) {
                ++it;
            }
            move_backward(it, end(), end() + 1);
            *it = move(value);
            ++size_;
            return it;
        } else {
            size_t count = pos - begin();
            size_t old_size = size_;
            Resize(size_ + 1);
            size_ = old_size;
            auto it = begin() + count;
            move_backward(it, end(), end() + 1);
            *it = move(value);
            ++size_;
            return it;
        }
    }

    void PopBack() noexcept {
        if(size_ > 0) {
            --size_;
        }
    }
    
    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos < end());
        size_t count = pos - begin();
        for (size_t i = count; i < size_; ++i) {
            data[i] = data[i + 1];
        }
        --size_;
        return begin() + count;
    }
    
    Iterator Erase(Iterator pos) {
        assert(pos >= begin() && pos < end());
        size_t count = pos - begin();
        for (size_t i = count; i < size_; ++i) {
            data[i] = move(data[i + 1]);
        }
        --size_;
        return begin() + count;
    }

    void swap(SimpleVector& other) noexcept {
        data.swap(other.data);
        size_t tmp = size_;
        size_ = other.size_;
        other.size_ = tmp;
        tmp = capacity_;
        capacity_ = other.capacity_;
        other.capacity_ = tmp;
    }
    
    void Print() {
        for (size_t i = 0; i < GetSize(); ++i) {
            cout << data[i] << " ";
        }
        cout << endl;
    }
    
private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    ArrayPtr<Type> data;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs > lhs);
} 

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    ReserveProxyObj obj(capacity_to_reserve);
    return obj;
}
