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
        Resize(size_, inp.size);
    }

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        Resize(size);
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
        Resize(size);
        std::fill(begin(), end(), value);
        size_ = size;
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        Resize(init.size());
        std::copy(init.begin(), init.end(), begin());
    }

    // Конструктор копирования
    SimpleVector(const SimpleVector& init) {
        Resize(init.GetSize());
        ArrayPtr iterator(first);
        for (const Type& value : init) {
            *iterator = value;
            ++iterator;
        }
    }

    // Конструктор перемещения
    SimpleVector(SimpleVector&& init) {
        Resize(init.GetSize());
        ArrayPtr iterator(first);
        for (auto& value : init) {
            *iterator = std::move(value);
            ++iterator;
        }
        init.size_ = 0;
    }

    //Деструктор
    ~SimpleVector() {
        delete[] first.Get();
    }

    SimpleVector& operator=(const SimpleVector& rval) {
        if (first == rval.first && GetSize() == rval.GetSize()) {
            return *this;
        }
        SimpleVector copy(rval);
        swap(copy);
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rval) {
        if (first == rval.first && GetSize() == rval.GetSize()) {
            return *this;
        }
        SimpleVector copy(std::move(rval));
        swap(copy);
        return *this;
    }

    void Reserve(size_t new_capacity) {
        Resize(size_, new_capacity);
    };

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type item) {
        if (size_ + 1 > capacity_) {
            if (size_ != 0) { Resize(size_ + 1, size_ * 2); }
            if (size_ == 0) { Resize(1); }
        }
        else {
            ++size_;
        }
        *(end() - 1) = std::move(item);
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, Type&& value) {

        SimpleVector copy;
        if (size_ + 1 > capacity_) {
            if (size_ != 0) { copy.Resize(size_ + 1, size_ * 2); }
            if (size_ == 0) { copy.Resize(1); }

            size_t position = pos - first;

            //Перемещаем первую половину значений 
            auto iter = MoveCopy(first, first + position, copy.first);

            //Вставляем новое значение
            *iter = std::move(value);

            //Перемещаем вторую половину значений
            if (iter != copy.end()) {
                MoveCopy(first + position, end(), iter + 1);
            }

            swap(copy);
            return first + position;
        }
        else {
            size_t position = pos - first;
            //Сдвигаем вторую половину значений вправо
            MoveRight(first + position, end());

            //Вставляем новое значение
            first[position] = std::move(value);
            ++size_;
            return (first + position).Get();
        }
    }

    Iterator Insert(ConstIterator pos, const Type& value) {

        SimpleVector copy;
        if (size_ + 1 > capacity_) {
            if (size_ != 0) { copy.Resize(size_ + 1, size_ * 2); }
            if (size_ == 0) { copy.Resize(1); }

            //Перемещаем первую половину значений 
            Iterator iter = std::copy(cbegin(), pos, copy.begin());

            //Вставляем новое значение
            *iter = value;

            //Перемещаем вторую половину значений
            std::copy_backward(pos, cend(), copy.end());
            swap(copy);
            return iter;
        }
        else {
            //Перемещаем первую половину значений
            Iterator iter = std::copy(cbegin(), pos, begin());

            //Вставляем новое значение
            *iter = value;

            //Перемещаем вторую половину значений
            std::copy_backward(pos, cend(), end());
            ++size_;
            return iter;
        }
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (size_ > 0) {
            Resize(size_ - 1);
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        if (IsEmpty()) {
            throw std::out_of_range("vector_is_empty");
        }
        else if (size_ == 1) {
            --size_;
        }
        else {

            int position = pos - first;
            Moveleft((first + position).Get(), end());
            --size_;
        }
        size_t index = pos - begin();
        return (first + index).Get();
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
        return *(first + index);
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return *(first + index);
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
        Resize(new_size, new_size);
    }

    void Resize(size_t new_size, size_t new_capacity) {
        if (new_size <= size_ && new_capacity <= capacity_) {
            size_ = new_size;
            return;
        }

        Type* copy = new Type[new_capacity + 1];
        ArrayPtr<Type> beginp(copy);
        ArrayPtr<Type> endp(copy);
        for (size_t i = 0; i < new_capacity; ++i) {
            *endp = i < size_ ? std::move(first[i]) : Type(); //копирование элементов текущего массива
            ++endp;
        }
        delete[] first.Get();
        first = beginp;

        size_ = new_size;
        capacity_ = new_capacity;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return first;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return first + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return first;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return first + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return ConstIterator(first);
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return ConstIterator(first + size_);
    }

private:
    ArrayPtr<Type> first = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

    //Сдвигает позиции (begin, end] вправо, возвращает итератор на первый пустрой элемент
    Iterator MoveRight(Iterator begin, Iterator end) {
        for (; end != begin; --end) {
            *end = std::move(*(end - 1));
        }
        return begin;
    }

    //Сдвигает позиции [begin, end) влево, возвращает итератор на последний пустой элемент
    Iterator Moveleft(Iterator begin, Iterator end) {
        for (; begin != end; ++begin) {
            *begin = std::move(*(begin + 1));
        }
        return end;
    }

    //Перемещает без изменений [begin, end), возвращает итератор на элемент, следующий за последним вставленным
    Iterator MoveCopy(Iterator begin, Iterator end, Iterator dest_begin) {

        for (; begin != end; ++begin, ++dest_begin) {
            *dest_begin = std::move(*(begin));
        }
        return dest_begin;
    }
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
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