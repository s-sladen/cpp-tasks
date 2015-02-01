#include <iostream>
#include <atomic>
#include <thread>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE test_smart_ptr
#include <boost/test/unit_test.hpp>

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

BOOST_AUTO_TEST_CASE(smart_ptr_construct)
{
	int* x = new int(100);
	smart_ptr<int> a = x;
	BOOST_CHECK_EQUAL(a.get(), x);
	BOOST_CHECK_EQUAL(*a, *x);
	BOOST_CHECK((bool) a);

	smart_ptr<int> b = nullptr;
	BOOST_CHECK_EQUAL(b.get(), (int*)nullptr);
	BOOST_CHECK(!b);

	b = a;
	BOOST_CHECK_EQUAL(b.get(), x);
	BOOST_CHECK_EQUAL(*b, *x);
	BOOST_CHECK((bool) b);
}

struct deletion_counter {
	std::atomic_int& deletions;
	
	deletion_counter(std::atomic_int& deletions)
		: deletions(deletions)
	{}

	~deletion_counter() {
		deletions++;
	}
};

BOOST_AUTO_TEST_CASE(smart_ptr_single_thread_counting) 
{
	std::atomic_int counter(0);
	auto a = make_smart<deletion_counter>(counter);
	auto b = a;
	a.reset();
	a.reset();
	b.reset();
	b.reset();
	BOOST_CHECK_EQUAL(counter, 1);
}

BOOST_AUTO_TEST_CASE(smart_ptr_multithread_counting) 
{
	for (size_t it = 0; it < 1000; ++it) {
		std::atomic_int counter_a(0);
		std::atomic_int counter_b(0);
		auto a = make_smart<deletion_counter>(counter_a);
		auto b = make_smart<deletion_counter>(counter_b);
		std::thread thread_a([&a, &b]() {
			a = b;
		});
		std::thread thread_b([&a, &b]() {
			b = a;
		});
		thread_a.join();
		thread_b.join();
		BOOST_CHECK(counter_a == 1 && counter_b == 0 
			|| counter_a == 0 && counter_b == 1 
			|| counter_a == 0 && counter_b == 0);
	}
}