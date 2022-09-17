#define BOOST_TEST_MODULE UniquePtrTest
#include <boost/test/included/unit_test.hpp>
#include "MUniquePointer.h"
#include <iostream>



struct MyInt {
    int value;
    static int destructorCounter;

    explicit MyInt(int v = 0) : value(v) {
		std::cout<<"C";

	 }
    ~MyInt() { ++destructorCounter;  
		std::cout<<"D";
	}




	operator int() const { return value; }
};

int MyInt::destructorCounter = 0;

template <typename T>
struct MyDeleter {
	static int deleterCalled;
	void operator() (T* p) const {
		std::cout << "DeleterCalled" <<std::endl;
		deleterCalled++;
		delete p;
	}
};
template <typename T>
struct MyArrDeleter {
	static int deleterCalled;
	void operator() (T* p) const {
		std::cout << "ArrDeleterCalled" <<std::endl;
		deleterCalled++;
		delete [] p;
	}
};
template<> int MyDeleter <MyInt> :: deleterCalled = 0;
template<> int MyArrDeleter <MyInt> :: deleterCalled = 0;

template <typename T>
//using Unique = std::unique_ptr<T>;
using Unique = vz::MUnique_ptr<T>;

template <typename T, typename Deleter>
//using UniqueD = std::unique_ptr<T, Deleter>;
using UniqueD = vz::MUnique_ptr<T, Deleter>;



BOOST_AUTO_TEST_CASE ( Test_Deleter ) {
	{
		UniqueD <MyInt, MyDeleter<MyInt>> val1(new MyInt (8));
		UniqueD <MyInt, MyDeleter<MyInt>> val2(new MyInt (8));
		UniqueD <MyInt, MyDeleter<MyInt>> val3(new MyInt (8));
	}

    BOOST_CHECK(MyDeleter<MyInt>::deleterCalled == 3);

	{
		UniqueD <MyInt[], MyArrDeleter<MyInt>> val1(new MyInt [8]);
		UniqueD <MyInt[], MyArrDeleter<MyInt>> val2(new MyInt [8]);
		UniqueD <MyInt[], MyArrDeleter<MyInt>> val3(new MyInt [8]);
	}
    BOOST_CHECK(MyArrDeleter<MyInt>::deleterCalled == 3);

}

BOOST_AUTO_TEST_CASE ( Test_constructors ) {
	Unique <int> def;
    BOOST_CHECK(!def);

	Unique <int> def2(nullptr);
    BOOST_CHECK(!def2);

	Unique <int> val(new int (8));
    BOOST_CHECK(val);
    BOOST_CHECK(*val == 8);
    BOOST_CHECK(*val.get() == 8);

	Unique <int> valMoved(std::move(val));
    BOOST_CHECK(!val);
    BOOST_CHECK(valMoved);
    BOOST_CHECK(*valMoved == 8);
    BOOST_CHECK(*valMoved.get() == 8);
}

BOOST_AUTO_TEST_CASE ( test_operator_move ) {
	Unique <int> val(new int (8));
    BOOST_CHECK(val);
    BOOST_CHECK(*val == 8);
    BOOST_CHECK(*val.get() == 8);

	Unique <int> valMoved(std::move(val));
    BOOST_CHECK(!val);
    BOOST_CHECK(valMoved);
    BOOST_CHECK(*valMoved == 8);
    BOOST_CHECK(*valMoved.get() == 8);

	val = std::move(valMoved);
    BOOST_CHECK(!valMoved);
    BOOST_CHECK(val);
    BOOST_CHECK(*val == 8);
    BOOST_CHECK(*val.get() == 8);
}

BOOST_AUTO_TEST_CASE ( Test_swap ) {
	Unique <int> val1 (new int (9));
	Unique <int> val2 (new int (40));
    BOOST_CHECK(*val1 == 9);
    BOOST_CHECK(*val2 == 40);

	std::swap(val1, val2);
    BOOST_CHECK(*val2 == 9);
    BOOST_CHECK(*val1 == 40);
}

