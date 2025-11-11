#pragma once
#include <cstddef>
#include <iostream>
#include <stdexcept>

// Minimal custom vector implementation used by the backend.
// the container owns the pointers
template <typename T> class CustomVector
{
public:
	CustomVector();
	~CustomVector();
	CustomVector(const CustomVector& other);
	CustomVector& operator=(const CustomVector& other);
	CustomVector& operator=(CustomVector&& other) noexcept;
	CustomVector(CustomVector&& other) noexcept;

	void pop_back();
	void push_back(const T& value);
	void erase(std::size_t index);
	T extract(std::size_t index);
	void resize(std::size_t newCapacity);

	T& operator[](std::size_t index);
	const T& operator[](std::size_t index) const;

	std::size_t size() const;
	std::size_t getCapacity() const;

	T* begin();
	T* end();
	const T* begin() const;
	const T* end() const;
	bool isEmpty() const;
	void clear();

private:
	T* data;
	std::size_t vectorSize;
	std::size_t capacity;
};

#include "../../src/BACKEND/CustomVector.tpp"
