#include "CustomVector.h"
#include <iostream>
#include <type_traits>
#include <utility>

//Constructor
template <typename T> CustomVector<T>::CustomVector() : data(nullptr), vectorSize(0), capacity(0) {}

// Destructor
template <typename T> CustomVector<T>::~CustomVector() { clear(); }

// ----------------------------------------------------------------------
// Rule of Five
// ----------------------------------------------------------------------

// Copy constructor — deep copy for pointer types
template <typename T>
CustomVector<T>::CustomVector(const CustomVector& other)
	: data(nullptr), vectorSize(other.vectorSize), capacity(other.capacity)
{
	data = new T[capacity];
	for (std::size_t i = 0; i < vectorSize; ++i)
	{
		if constexpr (std::is_pointer<T>::value)
		{
			if (other.data[i])
			{
				if constexpr (requires { other.data[i]->clone(); })
				{
					data[i] = other.data[i]->clone(); // deep copy via clone()
				}
				else
				{
					data[i] = new std::remove_pointer_t<T>(*other.data[i]); // fallback
				}
			}
			else
			{
				data[i] = nullptr;
			}
		}
		else
		{
			data[i] = other.data[i];
		}
	}
}

// Move constructor — steal resources
template <typename T>
CustomVector<T>::CustomVector(CustomVector&& other) noexcept
	: data(other.data), vectorSize(other.vectorSize), capacity(other.capacity)
{
	other.data = nullptr;
	other.vectorSize = 0;
	other.capacity = 0;
}

// Copy assignment — deep copy for pointer types
template <typename T> CustomVector<T>& CustomVector<T>::operator=(const CustomVector& other)
{
	if (this != &other)
	{
		clear();
		vectorSize = other.vectorSize;
		capacity = other.capacity;
		data = new T[capacity];

		for (std::size_t i = 0; i < vectorSize; ++i)
		{
			if constexpr (std::is_pointer<T>::value)
			{
				if (other.data[i])
				{
					if constexpr (requires { other.data[i]->clone(); })
					{
						data[i] = other.data[i]->clone();
					}
					else
					{
						data[i] = new std::remove_pointer_t<T>(*other.data[i]);
					}
				}
				else
				{
					data[i] = nullptr;
				}
			}
			else
			{
				data[i] = other.data[i];
			}
		}
	}
	return *this;
}

// Move assignment — free current, steal other's buffer
template <typename T> CustomVector<T>& CustomVector<T>::operator=(CustomVector&& other) noexcept
{
	if (this != &other)
	{
		clear();

		data = other.data;
		vectorSize = other.vectorSize;
		capacity = other.capacity;

		other.data = nullptr;
		other.vectorSize = 0;
		other.capacity = 0;
	}
	return *this;
}

// ----------------------------------------------------------------------
// Element management
// ----------------------------------------------------------------------

// Push back a new element
template <typename T> void CustomVector<T>::push_back(const T& value)
{
	if (vectorSize == capacity)
		resize(capacity == 0 ? 1 : capacity * 2);
	data[vectorSize++] = value;
}

// Pop the last element
template <typename T> void CustomVector<T>::pop_back()
{
	if (vectorSize == 0)
		throw std::out_of_range("Vector is empty");
	--vectorSize;
	if constexpr (std::is_pointer<T>::value)
	{
		delete data[vectorSize];
	}
}

// Erase element at index
template <typename T> void CustomVector<T>::erase(std::size_t index)
{
	if (index >= vectorSize)
		throw std::out_of_range("Index out of bounds");

	if constexpr (std::is_pointer<T>::value)
	{
		delete data[index];
	}

	for (std::size_t i = index; i < vectorSize - 1; ++i)
		data[i] = std::move(data[i + 1]);
	--vectorSize;
}

// Extract element at index (removes from vector without deleting)
template <typename T> T CustomVector<T>::extract(std::size_t index)
{
	if (index >= vectorSize)
	{
		throw std::out_of_range("CustomVector::extract index out of range");
	}

	T element = data[index]; // take ownership (shallow copy)

	// Shift elements left
	for (std::size_t i = index; i + 1 < vectorSize; ++i)
	{
		data[i] = data[i + 1];
	}
	--vectorSize;

	// Do not delete element
	return element;
}


// Clear the vector, deleting owned pointers if applicable
template <typename T> void CustomVector<T>::clear()
{
	if (data)
	{
		if constexpr (std::is_pointer<T>::value)
		{
			for (std::size_t i = 0; i < vectorSize; ++i)
				delete data[i];
		}
		delete[] data;
	}
	data = nullptr;
	vectorSize = 0;
	capacity = 0;
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

// Resize internal buffer
template <typename T> void CustomVector<T>::resize(std::size_t newCapacity)
{
	if (newCapacity == capacity)
		return;

	T* newData = new T[newCapacity];
	std::size_t copyCount = (newCapacity < vectorSize) ? newCapacity : vectorSize;

	for (std::size_t i = 0; i < copyCount; ++i)
		newData[i] = data[i];

	if constexpr (std::is_pointer<T>::value)
	{
		if (newCapacity < vectorSize)
		{
			for (std::size_t i = newCapacity; i < vectorSize; ++i)
				delete data[i];
		}
	}

	delete[] data;
	data = newData;
	capacity = newCapacity;
	if (vectorSize > capacity)
		vectorSize = capacity;
}

// ----------------------------------------------------------------------
// Accessors
// ----------------------------------------------------------------------
template <typename T> std::size_t CustomVector<T>::size() const { return vectorSize; }

template <typename T> std::size_t CustomVector<T>::getCapacity() const { return capacity; }

template <typename T> bool CustomVector<T>::isEmpty() const { return vectorSize == 0; }

template <typename T> T& CustomVector<T>::operator[](std::size_t index)
{
	if (index >= vectorSize)
		throw std::out_of_range("Index out of bounds");
	return data[index];
}

template <typename T> const T& CustomVector<T>::operator[](std::size_t index) const
{
	if (index >= vectorSize)
		throw std::out_of_range("Index out of bounds");
	return data[index];
}

template <typename T> T* CustomVector<T>::begin() { return data; }

template <typename T> T* CustomVector<T>::end() { return data + vectorSize; }

template <typename T> const T* CustomVector<T>::begin() const { return data; }

template <typename T> const T* CustomVector<T>::end() const { return data + vectorSize; }