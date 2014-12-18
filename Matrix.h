#ifndef SSS_MARIX_H
#define SSS_MARIX_H

#include <type_traits>
#include <cstring> // for memset

#include "AssertUtil.h"
#include "math\Vector.h"

// tsMatrix[row][column]

template<typename T, int r, int c>
struct tsMatrix
{
	tsMatrix(){}

	explicit tsMatrix(T v)
	{
		int num = r * c;
		T *s = (T *)m_val; 
		for (int i = 0; i < num; ++i)
		{
			*(s + i) = v;
		}
	}

	template<typename U>
	tsMatrix(typename std::enable_if<std::is_convertible<U, T>::value && 3 == r && 3 == c, T>::type
			 v00, U v01, T v02, 
		   T v10, T v11, T v12, 
		   T v20, T v21, T v22)
	{
		m_val[0][0] = v00; m_val[0][1] = (T)v01; m_val[0][2] = v02;
		m_val[1][0] = v10; m_val[1][1] = v11;    m_val[1][2] = v12;
		m_val[2][0] = v20; m_val[2][1] = v21;    m_val[2][2] = v22;
	}

	template<typename U>
	tsMatrix(typename std::enable_if<std::is_convertible<U, T>::value && 4 == r && 4 == c, T>::type
		     val00, U val01, T val02, T val03,
		   T val10, T val11, T val12, T val13,
		   T val20, T val21, T val22, T val23,
		   T val30, T val31, T val32, T val33)
	{
		m_val[0][0] = val00; m_val[0][1] = (T)val01; m_val[0][2] = val02; m_val[0][3] = val03;
		m_val[1][0] = val10; m_val[1][1] = val11;    m_val[1][2] = val12; m_val[1][3] = val13;
		m_val[2][0] = val20; m_val[2][1] = val21;    m_val[2][2] = val22; m_val[2][3] = val23;
		m_val[3][0] = val30; m_val[3][1] = val31;    m_val[3][2] = val32; m_val[3][3] = val33;
	}

	// row index, cannot use unsigned int as parameter
	// seems like you can actually use negative for indexing,
	// char *test0 = (char *)malloc(2);
	// char *test1 = test0 + 1;
	// test1[-1] = 'a'; // now test0[0] == 'a'
	T *operator [] (int i)
	{
		tsDebugAssert(i < r);
		return m_val[i];
	}

	const T *operator [] (int i) const
	{
		tsDebugAssert(i < r);
		return (const T *)m_val[i];
	}

	void operator = (const tsMatrix<T, r, c> &m)
	{
		for (int i = 0; i < r; ++i)
		{
			for (int j = 0; j < c; ++j)
			{
				m_val[i][j] = m[i][j];
			}
		}
	}

	// implicitly convert the Matrix to float *,
	// or explicitly (float *)matrix
	operator const T *()
	{
		return &m_val[0][0];
	}

	typename std::enable_if<(3 == r && 3 == c) || (4 == r && 4 == c), void>::type 
		setAxisX(T x, T y, T z)
	{
		m_val[0][0] = x;
		m_val[1][0] = y;
		m_val[2][0] = z;
	}

	typename std::enable_if<(3 == r && 3 == c) || (4 == r && 4 == c), void>::type 
		setAxisY(T x, T y, T z)
	{
		m_val[0][1] = x;
		m_val[1][1] = y;
		m_val[2][1] = z;
	}

	typename std::enable_if<(3 == r && 3 == c) || (4 == r && 4 == c), void>::type 
		setAxisZ(T x, T y, T z)
	{
		m_val[0][2] = x;
		m_val[1][2] = y;
		m_val[2][2] = z;
	}

	typename std::enable_if<(3 == r && 3 == c) || (4 == r && 4 == c), tsVector<T, 3>>::type 
		axisX()
	{
		return tsVector<T, 3>(m_val[0][0], m_val[1][0], m_val[2][0]);
	}

	//template<typename U = std::enable_if<(3 == r && 3 == c) || (4 == r && 4 == c), T>::type>
	typename std::enable_if<(3 == r && 3 == c) || (4 == r && 4 == c), tsVector<T, 3>>::type 
		axisY()
	{
		return tsVector<T, 3>(m_val[0][1], m_val[1][1], m_val[2][1]);
	}

