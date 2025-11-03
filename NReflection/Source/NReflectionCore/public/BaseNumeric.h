#pragma once
#include "Type.h"
#include "TypeFactory.h"

template<typename Type_T>
class TNumericFactory
{
public:
	using Type_ = typename Type_T;
	static TNumericFactory& Instance()
	{
		static TNumericFactory inst;
		TNumericFactory* instPtr = &inst;
		return inst;
	}

	typename TGetTypeInfo<Type_T>::Type_& Info() { return *info_; }
private:
	typename TGetTypeInfo<Type_T>::Type_* info_;
	TNumericFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<TGetTypeInfo<Type_T>::Type_>();
	}
};

template<>
struct TTypeName<float>
{
	static constexpr char Name[] = "float";
	inline static const auto RealName = typeid(float).name();
};

class CFloat : public CNumeric , public ValueIStream
{
public:
	using Type_ = float;
	CFloat() : CNumeric(TTypeName<Type_>::Name, TTypeName<Type_>::RealName, typeid(Type_), sizeof(Type_)) {};
	virtual bool SetValueFromString(void* ValuePtr, const std::string& valstring) const override
	{
		float& Ptr = *(float*)ValuePtr;
		Ptr = std::stof(valstring);
		return true;
	}
};

template<>
struct TGetTypeInfo<float>
{
	using Type_ = CFloat;
	static constexpr size_t Size = sizeof(float);
};

template<>
class TFactoryGetter<float>
{
public:
	using GetFactory = TNumericFactory<float>;
};

template<>
struct TTypeName<double>
{
	static constexpr char Name[] = "double";
	inline static const auto RealName = typeid(double).name();
};


class CDouble : public CNumeric, public ValueIStream
{
public:
	using Type_ = double;
	CDouble() : CNumeric(TTypeName<Type_>::Name, TTypeName<Type_>::RealName, typeid(Type_), sizeof(Type_)) {};
	virtual bool SetValueFromString(void* ValuePtr, const std::string& valstring) const override
	{
		double& Ptr = *(double*)ValuePtr;
		Ptr = std::stod(valstring);
		return true;
	}
};

template<>
struct TGetTypeInfo<double>
{
	using Type_ = CDouble;
	static constexpr size_t Size = sizeof(double);
};

template<>
class TFactoryGetter<double>
{
public:
	using GetFactory = TNumericFactory<double>;
};

template<>
struct TTypeName<int8_t>
{
	static constexpr char Name[] = "int8";
	inline static const auto RealName = typeid(int8_t).name();
};

class CInt8 : public CNumeric
{
public:
	using Type_ = int8_t;
	CInt8() : CNumeric(TTypeName<Type_>::Name, TTypeName<Type_>::RealName, typeid(Type_),sizeof(Type_)) {};
};

template<>
struct TGetTypeInfo<int8_t>
{
	using Type_ = CInt8;
	static constexpr size_t Size = sizeof(int8_t);
};

template<>
class TFactoryGetter<int8_t>
{
public:
	using GetFactory = TNumericFactory<int8_t>;
};

template<>
struct TTypeName<uint8_t>
{
	static constexpr char Name[] = "uint8";
	inline static const auto RealName = typeid(uint8_t).name();
};


class CUInt8 : public CNumeric
{
public:
	using Type_ = uint8_t;
	CUInt8() : CNumeric(TTypeName<Type_>::Name, TTypeName<Type_>::RealName, typeid(Type_), sizeof(Type_)) {};
};

template<>
struct TGetTypeInfo<uint8_t>
{
	using Type_ = CUInt8;
	static constexpr size_t Size = sizeof(uint8_t);
};

template<>
class TFactoryGetter<uint8_t>
{
public:
	using GetFactory = TNumericFactory<uint8_t>;
};

template<>
struct TTypeName<int16_t>
{
	static constexpr char Name[] = "int16";
	inline static const auto RealName = typeid(int16_t).name();
};


