#pragma once

#include <iostream>
#include <atomic>
#include <thread>

template<typename T>
class smart_ptr {
public:
	smart_ptr(T * ptr)
		: value(ptr == nullptr ? nullptr : new value_container(ptr))
	{}

	smart_ptr(smart_ptr const& orig) {
		std::lock_guard<std::mutex> lock(orig.mutex);
		value = orig.value;
		if (value != nullptr) {
			value->counter++;
		}
	}

	smart_ptr(smart_ptr && orig)
		: value(orig.value)
	{
		orig.value = nullptr;
	}

	~smart_ptr() {
		reset(nullptr);
	}

	T* get() const {
		std::lock_guard<std::mutex> lock(mutex);
		return value == nullptr ? nullptr : value->ptr;
	}

	T & operator*() const {
		return *get();
	}

	T * operator->() const {
		return get();
	}

	bool operator<(smart_ptr const& ptr) const {
		return get() < ptr.get();
	}

	bool operator>(smart_ptr const& ptr) const {
		return ptr < (*this);
	}

	bool operator<=(smart_ptr const& ptr) const {
		return !(ptr < (*this));
	}

	bool operator>=(smart_ptr const& ptr) const {
		return !((*this) < ptr);
	}

	bool operator==(smart_ptr const& ptr) const {
		return get() == ptr.get();
	}

	bool operator!=(smart_ptr const& ptr) const {
		return !((*this) == ptr);
	}

	operator bool() const {
		return value != nullptr;
	}

	operator int() const = delete;
	operator void*() const = delete;

	smart_ptr& operator=(smart_ptr ptr) {
		std::lock_guard<std::mutex> lock(mutex);
		std::swap(value, ptr.value);
		return *this;
	}

	void reset(T * new_value = nullptr) {
		std::lock_guard<std::mutex> lock(mutex);
		value_container* old = value;
		value = new_value == nullptr ? nullptr : new value_container(new_value);
		if (old != nullptr && --old->counter == 0) {
			delete old->ptr;
			delete old;
		}
	}
private:
	struct value_container {
		std::atomic_int counter;
		T* const ptr;

		value_container(T * ptr)
			: counter(1)
			, ptr(ptr)
		{}
	};

	value_container* value;
	mutable std::mutex mutex;
};


template<typename T, typename... Args>
smart_ptr<T> make_smart(Args&&... args) {
	return smart_ptr<T>(new T(std::forward<Args>(args)...));
}