BOOST_AUTO_TEST_CASE ( Test_reset_and_release ) {

	MyInt::destructorCounter = 0;
	BOOST_CHECK(MyInt::destructorCounter == 0);
	{
		Unique <MyInt> val(new MyInt (8));
		BOOST_CHECK(val);
		BOOST_CHECK((*val).value == 8);
		BOOST_CHECK(val->value == 8);
		BOOST_CHECK(val.get()->value == 8);
	}

	BOOST_CHECK(MyInt::destructorCounter == 1);

	{
		Unique <MyInt> val(new MyInt (8));
		BOOST_CHECK(val);
		BOOST_CHECK((*val).value == 8);
		BOOST_CHECK(val->value == 8);
		BOOST_CHECK(val.get()->value == 8);

		MyInt* vPtr  = val.release();
		BOOST_CHECK(!val);
		BOOST_CHECK(vPtr->value == 8);
		BOOST_CHECK(MyInt::destructorCounter == 1);
		delete vPtr;
		BOOST_CHECK(MyInt::destructorCounter == 2);
	}

	{
		MyInt* vPtr;
		{
			Unique <MyInt> val(new MyInt (8));
			BOOST_CHECK(val);
			BOOST_CHECK((*val).value == 8);
			BOOST_CHECK(val->value == 8);
			BOOST_CHECK(val.get()->value == 8);

			vPtr  = val.release();
		}
		BOOST_CHECK(vPtr->value == 8);
		{
			Unique <MyInt> val(vPtr);
			BOOST_CHECK(MyInt::destructorCounter == 2);
		}
	}
	BOOST_CHECK(MyInt::destructorCounter == 3);

	{
		Unique <MyInt> val(new MyInt (8));
		MyInt* vPtr  = val.release();
		BOOST_CHECK(!val);

		//! checking for additional release
		val.reset();
		val.reset();

		val.reset(vPtr);

		BOOST_CHECK(val);
		BOOST_CHECK((*val).value == 8);
		BOOST_CHECK(val->value == 8);
		BOOST_CHECK(val.get()->value == 8);

	}
	BOOST_CHECK(MyInt::destructorCounter == 4);
}

/* ====================================================================== */
/* ====================================================================== */
/* ====================================================================== */

BOOST_AUTO_TEST_CASE ( Test_constructor_for_arrays ) {
	MyInt::destructorCounter = 0;
	BOOST_CHECK(MyInt::destructorCounter == 0);
	{
		Unique <MyInt[]> arr(new MyInt[20]);
		for (size_t i = 0; i < 20; ++i) {
			arr[i].value = i;
		}

		for (size_t i = 0; i < 20; ++i) {
			BOOST_CHECK(arr[i] == i);
		}
	}
	BOOST_CHECK(MyInt::destructorCounter == 20);
}

BOOST_AUTO_TEST_CASE ( Test_Move_constructor_for_arrays ) {
	MyInt::destructorCounter = 0;
	{
		Unique <MyInt[]> ar1(new MyInt[20]);
		for (size_t i = 0; i < 20; ++i) {
			ar1[i].value = i;
		}
		Unique <MyInt[]> ar2 = std::move(ar1);

		for (size_t i = 0; i < 20; ++i) {
			BOOST_CHECK(ar2[i] == i);
		}
	}
	BOOST_CHECK(MyInt::destructorCounter == 20);
}

BOOST_AUTO_TEST_CASE ( Test_Move_assignment_for_arrays ) {
	MyInt::destructorCounter = 0;
	{
		Unique <MyInt[]> ar1(new MyInt[20]);
		Unique <MyInt[]> ar2(new MyInt[20]);
		for (size_t i = 0; i < 20; ++i) {
			ar1[i].value = i;
			ar2[i].value = i * 2;
		}
		BOOST_CHECK(MyInt::destructorCounter == 0);
		ar1 = std::move(ar2);
		BOOST_CHECK(MyInt::destructorCounter == 20);
		BOOST_CHECK(ar1);
		BOOST_CHECK(!ar2);

		for (size_t i = 0; i < 20; ++i) {
			BOOST_CHECK(ar1[i] == i*2);
		}
	}
	BOOST_CHECK(MyInt::destructorCounter == 40);
}

BOOST_AUTO_TEST_CASE ( Test_SWAP_for_arrays ) {
	MyInt::destructorCounter = 0;
	{
		Unique <MyInt[]> ar1(new MyInt[20]);
		Unique <MyInt[]> ar2(new MyInt[20]);
		for (size_t i = 0; i < 20; ++i) {
			ar1[i].value = i;
			ar2[i].value = i * 2;
		}
		BOOST_CHECK(MyInt::destructorCounter == 0);
		std::swap(ar1, ar2);
		BOOST_CHECK(MyInt::destructorCounter == 0);
		BOOST_CHECK(ar1);
		BOOST_CHECK(ar2);

		for (size_t i = 0; i < 20; ++i) {
			BOOST_CHECK(ar2[i] == i);
			BOOST_CHECK(ar1[i] == i * 2);
		}
	}
	BOOST_CHECK(MyInt::destructorCounter == 40);
}

BOOST_AUTO_TEST_CASE ( Test_RESET_for_arrays ) {
	MyInt::destructorCounter = 0;
	{
		Unique <MyInt[]> ar1(new MyInt[20]);
		Unique <MyInt[]> ar2(new MyInt[20]);
		for (size_t i = 0; i < 20; ++i) {
			ar1[i].value = i;
			ar2[i].value = i * 2;
		}
		ar1.reset();
		ar1.reset();
		BOOST_CHECK(!ar1);
		BOOST_CHECK(ar2);

		MyInt* myIntPtr = ar2.release();
		BOOST_CHECK(!ar2);
		BOOST_CHECK(MyInt::destructorCounter == 20);

		ar1.reset(myIntPtr);
		BOOST_CHECK(ar1);
	}
	BOOST_CHECK(MyInt::destructorCounter == 40);
}


/* */