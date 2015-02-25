#pragma once

#include <iostream>
#include <atomic>
#include <thread>

/**
* Thread-safe reference counting pointer wrapper.
* Destroys underlying value when reference count equals 0.
* @tparam T type of underlying value
*/
template<typename T>
class smart_ptr {
public:
    /**
    * Constructs wrapper with given pointer
    * @param ptr pointer to wrap
    */
	smart_ptr(T * ptr = nullptr)
		: value(ptr == nullptr ? nullptr : new value_container(ptr))
	{}

    /**
    * Copies given smart_ptr increasing underlying reference counter if it's value isn't nullptr
    * @param orig value to copy
    */
	smart_ptr(smart_ptr const& orig) {
		std::lock_guard<std::mutex> lock(orig.mutex);
		value = orig.value;
		if (value != nullptr) {
			value->counter++;
		}
	}

    /**
    * Moves pointer from given smart_ptr to newly constructed smart_ptr. Reference counter isn't increased.
    * @param org value to move
    */
	smart_ptr(smart_ptr && orig)
		: value(orig.value)
	{
		orig.value = nullptr;
	}

    /**
    * Destroys this wrapper decreasing reference counter and destroying underlying value if needed
    */
	~smart_ptr() {
		reset(nullptr);
	}

    /**
    * Returns underlying pointer
    * @return underlying pointer
    */
	T* get() const {
		std::lock_guard<std::mutex> lock(mutex);
		return value == nullptr ? nullptr : value->ptr;
	}

    /**
    * Dereferences the underlying pointer
    * @return reference to the underlying value
    */
	T & operator*() const {
		return *get();
	}

    /**
    * Returns underlying pointer
    * @return underlying pointer
    */
	T * operator->() const {
		return get();
	}

    /**
    * Checks if underlying pointer of this smart_ptr is less then underlying pointer of given smart_ptr
    * @param ptr smart_ptr to compare with
    * @return true if this->get() < ptr.get(), false otherwise
    */
	bool operator<(smart_ptr const& ptr) const {
		return get() < ptr.get();
	}

    /**
    * Checks if underlying pointer of this smart_ptr is greater then underlying pointer of given smart_ptr
    * @param ptr smart_ptr to compare with
    * @return true if this->get() > ptr.get(), false otherwise
    */
	bool operator>(smart_ptr const& ptr) const {
		return ptr < (*this);
	}

    /**
    * Checks if underlying pointer of this smart_ptr is less then or equal to underlying pointer of given smart_ptr
    * @param ptr smart_ptr to compare with
    * @return true if this->get() <= ptr.get(), false otherwise
    */
	bool operator<=(smart_ptr const& ptr) const {
		return !(ptr < (*this));
	}

    /**
    * Checks if underlying pointer of this smart_ptr is greater then or equal to underlying pointer of given smart_ptr
    * @param ptr smart_ptr to compare with
    * @return true if this->get() >= ptr.get(), false otherwise
    */
	bool operator>=(smart_ptr const& ptr) const {
		return !((*this) < ptr);
	}

    /**
    * Checks if underlying pointer of this smart_ptr is equal to underlying pointer of given smart_ptr
    * @param ptr smart_ptr to compare with
    * @return true if this->get() == ptr.get(), false otherwise
    */
	bool operator==(smart_ptr const& ptr) const {
		return get() == ptr.get();
	}

    /**
    * Checks if underlying pointer of this smart_ptr is not equal to underlying pointer of given smart_ptr
    * @param ptr smart_ptr to compare with
    * @return true if this->get() != ptr.get(), false otherwise
    */
	bool operator!=(smart_ptr const& ptr) const {
		return !((*this) == ptr);
	}

    /**
    * Checks if underlying pointer doesn't point to null
    * @return true if this->get() != nullptr, false otherwise
    */
	operator bool() const {
		return value != nullptr;
	}

    /**
    * Cast to int is prohibited
    */
	operator int() const = delete;
    /**
    * Cast to void* is prohibited
    */
	operator void*() const = delete;

    /**
    * Assign given smart_ptr to this performing all necessary reference counter manipulations.
    * @param ptr value to assign
    * @return this
    */
	smart_ptr& operator=(smart_ptr ptr) {
		std::lock_guard<std::mutex> lock(mutex);
		std::swap(value, ptr.value);
		return *this;
	}

    /**
    * Assigns new pointer to this smart_ptr, decreasing reference counter and destroying underlying value if needed.
    * @param new_value pointer to assign
    */
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

/**
* Creates smart_ptr with pointer obtained by calling given type's constructor with given arguments.
* @tparam T type of underlying value
* @tparam Args arguments types
*/
template<typename T, typename... Args>
smart_ptr<T> make_smart(Args&&... args) {
	return smart_ptr<T>(new T(std::forward<Args>(args)...));
}