class CInt16 : public CNumeric
{
public:
	using Type_ = int16_t;
	CInt16() : CNumeric(TTypeName<Type_>::Name, TTypeName<Type_>::RealName,typeid(Type_), sizeof(Type_)) {};
};

template<>
struct TGetTypeInfo<int16_t>
{
	using Type_ = CInt16;
	static constexpr size_t Size = sizeof(int16_t);
};

template<>
class TFactoryGetter<int16_t>
{
public:
	using GetFactory = TNumericFactory<int16_t>;
};

template<>
struct TTypeName<uint16_t>
{
	static constexpr char Name[] = "uint16";
	inline static const auto RealName = typeid(uint16_t).name();
};

class CUInt16 : public CNumeric
{
public:
	using Type_ = uint16_t;
	CUInt16() : CNumeric(TTypeName<Type_>::Name, TTypeName<Type_>::RealName, typeid(Type_),sizeof(Type_)) {};
};

template<>
struct TGetTypeInfo<uint16_t>
{
	using Type_ = CUInt16;
	static constexpr size_t Size = sizeof(uint16_t);
};

template<>
class TFactoryGetter<uint16_t>
{
public:
	using GetFactory = TNumericFactory<uint16_t>;
};

template<>
struct TTypeName<int32_t>
{
	static constexpr char Name[] = "int32";
	inline static const auto RealName = typeid(int32_t).name();
};

class CInt32 : public CNumeric
{
public:
	using Type_ = int32_t;
	CInt32() : CNumeric(TTypeName<Type_>::Name, TTypeName<Type_>::RealName, typeid(Type_),sizeof(Type_)) {};
};

template<>
struct TGetTypeInfo<int32_t>
{
	using Type_ = CInt32;
	static constexpr size_t Size = sizeof(int32_t);
};

template<>
class TFactoryGetter<int32_t>
{
public:
	using GetFactory = TNumericFactory<int32_t>;
};

template<>
struct TTypeName<uint32_t>
{
	static constexpr char Name[] = "uint32";
	inline static const auto RealName = typeid(uint32_t).name();
};

class CUInt32 : public CNumeric
{
public:
	using Type_ = uint32_t;
	CUInt32() : CNumeric(TTypeName<Type_>::Name, TTypeName<Type_>::RealName, typeid(Type_), sizeof(Type_)) {};
};

template<>
struct TGetTypeInfo<uint32_t>
{
	using Type_ = CUInt32;
	static constexpr size_t Size = sizeof(uint32_t);
};

template<>
class TFactoryGetter<uint32_t>
{
public:
	using GetFactory = TNumericFactory<uint32_t>;
};

template<>
struct TTypeName<int64_t>
{
	static constexpr char Name[] = "int64";
	inline static const auto RealName = typeid(int64_t).name();
};

class CInt64 : public CNumeric
{
public:
	using Type_ = int64_t;
	CInt64() : CNumeric(TTypeName<Type_>::Name, TTypeName<Type_>::RealName, typeid(Type_), sizeof(Type_)) {};
};

template<>
struct TGetTypeInfo<int64_t>
{
	using Type_ = CInt64;
	static constexpr size_t Size = sizeof(int64_t);
};

template<>
class TFactoryGetter<int64_t>
{
public:
	using GetFactory = TNumericFactory<int64_t>;
};

template<>
struct TTypeName<uint64_t>
{
	static constexpr char Name[] = "uint64";
	inline static const auto RealName = typeid(uint64_t).name();
};

class CUInt64 : public CNumeric
{
public:
	using Type_ = uint64_t;
	CUInt64() : CNumeric(TTypeName<Type_>::Name, TTypeName<Type_>::RealName, typeid(Type_), sizeof(Type_)) {};
};

template<>
struct TGetTypeInfo<uint64_t>
{
	using Type_ = CUInt64;
	static constexpr size_t Size = sizeof(uint64_t);
};

template<>
class TFactoryGetter<uint64_t>
{
public:
	using GetFactory = TNumericFactory<uint64_t>;
};
