#pragma once
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include "Variable_traits.h"
#include "GCSystemBase.h"
#include "NReflectionCoreAPI.h"

class NREFLECTIONCORE_API Type
{
public:
	virtual ~Type() = default;

	Type(const std::string& name, const std::string& realname, const type_info& type_info_, size_t size) : name_(name), realname_(realname), type_info_(type_info_), size_(size) { anno_ = false; };
	Type(const std::string& name, const std::string& realname, const type_info& type_info_, size_t size, bool anno) : name_(name), realname_(realname), type_info_(type_info_), size_(size), anno_(anno){};
	Type(const Type&) = delete;
	const std::string& GetTypeName() const { return name_; };
	const std::string& GetRealName() const { return realname_; };
	size_t GetTypeSize() const { return size_; };
	virtual bool bChangeable() const { return true; };
	virtual bool bUseful() const { return true; };
	virtual bool BableToBinary() const { return false; };
	virtual std::vector<uint8_t> ToBinary(const void* self) const { return std::vector<uint8_t>(); };
	virtual bool BableToString() const { return false; };
	virtual std::string ToString(const void* self) const { return std::string(); };
	virtual operator std::string() const
	{
		return name_;
	}
	virtual bool operator ==(const Type& other) const
	{
		return this == &other;
	}
	virtual bool operator >(const Type & other) const
	{
		return this > &other;
	}
	virtual bool operator <(const Type& other) const
	{
		return this < &other;
	}
	virtual bool InputRequire(bool SelfConst, const Type& other, bool OtherConst) const;
	virtual void GCProcess(const void* OP_TargetObject, std::set<const void*>& OP_Inspecteds, std::set<IGCObjectInterface*>& OP_SomethingFound) const 
	{ 
		if (OP_Inspecteds.find(OP_TargetObject) != OP_Inspecteds.end()) return;
		OP_Inspecteds.insert(OP_TargetObject); 
	};
protected:
	std::string name_;
	std::string realname_;
	const type_info& type_info_;
	size_t size_;
	bool anno_;
};

class NREFLECTIONCORE_API ValueIStream
{
public:
	virtual ~ValueIStream() = default;
	virtual bool SetValueFromString(void* ValuePtr, const std::string& valstring) const = 0;
};

inline bool operator!=(const Type& A, const Type& B)
{
	return !(A == B);
}

class NREFLECTIONCORE_API Void : public Type
{
public:
	Void() : Type(typeid(void).name(), typeid(void).name(),typeid(void), 0) {};
};

class NREFLECTIONCORE_API Numeric : public Type
{
public:
	Numeric(const std::string& name, const std::string& realname, const type_info& type_info_,size_t size) : Type(name, realname, type_info_,size) {};
	virtual bool BableToBinary() const override
	{
		return true;
	}
	virtual std::vector<uint8_t> ToBinary(const void* self) const override
	{
		std::vector<uint8_t> reuslt;
		reuslt.resize(size_);
		memcpy(reuslt.data(), self, size_);
		return reuslt;
	}
};

class NREFLECTIONCORE_API Function : public Type
{
public:
};

class NREFLECTIONCORE_API Enum : public Type
{
public:
};

class NREFLECTIONCORE_API UnknownBase : public Type
{
public:
	UnknownBase(const std::string& realname, size_t size) : Type("Unknow", realname,typeid(void), size) {};
	virtual bool operator ==(const Type& other)const override
	{
		return false;
	}
	virtual bool InputRequire(bool SelfConst, const Type& other, bool OtherConst) const
	{
		return false;
	}
	virtual bool bUseful() const override { return false; };
	virtual void AAA() = 0;
};

template<typename InType>
class Unknown : public UnknownBase
{
public:
	Unknown() : UnknownBase(typeid(InType).name(), sizeof(InType)) {};
	virtual bool operator ==(const Type& other)const override
	{
		return false;
	}
	virtual bool InputRequire(bool SelfConst, const Type& other, bool OtherConst) const
	{
		return false;
	}
	virtual bool bUseful() const override { return false; };
	virtual void AAA() override {};
};

template<typename InType>
class Variable_Type_Container
{
public:
	Variable_Type_Container(InType& InType,bool IsConst) :Type_(InType), IsConst(IsConst){};
	template<typename InTypei>
	Variable_Type_Container(const Variable_Type_Container<InTypei>& Type_container_) :Type_(Type_container_.Type_), IsConst(Type_container_.IsConst) 
	{
		static_assert(std::is_base_of_v<InType, InTypei>, "InType Is not base of Type");
	};
	InType& GetType() { return Type_; };
	const InType& GetTypeConst() const { return Type_; };
	const bool bConst() const { return IsConst; };
	operator std::string() const
	{
		if (IsConst)
		{
			return "const " + std::string(Type_);
		}
		return std::string(Type_);
	}
	template<typename OtherInType>
	bool InputRequire(const Variable_Type_Container<OtherInType>& Other) const
	{
		return Type_.InputRequire(IsConst, Other.Type_, Other.IsConst);
	}
	bool SetValueFromString(void* ValuePtr, const std::string& valstring) const
	{
		ValueIStream* IStream = dynamic_cast<ValueIStream*>(&Type_);
		if (IsConst)
		{
			return false;
		}
		if (IStream)
		{
			IStream->SetValueFromString(ValuePtr, valstring);
			return true;
		}
		return false;
	}
	InType& Type_;
	const bool IsConst;
};

struct NREFLECTIONCORE_API ClassMember
{
	virtual ~ClassMember() {};
};

template<typename T>
auto& GetType();

template<typename Type_T, typename = void>
struct GetTypeInfo;

template<typename Type_T, typename = void>
auto GetType_container()
{
	using ValueType_ = typename VReflectionTraits::remove_const<Type_T>::type;
	static constexpr bool IsConst = VReflectionTraits::remove_const<Type_T>::is_const;
	if constexpr (std::is_same_v<ValueType_, void>)
	{
		static constexpr size_t Size = 0;
	}
	else
	{
		static constexpr size_t Size = sizeof(Type_T);
	}
	auto& Type_ = GetType<ValueType_>();
	return Variable_Type_Container(Type_, IsConst);
};

template<typename Type_T, typename = void>
struct TypeName
{
	inline static const auto Name = typeid(Type_T).name();
};

template<typename Type_T, typename = void>
struct RealTypeName
{
	inline static const auto Name = typeid(Type_T).name();
};