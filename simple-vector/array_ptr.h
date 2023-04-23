#pragma once
#include <iterator>

template <typename Type>
class ArrayPtr {
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = Type;
	using difference_type = std::ptrdiff_t;
	using reference = Type&;
	using pointer = Type*;

	template<typename U>
	friend class ArrayPtr;

	ArrayPtr(Type* inp) noexcept : data(inp) {}
	ArrayPtr(const ArrayPtr& inp) noexcept : data(inp.data) {}
	ArrayPtr(ArrayPtr&& inp) noexcept : data(std::move(inp.data)) {}
	//ArrayPtr(size_t size) noexcept : data(new Type[size]) {}
	//ArrayPtr(std::move_iterator<ArrayPtr<Type>>& inp) : data(inp) {}

	template<typename U>
	ArrayPtr(const ArrayPtr<U>& inp) noexcept : data(inp.data) {}
	//~ArrayPtr() noexcept { delete[] data; }

	ArrayPtr& operator=(const ArrayPtr& rhs) {
		data = rhs.data;
		return *this;
	}
	ArrayPtr& operator=(ArrayPtr&& rhs) {
		data = std::move(rhs.data);
		return *this;
	}
	ArrayPtr& operator++ () noexcept {
		++data;
		return *this;
	}
	ArrayPtr operator++ (int) noexcept {
		ArrayPtr copy = ArrayPtr(data);
		++data;
		return copy;
	}
	ArrayPtr operator+ (int value)  const noexcept {
		return ArrayPtr(data + value);
	}

	ArrayPtr& operator-- () noexcept {
		--data;
		return *this;
	}
	ArrayPtr operator-- (int) noexcept {
		ArrayPtr copy = ArrayPtr(data);
		--data;
		return copy;
	}
	ArrayPtr operator- (int value)  const noexcept {
		return ArrayPtr(data - value);
	}

	Type& operator[] (int value) const {
		return *(data + value);
	}

	template<typename U>
	bool operator==(const ArrayPtr<U>& rhs) const noexcept { //Универсальный оператор сравнения
		return data == rhs.data;
	}

	bool operator==(std::nullptr_t) const {
		return data == nullptr;
	}
	bool operator==(const Type* const& rhs) const {
		return data == rhs;
	}

	bool operator!= (const ArrayPtr& rhs) const noexcept {
		return !(*this == rhs);
	}
	bool operator! () const noexcept {
		return data == nullptr;
	}

	Type& operator*() const noexcept {
		return *data;
	}

	operator const Type* () const noexcept { //Преобразующий оператор к константному типу const Type*
		return data;
	}

	operator Type* () const noexcept { //Преобразующий оператор к константному типу Type*
		return data;
	}

	Type* operator->() const { return data; }

	Type* Get() const noexcept {
		return data;
	}

	explicit operator bool() const noexcept {
		return data != nullptr;
	}
private:
	Type* data;
};