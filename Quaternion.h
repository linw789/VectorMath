#ifndef SSS_QUATERNION_H
#define SSS_QUATERNION_H

#include "Vector.h"
#include "MathUtil.h"

template<typename T>
struct tsQuaternion
{
	tsQuaternion()
		:x(0), y(0), z(0), w(0) 
	{}

	~tsQuaternion()
	{}

	tsQuaternion(T xx, T yy, T zz, T ww)
		:x(xx), y(yy), z(zz), w(ww)
	{}

	tsQuaternion(const tsVector<T, 3> &v, T s)
		:x(v.x), y(v.y), z(v.z), w(s)
	{}

	tsQuaternion(const tsQuaternion<T> &q)
		:x(q.x), y(q.y), z(q.z), w(q.w)
	{}

	// construct a tsQuaternion from the form of angle and axis
	// the tsQuaternion represents a rotation around the axis by 
	// angle degree in counter-clockwise when looking at the opposite
	// direction of the axis, axis should be unit length
	tsQuaternion(const tsVector<T, 3> &axis, T deg)
	{
		// note: result tsQuaternion is already normalized

		T radian = tsMath::deg2rad(deg * (T)0.5);
		T sinValue = tsMath::sin(radian);

		T len = tsLen(axis);

		tsDebugAssert(len != 0);

		axis *= 1 / len;

		w = tsMath::cos(radian);
		x = axis.x * sinValue;
		y = axis.y * sinValue;
		z = axis.z * sinValue;
	}

	bool operator == (const tsQuaternion &q) const
	{
		return tsMath::abs(x - q.x) < (T)1e-5
			&& tsMath::abs(y - q.y) < (T)1e-5
			&& tsMath::abs(z - q.z) < (T)1e-5
			&& tsMath::abs(w - q.w) < (T)1e-5
	}

	void operator = (const tsQuaternion<T> &q)
	{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
	}

	void operator += (const tsQuaternion<T> &q)
	{
		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;
	}

	void operator /= (T d)
	{
		tsDebugAssert(d != 0);

		float tmp = 1 / d;
		x *= d;
		y *= d;
		z *= d;
		w *= d;
	}

	tsQuaternion<T> operator + (const tsQuaternion<T> &other) const
	{
		return tsQuaternion<T>(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	tsQuaternion<T> operator * (T val) const
	{
		return tsQuaternion<T>(x * val, y * val, z * val, w * val);
	}

	tsQuaternion<T> operator * (const tsQuaternion<T> &q) const 
	{
		// p * q = (p.w * q.v + q.w * p.v + p.v cross q.v, p.w * q.w - p.v dot q.v);
		return tsQuaternion<T>(w * q.x + x * q.w + y * q.z - z * q.y,
						  w * q.y + y * q.w + z * q.x - x * q.z,
	                      w * q.z + z * q.w + x * q.y - y * q.x,
						  w * q.w - x * q.x - y * q.y - z * q.z);
	}

	// negation operation
	tsQuaternion<T> operator -() const
	{
		return tsQuaternion<T>(-x, -y, -z, -w);
	}

	tsQuaternion conjugate()
	{
		return tsQuaternion(-x, -y, -z, w);
	}

	void setScalar(float ww)
	{
		w = ww;
	}

	void setVec3(const tsVector<T, 3> &v3)
	{
		x = v3[0];
		y = v3[1];
		z = v3[2];
	}

	tsVector<T, 3> vec3()
	{
		return tsVector<T, 3>(x, y, z);
	}

	// rotate a vector
	// tsQuaternion multiplication involves vector cross product,
	// so in different hand system, cross product should be implemented differently.
	tsVector<T, 3> rotateVec3(const tsVector<T, 3> &v3)
	{
		// faster tsQuaternion-vector multiplicatoin
		// http://molecularmusings.wordpress.com/2013/05/24/a-faster-tsQuaternion-vector-multiplication/
		tsVector<T, 3> v = vec3();
		tsVector<T, 3> tmp = 2 * tsCross(v, v3);
		return v3 + w * tmp + tsCross(v, tmp);
	}	

	T x, y, z, w;
};

template<typename T>
void tsNormalize(tsQuaternion<T> &q)
{
	float sqaureMag = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;

	tsDebugAssert(sqaureMag > (T)1e-6);

	float tmp = tsMath::invSqrt(sqaureMag);
	x *= tmp;
	y *= tmp;
	z *= tmp;
	w *= tmp;
}

// parameter tsQuaternions must be unit tsQuaternion!
// the return value is the cosine of the angle between two tsQuaternions on the greatest arc 
template<typename T>
T tsDot(const tsQuaternion<T> &q0, const tsQuaternion<T> &q1)
{
	return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

// http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
//         commutative    constant velocity    torque-minimal
// slerp       no                yes                yes
// nlerp       yes               no                 yes
// log lerp    yes               yes                no
// slerp is computationally expensive due to the operations of trigonometry
// argument Quaternions must be unit Quaternions!
template<typename T>
tsQuaternion<T> tsSlerp(T t, const tsQuaternion<T> &startq, const tsQuaternion<T> &endq)
{
	// cosine of angle between startq and endq
	T cosOfAngle = tsDot(startq, endq);

	// to correct from float point error
	cosOfAngle = cosOfAngle > 1 ? 1 : cosOfAngle;
	T angle = tsMath::acos(cosOfAngle);

	// sinAngle being positive ensures shortest arc
	T sinAngle = tsMath::sqrt(1 - cosOfAngle * cosOfAngle);
	if (sinAngle < 1e-6f)
	{
		return startq;
	}
	T oneOverSinAngle = 1 / sinAngle;

	T sinR = tsMath::sin((1 - t) * angle);
	T sinT = tsMath::sin(t * angle);

	tsQuaternion result = (sinR * oneOverSinAngle) * startq + (sinT * oneOverSinAngle) * endq;
	tsNormalize(result);

	return result;
}

template<typename T>
tsQuaternion<T> tsLerp(T t, const tsQuaternion<T> &startq, const tsQuaternion<T> &endq)
{
	tsQuaternion<T> res = startq * (1 - t) + endq * t;
	tsNormalize(res);
	return res;
}

#endif // SSS_QUATERNION_H