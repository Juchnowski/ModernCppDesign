#include <exception>
#include <iostream>

template<class T,
	template <class> class CheckingPolicy,
	template <class> class ThreadingModel>
class SmartPtr : public CheckingPolicy<T>, public ThreadingModel<T> {
	// ...
public:
	T* operator->() {
		typename ThreadingModel<SmartPtr>::Lock guard(*this);
		CheckingPolicy<T>::Check(pointee_);
		return pointee_;
	}
private:
	T* pointee_;
};

// checking policies
//
template <class T> struct NoChecking {
	static void Check(T*) {}
};

template <class T> struct EnforceNotNull {
	class NullPointerException : public std::exception { /* ... */};
	static void Check(T*) {
//		if (!ptr) throw NullPointerException();
	}
};

template <class T> struct EnsureNotNull {

public:
	// from the book: "You can even initialize the pointer with a default value by accepting a reference to a pointer" (1.9. Combining Policy Classes)
	static void Check(T*& ptr) { // why reference to pointer?
		if (!ptr) { ptr = GetDefaultValue(); }
	}

private:
	static T s_defaultValue;
	static T* GetDefaultValue() { return &s_defaultValue; }
};
template <class T>
T EnsureNotNull<T>::s_defaultValue = T();



// Threading policies
//
template <class T> struct  SingleThreaded{
	class Lock {
	  public:
			Lock(T& obj) { (void)obj; /* ... */ }
	};

};

class Widget {
public:
	void hello() { std::cout << "Hello" << "\n"; }
};

typedef SmartPtr<Widget, NoChecking, SingleThreaded> WidgetPtr;

typedef SmartPtr<Widget, EnsureNotNull, SingleThreaded> EnsuredWidgetPtr;

int main(void) {
  WidgetPtr widgetPtr;
	widgetPtr->hello();

	EnsuredWidgetPtr ensuredPtr;
	ensuredPtr->hello();

	return 0;
}
