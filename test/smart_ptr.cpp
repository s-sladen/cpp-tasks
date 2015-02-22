#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE smart_ptr
#include <boost/test/unit_test.hpp>

#include <smart_ptr.h>

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