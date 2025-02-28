#pragma once
#include <math.h>


constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template <typename T>
constexpr auto sq(const T& x) noexcept
{
	return x * x;
}

template<typename T>
T wrap_angle(T theta) noexcept
{
	const T modded =  (T)fmod(theta, (T)2.0 * (T)PI_D);
	if (modded > (T)PI_D) return (modded - (T)2.0 * (T)PI_D);
	else if (modded < -(T)PI_D) return (modded + (T)2.0 * (T)PI_D);
	return modded;
}

template<typename T>
constexpr T interpolate(const T& src, const T& dst, float alpha) noexcept
{
	return src + (dst - src) * alpha;
}

template<typename T>
constexpr T to_rad(T deg) noexcept
{
	return deg * PI / (T)180.0;
}

template<typename T>
constexpr T gauss(T x, T sigma) noexcept
{
	const auto ss = sq(sigma);
	return ((T)1.0 / sqrt((T)2.0 * (T)PI_D * ss)) * exp(-sq(x) / ((T)2.0 * ss));
}