	typename std::enable_if<(3 == r && 3 == c) || (4 == r && 4 == c), tsVector<T, 3>>::type 
		axisZ()
	{
		return tsVector<T, 3>(m_val[0][2], m_val[1][2], m_val[2][2]);
	}

	typename std::enable_if<(3 == r && 3 == c) || (4 == r && 4 == c), tsVector<T, 3>>::type
		getPos()
	{
		return tsVector<T, 3>(m_val[0][3], m_val[1][3], m_val[2][3]);
	}

	T m_val[r][c];
};

typedef tsMatrix<float, 3, 3> Mat33f;
typedef tsMatrix<float, 4, 4> Mat44f;

template<typename T, int r, int s, int c>
tsMatrix<T, r, c> operator * (const tsMatrix<T, r, s> &m0, const tsMatrix<T, s, c> &m1)
{
	tsMatrix<T, r, c> res;
	tsZero(res);

	for (int i = 0; i < r; ++i)
	{
		for (int j = 0; j < c; ++j)
		{
			for(int k = 0; k < s; ++k)
			{
				res[i][j] += m0[i][k] * m1[k][j];
			}
		}
	}

	return res;
}

template<typename T, int r, int c>
tsVector<T, r> operator * (const tsMatrix<T, r, c> &m, const tsVector<T, c> &v)
{
	tsVector<T, r> res;
	tsZero(res);
	for (int i = 0; i < r; ++i)
	{
		for (int j = 0; j < c; ++j)
		{
			res[i] += m[i][j] * v[j];
		}
	}
	return res;
}

template<typename T, int r, int c>
tsVector<T, c> operator * (const tsVector<T, r> &v, const tsMatrix<T, r, c> &m)
{
	tsVector<T, c> res;
	tsZero(res);
	for (int i = 0; i < c; ++i)
	{
		for (int j = 0; j < r; ++j)
		{
			res[i] += m[j][i] * v[j];
		}
	}
}

template<typename T, int r, int c>
tsMatrix<T, r, c> operator * (T s, const tsMatrix<T, r, c> &m)
{
	tsMatrix<T, r, c> res;
	for (int i = 0; i < r; ++i)
	{
		for (int j = 0; j < c; ++j)
		{
			res[i][j] += s * m[i][j];
		}
	}
}

template<typename T, int r, int c>
tsMatrix<T, r, c> operator * (const tsMatrix<T, r, c> &m, T s)
{
	tsMatrix<T, r, c> res;
	for (int i = 0; i < r; ++i)
	{
		for (int j = 0; j < c; ++j)
		{
			res[i][j] += m[i][j] * s;
		}
	}
}

// fill in element row by row
template<typename T, int r, int c>
void tsSet(tsMatrix<T, r, c> &m, T *ptr, int num = r * c)
{
	tsDebugAssert(num <= r * c);
	T *s = (T *)(m.m_val);
	for (int i = 0; i < num; ++i)
	{
		*(s + i) = ptr[i];
	}
}

template<typename T, int r, int c>
void tsZero(tsMatrix<T, r, c> &m)
{
	T *s = (T *)(m.m_val);
	memset(s, 0, sizeof(T) * r * c);
}

template<typename T, int s>
void tsIdentity(tsMatrix<T, s, s> &m)
{
	tsZero(m);

	for (int i = 0; i < s; ++i)
	{
		m.m_val[i][i] = 1;
	}
}

template<typename T, int r, int c>
tsMatrix<T, c, r> tsTranspose(const tsMatrix<T, r, c> &m)
{
	tsMatrix<T, c, r> res;
	for (int i = 0; i < r; ++i)
	{
		for (int j = 0; j < c; ++j)
		{
			res[j][i] = m[i][j];
		}
	}
	return res;
}

template<typename T, int s>
void tsTranspose(tsMatrix<T, s, s> &m)
{
	for (int i = 0; i < s - 1; ++i)
	{
		for (int j = i + 1; j < s; ++j)
		{
			T tmp = m[i][j];
			m[i][j] = m[j][i];
			m[j][i] = tmp;
		}
	}
}

#endif // SSS_MARIX_H