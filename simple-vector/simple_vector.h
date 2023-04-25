#pragma once
#include <cassert>
#include <algorithm>
#include <initializer_list>
#include <array>
#include <stdexcept>
#include <utility>
#include <iostream>


#include "array_ptr.h"

struct ReserveProxyObj {
    explicit ReserveProxyObj(int size_) : size(size_) {}
    int size = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    /*
    using Iterator = ArrayPtr<Type>;
    using ConstIterator = ArrayPtr<const Type>;
    */
    using Iterator = Type*;
    using ConstIterator = const Type*;


    SimpleVector() noexcept = default;

    SimpleVector(ReserveProxyObj inp) {
        Reserve(inp.size);
    }

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        Resize(size);
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
        Reserve(size);
        std::fill(begin(), begin() + size, value);
        size_ = size;
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        Reserve(init.size());
        std::copy(init.begin(), init.end(), begin());
        size_ = init.size();
    }

    // Конструктор копирования
    SimpleVector(const SimpleVector& init) {
        Reserve(init.GetSize());
        std::copy(init.begin(), init.end(), begin());
        size_ = init.GetSize();
    }

    // Конструктор перемещения
    SimpleVector(SimpleVector&& init) {
        /*
        first = std::move(init.first);
        size_ = init.size_;
        capacity_ = init.capacity_;
        init.size_ = 0;
        init.capacity_ = 0;
        */
        swap(init);
    }

    SimpleVector& operator=(const SimpleVector& rval) {
        if (this == &rval) {
            return *this;
        }
        SimpleVector copy(rval);
        swap(copy);
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rval) {
        if (this == &rval) {
            return *this;
        }
        SimpleVector copy(std::move(rval));
        swap(copy);
        return *this;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {

            ArrayPtr<Type> copy(new_capacity + 1);
            Iterator iter(copy.Get());
            for (size_t i = 0; i < size_; ++i) {
                *iter = std::move(first[i]); //копирование элементов текущего массива
                ++iter;
            }
            first = std::move(copy);
            capacity_ = new_capacity;
        }
    };

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    // Если объекты перемещаемые, стоит выбрать передачу аргумента по значению или универсальной ссылке.Если неперемещаемые — по константной ссылке.
    void PushBack(const Type& item) {
        if (size_ + 1 > capacity_) {
            if (size_ != 0) {
                Reserve(size_ * 2);
            }
            if (size_ == 0) {
                Reserve(1);
            }
        }
        *end() = item;
        ++size_;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type&& item) {
        if (size_ + 1 > capacity_) {
            if (size_ != 0) {
                Reserve(size_ * 2);
            }
            if (size_ == 0) {
                Reserve(1);
            }
        }
        *(end()) = std::move(item);
        ++size_;

    }
    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, Type&& value) {
        //Проверка на корректность итератора
        assert(pos >= begin() && pos <= end());

        size_t position = pos - begin();
        if (size_ + 1 > capacity_) {
            if (size_ != 0) {
                Reserve(size_ * 2);
            }
            if (size_ == 0) {
                Reserve(1);
            }
        }

        std::move_backward(begin() + position, end(), end() + 1);
        first[position] = std::move(value);
        ++size_;
        return begin() + position;
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        //Проверка на корректность итератора
        assert(pos >= begin() && pos <= end());

        size_t position = pos - begin();
        if (size_ + 1 > capacity_) {
            if (size_ != 0) {
                Reserve(size_ * 2);
            }
            if (size_ == 0) {
                Reserve(1);
            }
        }

        std::move_backward(begin() + position, end(), end() + 1);
        first[position] = value;
        ++size_;
        return begin() + position;
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(size_ > 0);
        Resize(size_ - 1);
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        //Проверка на корректность итератора
        assert(pos >= begin() && pos <= end());

        if (IsEmpty()) {
            throw std::out_of_range("vector_is_empty");
        }
        else if (size_ == 1) {
            --size_;
        }
        else {
            int position = pos - begin();
            std::move((begin() + position + 1), end(), begin() + position);
            --size_;
        }
        size_t index = pos - begin();
        return begin() + index;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& rval)  noexcept {
        std::swap(first, rval.first);
        std::swap(capacity_, rval.capacity_);
        std::swap(size_, rval.size_);
    }

    // Выводит список элементов вектора
    void Print() {
        for (auto elem : (*this)) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }


    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return *(begin() + index);
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        //Проверка на корректность index
        assert(index < size_);
        return *(begin() + index);
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) throw std::out_of_range("");
        return first[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) throw std::out_of_range("");
        return first[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
            return;
        }

        Reserve(new_size);
        for (size_t i = size_; i < new_size; ++i) {
            first[i] = Type(); //Заполнение элементов массива стандартными значениями
        }

        size_ = new_size;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return first.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return first.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return first.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return first.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return ConstIterator(first.Get());
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return ConstIterator(first.Get() + size_);
    }

private:
    ArrayPtr<Type> first = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    //Проверяет, что две последовательности одинаковой длины равны
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
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
    return !(lhs < rhs);
}