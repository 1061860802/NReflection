#pragma once
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include "Variable_traits.h"
#include "GCSystemBase.h"
#include "NReflectionCoreAPI.h"

class NREFLECTIONCORE_API CType
{
public:
	virtual ~CType() = default;

	CType(const std::string& name, const std::string& realname, const type_info& type_info_, size_t size) : name_(name), realname_(realname), type_info_(type_info_), size_(size) { anno_ = false; };
	CType(const std::string& name, const std::string& realname, const type_info& type_info_, size_t size, bool anno) : name_(name), realname_(realname), type_info_(type_info_), size_(size), anno_(anno){};
	CType(const CType&) = delete;
	const std::string& GetTypeName() const { return name_; };
	const std::string& GetRealName() const { return realname_; };
	size_t GetTypeSize() const { return size_; };
	virtual bool Changeable() const { return true; };
	virtual bool Useful() const { return true; };
	virtual bool BableToBinary() const { return false; };
	virtual std::vector<uint8_t> ToBinary(const void* self) const { return std::vector<uint8_t>(); };
	virtual bool BableToString() const { return false; };
	virtual std::string ToString(const void* self) const { return std::string(); };
	virtual operator std::string() const
	{
		return name_;
	}
	virtual bool operator ==(const CType& other) const
	{
		return this == &other;
	}
	virtual bool operator >(const CType & other) const
	{
		return this > &other;
	}
	virtual bool operator <(const CType& other) const
	{
		return this < &other;
	}
	virtual bool InputRequire(bool SelfConst, const CType& other, bool OtherConst) const;
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

inline bool operator!=(const CType& A, const CType& B)
{
	return !(A == B);
}

class NREFLECTIONCORE_API Void : public CType
{
public:
	Void() : CType(typeid(void).name(), typeid(void).name(),typeid(void), 0) {};
};

class NREFLECTIONCORE_API CNumeric : public CType
{
public:
	CNumeric(const std::string& name, const std::string& realname, const type_info& type_info_,size_t size) : CType(name, realname, type_info_,size) {};
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

class NREFLECTIONCORE_API CFunction : public CType
{
public:
};

class NREFLECTIONCORE_API CEnum : public CType
{
public:
};

class NREFLECTIONCORE_API CUnknownBase : public CType
{
public:
	CUnknownBase(const std::string& realname, size_t size) : CType("Unknow", realname,typeid(void), size) {};
	virtual bool operator ==(const CType& other)const override
	{
		return false;
	}
	virtual bool InputRequire(bool SelfConst, const CType& other, bool OtherConst) const
	{
		return false;
	}
	virtual bool Useful() const override { return false; };
	virtual void AAA() = 0;
};

template<typename InType>
class CUnknown : public CUnknownBase
{
public:
	CUnknown() : CUnknownBase(typeid(InType).name(), sizeof(InType)) {};
	virtual bool operator ==(const CType& other)const override
	{
		return false;
	}
	virtual bool InputRequire(bool SelfConst, const CType& other, bool OtherConst) const
	{
		return false;
	}
	virtual bool Useful() const override { return false; };
	virtual void AAA() override {};
};

template<typename InType>
class TVariable_Type_Container
{
public:
	TVariable_Type_Container(InType& InType,bool IsConst) :Type_(InType), IsConst(IsConst){};
	template<typename InTypei>
	TVariable_Type_Container(const TVariable_Type_Container<InTypei>& Type_container_) :Type_(Type_container_.Type_), IsConst(Type_container_.IsConst) 
	{
		static_assert(std::is_base_of_v<InType, InTypei>, "InType Is not base of CType");
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
	bool InputRequire(const TVariable_Type_Container<OtherInType>& Other) const
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

struct NREFLECTIONCORE_API CClassMember
{
	virtual ~CClassMember() {};
};

template<typename T>
auto& GetType();

template<typename Type_T, typename = void>
struct TGetTypeInfo;

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
	return TVariable_Type_Container(Type_, IsConst);
};

template<typename Type_T, typename = void>
struct TTypeName
{
	inline static const auto Name = typeid(Type_T).name();
};

template<typename Type_T, typename = void>
struct RealTypeName
{
	inline static const auto Name = typeid(Type_T).name();
};