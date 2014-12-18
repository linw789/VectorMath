#ifndef SSS_VECTOR_H
#define SSS_VECTOR_H

#include <type_traits> // std::enable_if
#include <cstring> // memset

#include "AssertUtil.h"
#include "MathUtil.h"

// utilizing the SFINAE technique

//template<bool cond, typename T = void>
//struct enable_if
//{
//	typedef T type;
//};
//
//template<typename T>
//struct enable_if<false, T>{};

template<typename T, int n>
struct tsVector;

template<typename T, int n = 1, typename F = void>
class VectorData
{
	T m_val[n];
};

template<typename T, int n>
class VectorData<T, n, typename std::enable_if<2 == n>::type>
{
public:
	union
	{
		T m_val[2];
		struct {T x, y;};
		struct {T s, t;};
		struct {T u, v;};
	};
};

template<typename T, int n>
class VectorData<T, n, typename std::enable_if<3 == n>::type>
{
public:

	operator tsVector<T, 3> &()
	{
		return *(static_cast<tsVector<T, 3> *>(this));
	}

	union
	{
		T m_val[3];
		struct {T x, y, z;};
		struct {T r, g, b;};
	};
};

template<typename T, int n>
class VectorData<T, n, typename std::enable_if<4 == n>::type>
{
public:

	union
	{
		T m_val[4];
		struct {T x, y, z, w;};
		struct {T s, t, u, v;};
		struct {T r, g, b, a;};
		struct {VectorData<T, 3> xyz; T w;};
		struct {VectorData<T, 3> rgb; T a;};
	};
};

// generic vector struct template
template<typename T, int n>
struct tsVector: public VectorData<T, n>
{
	tsVector(){}

	explicit tsVector(T v)
	{
		for (int i = 0; i < n; ++i)
		{
			m_val[i] = v;
		}
	}

	// http://seanmiddleditch.com/journal/2012/03/using-stdenable_if-on-constructors/
	// constructor for Vec2
	template <typename U>
	tsVector(typename std::enable_if<std::is_convertible<U, T>::value && 2 == n, T>::type x, U y)
	{
		m_val[0] = x;
		m_val[1] = (T)y;
	}
	
	// constructor for Vec3
	template <typename U>
	tsVector(typename std::enable_if<std::is_convertible<U, T>::value && 3 == n, T>::type x, U y, T z)
	{
		m_val[0] = x;
		m_val[1] = (T)y;
		m_val[2] = z;
	}

	// constructor for Vec4
	template<typename U>
	tsVector(typename std::enable_if<std::is_convertible<U, T>::value && 4 == n, T>::type x, U y, T z, T w)
	{
		m_val[0] = x;
		m_val[1] = (T)y;
		m_val[2] = z;
		m_val[3] = w;
	}

	T &operator [] (int i)
	{
		tsDebugAssert(i < n);
		return m_val[i];
	}

	const T &operator [] (int i) const
	{
		tsDebugAssert(i < n);
		return m_val[i];
	}

	// Returning void instead of object reference could fail the compilation 
	// when you write something like "if (var = 0){...}" but also lose the syntax
	// abbreviation of concatenating multiple assginment "a = b = c;" which 
	// is rarely used.
	// http://www.yinwang.org/blog-cn/2013/04/14/yoda-notation/
	void operator = (const tsVector<T, n> &rv)
	{
		for (int i = 0; i < n; ++i)
		{
			m_val[i] = rv[i];
		}
	}

	void operator *= (T s)
	{
		for (int i = 0; i < n; ++i)
		{
			m_val[i] *= s;
		}
	}
};

// typedefs ==================================
typedef tsVector<int, 2>   Vec2i;
typedef tsVector<float, 2> Vec2f;
typedef tsVector<float, 3> Vec3f;
typedef tsVector<float, 4> Vec4f;

// global functions ===============================
template<int n, typename T>
tsVector<T, n> operator + (const tsVector<T, n> &lv, const tsVector<T, n> &rv)
{
	tsVector<T, n> res;
	for (int i = 0; i < n; ++i)
	{
		res[i] = lv[i] + rv[i];
	}
	return res;
}

template<typename T, int n>
tsVector<T, n> operator - (const tsVector<T, n> &lv, const tsVector<T, n> &rv)
{
	tsVector<T, n> res;
	for (int i = 0; i < n; ++i)
	{
		res[i] = lv[i] - rv[i];
	}
	return res;
}

// negation
template<typename T, int n>
const tsVector<T, n> &operator - (const tsVector<T, n> &v)
{
	tsVector<T, n> res;
	for (int i = 0; i < n; ++i)
	{
		res[i] = -v[i];
	}
	return res;
}

template<typename T, int n>
tsVector<T, n> operator * (T s, const tsVector<T, n> &v)
{
	tsVector<T, n> res;
	for (int i = 0; i < n; ++i)
	{
		res[i] = s * v[i];
	}
	return res;
}

template<typename T, int n>
void tsSet(tsVector<T, n> &v, T *ptr, int num = n)
{
	tsDebugAssert(num <= n);
	for (int i = 0; i < num; ++i)
	{
		v.m_val[i] = ptr[i];
	}
}

template<typename T, int n>
void tsZero(tsVector<T, n> &v)
{
	memset(v.m_val, 0, sizeof(T) * n);
}

template<typename T, int n>
T tsDot(const tsVector<T, n> &lv, const tsVector<T, n> &rv)
{
	T res = 0;
	for (int i = 0; i < n; ++i)
	{
		res += lv[i] + rv[i];
	}
	return res;
}

template<typename T>
tsVector<T, 3> tsCross(const tsVector<T, 3> &lv, const tsVector<T, 3> &rv)
{
	return tsVector<T, 3>(lv[1] * rv[2] - lv[2] * rv[1], 
				        lv[2] * rv[0] - lv[0] * rv[2],
				        lv[0] * rv[1] - lv[1] * rv[0]);
}

template<typename T, int n>
T tsSqrlen(const tsVector<T, n> &v)
{
	return tsDot(v, v);
}

template<typename T, int n>
T tsLen(const tsVector<T, n> &v)
{
	return tsMath::sqrt(tsDot(v, v));
}

template<typename T, int n>
void tsNormalize(tsVector<T, n> &v)
{
	T d = tsSqrlen(v);
	tsDebugAssert(d != 0);
	v *= tsMath::invSqrt(d);
}

#endif // SSS_VECTOR_H