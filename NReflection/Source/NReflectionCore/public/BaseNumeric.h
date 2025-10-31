#pragma once
#include "Type.h"
#include "TypeFactory.h"

template<typename Type_T>
class NumericFactory
{
public:
	using Type_ = typename Type_T;
	static NumericFactory& Instance()
	{
		static NumericFactory inst;
		NumericFactory* instPtr = &inst;
		return inst;
	}

	typename GetTypeInfo<Type_T>::Type_& Info() { return *info_; }
private:
	typename GetTypeInfo<Type_T>::Type_* info_;
	NumericFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<GetTypeInfo<Type_T>::Type_>();
	}
};

template<>
struct TypeName<float>
{
	static constexpr char Name[] = "float";
	inline static const auto RealName = typeid(float).name();
};

class Float : public Numeric , public ValueIStream
{
public:
	using Type_ = float;
	Float() : Numeric(TypeName<Type_>::Name, TypeName<Type_>::RealName, typeid(Type_), sizeof(Type_)) {};
	virtual bool SetValueFromString(void* ValuePtr, const std::string& valstring) const override
	{
		float& Ptr = *(float*)ValuePtr;
		Ptr = std::stof(valstring);
		return true;
	}
};

template<>
struct GetTypeInfo<float>
{
	using Type_ = Float;
	static constexpr size_t Size = sizeof(float);
};

template<>
class FactoryGetter<float>
{
public:
	using GetFactory = NumericFactory<float>;
};

template<>
struct TypeName<double>
{
	static constexpr char Name[] = "double";
	inline static const auto RealName = typeid(double).name();
};


class Double : public Numeric, public ValueIStream
{
public:
	using Type_ = double;
	Double() : Numeric(TypeName<Type_>::Name, TypeName<Type_>::RealName, typeid(Type_), sizeof(Type_)) {};
	virtual bool SetValueFromString(void* ValuePtr, const std::string& valstring) const override
	{
		double& Ptr = *(double*)ValuePtr;
		Ptr = std::stod(valstring);
		return true;
	}
};

template<>
struct GetTypeInfo<double>
{
	using Type_ = Double;
	static constexpr size_t Size = sizeof(double);
};

template<>
class FactoryGetter<double>
{
public:
	using GetFactory = NumericFactory<double>;
};

template<>
struct TypeName<int8_t>
{
	static constexpr char Name[] = "int8";
	inline static const auto RealName = typeid(int8_t).name();
};

class Int8 : public Numeric
{
public:
	using Type_ = int8_t;
	Int8() : Numeric(TypeName<Type_>::Name, TypeName<Type_>::RealName, typeid(Type_),sizeof(Type_)) {};
};

template<>
struct GetTypeInfo<int8_t>
{
	using Type_ = Int8;
	static constexpr size_t Size = sizeof(int8_t);
};

template<>
class FactoryGetter<int8_t>
{
public:
	using GetFactory = NumericFactory<int8_t>;
};

template<>
struct TypeName<uint8_t>
{
	static constexpr char Name[] = "uint8";
	inline static const auto RealName = typeid(uint8_t).name();
};


class UInt8 : public Numeric
{
public:
	using Type_ = uint8_t;
	UInt8() : Numeric(TypeName<Type_>::Name, TypeName<Type_>::RealName, typeid(Type_), sizeof(Type_)) {};
};

template<>
struct GetTypeInfo<uint8_t>
{
	using Type_ = UInt8;
	static constexpr size_t Size = sizeof(uint8_t);
};

template<>
class FactoryGetter<uint8_t>
{
public:
	using GetFactory = NumericFactory<uint8_t>;
};

template<>
struct TypeName<int16_t>
{
	static constexpr char Name[] = "int16";
	inline static const auto RealName = typeid(int16_t).name();
};


class Int16 : public Numeric
{
public:
	using Type_ = int16_t;
	Int16() : Numeric(TypeName<Type_>::Name, TypeName<Type_>::RealName,typeid(Type_), sizeof(Type_)) {};
};

template<>
struct GetTypeInfo<int16_t>
{
	using Type_ = Int16;
	static constexpr size_t Size = sizeof(int16_t);
};

template<>
class FactoryGetter<int16_t>
{
public:
	using GetFactory = NumericFactory<int16_t>;
};

template<>
struct TypeName<uint16_t>
{
	static constexpr char Name[] = "uint16";
	inline static const auto RealName = typeid(uint16_t).name();
};

class UInt16 : public Numeric
{
public:
	using Type_ = uint16_t;
	UInt16() : Numeric(TypeName<Type_>::Name, TypeName<Type_>::RealName, typeid(Type_),sizeof(Type_)) {};
};

template<>
struct GetTypeInfo<uint16_t>
{
	using Type_ = UInt16;
	static constexpr size_t Size = sizeof(uint16_t);
};

template<>
class FactoryGetter<uint16_t>
{
public:
	using GetFactory = NumericFactory<uint16_t>;
};

template<>
struct TypeName<int32_t>
{
	static constexpr char Name[] = "int32";
	inline static const auto RealName = typeid(int32_t).name();
};

class Int32 : public Numeric
{
public:
	using Type_ = int32_t;
	Int32() : Numeric(TypeName<Type_>::Name, TypeName<Type_>::RealName, typeid(Type_),sizeof(Type_)) {};
};

template<>
struct GetTypeInfo<int32_t>
{
	using Type_ = Int32;
	static constexpr size_t Size = sizeof(int32_t);
};

template<>
class FactoryGetter<int32_t>
{
public:
	using GetFactory = NumericFactory<int32_t>;
};

template<>
struct TypeName<uint32_t>
{
	static constexpr char Name[] = "uint32";
	inline static const auto RealName = typeid(uint32_t).name();
};

class UInt32 : public Numeric
{
public:
	using Type_ = uint32_t;
	UInt32() : Numeric(TypeName<Type_>::Name, TypeName<Type_>::RealName, typeid(Type_), sizeof(Type_)) {};
};

template<>
struct GetTypeInfo<uint32_t>
{
	using Type_ = UInt32;
	static constexpr size_t Size = sizeof(uint32_t);
};

template<>
class FactoryGetter<uint32_t>
{
public:
	using GetFactory = NumericFactory<uint32_t>;
};

template<>
struct TypeName<int64_t>
{
	static constexpr char Name[] = "int64";
	inline static const auto RealName = typeid(int64_t).name();
};

class Int64 : public Numeric
{
public:
	using Type_ = int64_t;
	Int64() : Numeric(TypeName<Type_>::Name, TypeName<Type_>::RealName, typeid(Type_), sizeof(Type_)) {};
};

template<>
struct GetTypeInfo<int64_t>
{
	using Type_ = Int64;
	static constexpr size_t Size = sizeof(int64_t);
};

template<>
class FactoryGetter<int64_t>
{
public:
	using GetFactory = NumericFactory<int64_t>;
};

template<>
struct TypeName<uint64_t>
{
	static constexpr char Name[] = "uint64";
	inline static const auto RealName = typeid(uint64_t).name();
};

class UInt64 : public Numeric
{
public:
	using Type_ = uint64_t;
	UInt64() : Numeric(TypeName<Type_>::Name, TypeName<Type_>::RealName, typeid(Type_), sizeof(Type_)) {};
};

template<>
struct GetTypeInfo<uint64_t>
{
	using Type_ = UInt64;
	static constexpr size_t Size = sizeof(uint64_t);
};

template<>
class FactoryGetter<uint64_t>
{
public:
	using GetFactory = NumericFactory<uint64_t>;
};
