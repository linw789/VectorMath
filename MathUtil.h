#ifndef SSS_MATH_H
#define SSS_MATH_H

#include <cmath>

// cannot use "Math" in global namespace
class tsMath
{
public:

	static float PI;
	static float HALF_PI;
	static double PI_d;
	static double HALF_PI_d;

	// convert degree to radian
	static float deg2rad(float deg)
	{
		return deg / 180 * PI;
	}

	// convert radian to degree
	static float rad2deg(float rad)
	{
		return rad / PI * 180;
	}

	// from Quake III source code
	// http://www.matrix67.com/data/InvSqrt.pdf
	static float invSqrt(float x)
	{
		float hx = x * 0.5f;
		int i = *(int*)&x; // get bits for floating VALUE
		i = 0x5f375a86 - (i >> 1); // gives initial guess y0
		x = *(float*)&i; // convert bits BACK to float
		x = x * (1.5f - hx * x * x); // Newton step, repeating increases accuracy
		x = x * (1.5f - hx * x * x);
		return x;
	}

	static float sqrt(const float x)
	{
		union
		{
			int i;
			float x;
		} u;
		u.x = x;
		u.i = /*(1<<29)*/0x20000000 + (u.i >> 1) - /*(1<<22)*/0x00400000; 
  
		// Two Babylonian Steps (simplified from:)
		// u.x = 0.5f * (u.x + x/u.x);
		// u.x = 0.5f * (u.x + x/u.x);
		u.x =       u.x + x/u.x;
		u.x = 0.25f*u.x + x/u.x;

		return u.x;
	} 

	static float pow(float x, float y)
	{
		return pow(x, y);
	}

	static float sin_deg(float x)
	{
		return sin(deg2rad(x));
	}

	static float cos_deg(float x)
	{
		return cos(deg2rad(x));
	}

	static float tan_deg(float x)
	{
		return tan(deg2rad(x));
	}

	static float sin(float x)
	{
		return sin(x);
	}

	static float cos(float x)
	{
		return cos(x);
	}

	static float acos(float x)
	{
		return acos(x);
	}

	static float abs(float x)
	{
		return abs(x);
	}

	static float max(float a, float b)
	{
		return a > b ? a : b;
	}

	static float min(float a, float b)
	{
		return a < b ? a : b;
	}

	static float clamp(float x, float min, float max)
	{
		if (x < min)
		{
			return min;
		}
		else if (x > max)
		{
			return max;
		}
		else
		{
			return x;
		}
	}
};

#endif // SSS_MATH_H