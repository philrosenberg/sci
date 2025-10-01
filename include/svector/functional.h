#ifndef SCI_FUNCTIONAL_H
#define SCI_FUNCTIONAL_H
namespace sci
{
	template<class T, class U>
	auto plus(const T& a, const U& b)
	{
		return a + b;
	}

	template<class T, class U>
	auto minus(const T& a, const U& b)
	{
		return a - b;
	}

	template<class T, class U>
	auto multiplies(const T& a, const U& b)
	{
		return a * b;
	}

	template<class T, class U>
	auto divides(const T& a, const U& b)
	{
		return a / b;
	}

	template<class T, class U>
	auto modulus(const T& a, const U& b)
	{
		return a % b;
	}

	template<class T, class U>
	auto spaceship(const T& a, const U& b)
	{
		return a <=> b;
	}

	template<class T, class U>
	auto less(const T& a, U b)
	{
		return a < b;
	}

	template<class T, class U>
	auto less_equal(const T& a, const U& b)
	{
		return a <= b;
	}

	template<class T, class U>
	auto greater(const T& a, const U& b)
	{
		return a > b;
	}

	template<class T, class U>
	auto greater_equal(const T& a, const U& b)
	{
		return a >= b;
	}

	template<class T, class U>
	auto equal_to(const T& a, const U& b)
	{
		return a == b;
	}

	template<class T, class U>
	auto not_equal_to(const T& a, const U& b)
	{
		return a != b;
	}

	template<class T, class U>
	auto logical_or(const T& a, const U& b)
	{
		return a || b;
	}

	template<class T, class U>
	auto logical_and(const T& a, const U& b)
	{
		return a && b;
	}

	template<class T>
	auto logical_not(const T& a)
	{
		return !a;
	}

	template<class T, class U>
	auto plus_assign(T &a, const U& b)
	{
		return a += b;
	}

	template<class T, class U>
	auto minus_assign(T& a, const U& b)
	{
		return a -= b;
	}

	template<class T, class U>
	auto multiplies_assign(T& a, const U& b)
	{
		return a *= b;
	}

	template<class T, class U>
	auto divides_assign(T& a, const U& b)
	{
		return a /= b;
	}

	template<class T, class U>
	auto modulus_assign(T& a, const U& b)
	{
		return a %= b;
	}

	template<class T, class U>
	auto and_assign(T& a, const U& b)
	{
		return a &= b;
	}

	template<class T, class U>
	auto or_assign(T& a, const U& b)
	{
		return a |= b;
	}

	template<class T>
	auto identity(const T& a)
	{
		return +a;
	}

	template<class T>
	auto negate(const T& a)
	{
		return -a;
	}

	template<class T>
	auto prefix_increment(T &a)
	{
		return ++a;
	}

	template<class T>
	auto postfix_increment(T& a)
	{
		return a++;
	}

	template<class T>
	auto prefix_decrement(T& a)
	{
		return --a;
	}

	template<class T>
	auto postfix_decrement(T& a)
	{
		return a--;
	}

	template<class T, class U>
	auto bit_and(const T& a, const U& b)
	{
		return a & b;
	}

	template<class T, class U>
	auto bit_or(const T& a, const U& b)
	{
		return a | b;
	}

	template<class T, class U>
	auto bit_xor(const T& a, const U& b)
	{
		return a ^ b;
	}

	template<class T, class U>
	auto bit_not(const T& a)
	{
		return ~a;
	}
}

#endif