#pragma once
#ifndef _FIX_FLOAT_H
#define _FIX_FLOAT_H

//
//  FixFloat.h
//
//  Copyright (c) 2023 misakichi kaminagare.
//  https://github.com/misakichi/mml2WaveRetro
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file "LICENSE" or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//

/// <summary>
/// Fixed float point value.
/// </summary>
/// <typeparam name="BASETYPE">In integer type.</typeparam>
/// <typeparam name="FloatPrecision">Precision of float.</typeparam>
template<typename BASETYPE=__int64, unsigned FLOATPRECISION = 16> 
class CFixFloat {
public:
	using BaseType = BASETYPE;
	using ThisType = CFixFloat<BASETYPE, FLOATPRECISION>;
	static constexpr unsigned FloatPrecision = FLOATPRECISION;
	static constexpr unsigned PrecisionMul = 1 << FloatPrecision;
	static_assert(BaseType(-1) < 0);

	constexpr CFixFloat()
		:value_(0)
	{
	}

	inline constexpr CFixFloat(const ThisType& v)
	{
		value_ = v.value_;
	}

	inline constexpr CFixFloat(float v)
	{
		value_ = BaseType(double(v) * PrecisionMul);
	}

	inline constexpr CFixFloat(double v)
	{
		value_ = BaseType(v * PrecisionMul);
	}

	template<typename T>
	inline constexpr CFixFloat(T v)
	{
		value_ = BaseType(v) << FloatPrecision;
	}

	operator float() const
	{
		return float(double(value_) / PrecisionMul);
	}
	operator double() const
	{
		return double(value_) / PrecisionMul;
	}

	template<typename T>
	operator T() const
	{
		return T(value_ >> FloatPrecision);
	}

	//operator =
	ThisType& operator = (const ThisType& r)
	{
		value_ = r.value_;
		return *this;
	}

	//operator +=
	ThisType& operator += (const ThisType& r)
	{
		value_ += r.value_;
		return *this;
	}
	template<typename T>
	ThisType& operator += (const T& r)
	{
		*this += ThisType(r);
		return *this;
	}

	//operator -=
	ThisType& operator -= (const ThisType& r)
	{
		value_ -= r.value_;
		return *this;
	}
	template<typename T>
	ThisType& operator -= (const T& r)
	{
		*this -= ThisType(r);
		return *this;
	}

	//operator *=
	ThisType& operator *= (const ThisType& r)
	{
		value_ = BaseType((int64_t(value_) * r.value_) >> FloatPrecision);
		return *this;
	}
	ThisType& operator *= (int r)
	{
		value_ *= r;
		return *this;
	}
	template<typename T>
	ThisType& operator *= (const T& r)
	{
		*this *= ThisType(r);
		return *this;
	}


	//operator /=
	ThisType& operator /= (const ThisType& r)
	{
		value_ = BaseType((int64_t(value_)<< FloatPrecision) / r.value_);
		return *this;
	}
	ThisType& operator /= (int r)
	{
		value_ /= r;
		return *this;
	}
	template<typename T>
	ThisType& operator /= (const T& r)
	{
		*this /= ThisType(r);
		return *this;
	}
	//operator <O>
	template<typename T>
	ThisType operator + (const T& r) const
	{
		ThisType tmp(*this);
		tmp += r;
		return tmp;
	}
	template<typename T>
	ThisType operator - (const T& r) const
	{
		ThisType tmp(*this);
		tmp -= r;
		return tmp;
	}
	template<typename T>
	ThisType operator * (const T& r) const
	{
		ThisType tmp(*this);
		tmp *= r;
		return tmp;
	}
	template<typename T>
	ThisType operator / (const T& r) const
	{
		ThisType tmp(*this);
		tmp /= r;
		return tmp;
	}
	ThisType operator %= (const ThisType& r) {
		value_ %= r.value_;
		return *this; 
	}
	template<typename T>
	ThisType operator % (const T& r) const
	{
		ThisType tmp(*this);
		tmp %= r;
		return tmp;
	}
	bool operator < (const ThisType& r) const { return value_ < r.value_; }
	bool operator > (const ThisType& r) const { return value_ > r.value_; }
	bool operator <= (const ThisType& r) const { return value_ <= r.value_; }
	bool operator >= (const ThisType& r) const { return value_ >= r.value_; }
	bool operator == (const ThisType& r) const { return value_ == r.value_; }
	bool operator != (const ThisType& r) const { return value_ != r.value_; }
	
	template<typename T> bool operator < (const T& r) const { return value_ < ThisType(r).value_; }
	template<typename T> bool operator > (const T& r) const { return value_ > ThisType(r).value_; }
	template<typename T> bool operator <= (const T& r) const { return value_ <= ThisType(r).value_; }
	template<typename T> bool operator >= (const T& r) const { return value_ >= ThisType(r).value_; }
	template<typename T> bool operator == (const T& r) const { return value_ == ThisType(r).value_; }
	template<typename T> bool operator != (const T& r) const { return value_ != ThisType(r).value_; }

	ThisType operator - () const { return *this * -1; }

private:
	BaseType value_;

};



#endif