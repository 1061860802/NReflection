#pragma once
#pragma once
#include "Type.h"
#include "BaseNumeric.h"
#include "TypeManager.h"
#include "TypeFactory.h"


class ArrayBase : public Type
{
public:
	ArrayBase(const std::string& name, const std::string& realname, const type_info& type_info_,size_t size, size_t In_ArraySize,const Variable_Type_Container<Type> Inner) : Type(name, realname,type_info_, size), ArraySize(In_ArraySize), Inner(Inner) 
	{
	};
	const Variable_Type_Container<Type> Inner;
	const size_t ArraySize;
};

template<typename _InType_,size_t In_ArraySize>
class Array : public ArrayBase
{
public:
	Array(const std::string& name, size_t size,const Variable_Type_Container<Type> Inner) : ArrayBase(name,typeid(_InType_[In_ArraySize]).name(), typeid(_InType_[In_ArraySize]), size, In_ArraySize, Inner) 
	{
	};
	virtual bool BableToBinary() const override
	{
		if constexpr (std::is_trivially_copyable_v<_InType_>)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	virtual std::vector<uint8_t> ToBinary(const void* self) const override
	{
		if (BableToBinary())
		{
			std::vector<uint8_t> reuslt;
			reuslt.resize(size_ * In_ArraySize);
			memcpy(reuslt.data(), self, size_ * In_ArraySize);
			return reuslt;
		}
		return std::vector<uint8_t>();
	}
};

template<typename _InType_,size_t In_ArraySize>
struct TypeName<const _InType_[In_ArraySize]>
{
	inline static const auto Name = ("const_" + std::string(typeid(_InType_).name()) + "[" + std::to_string(In_ArraySize) + "]");
};

template<typename _InType_, size_t In_ArraySize>
struct TypeName<_InType_[In_ArraySize]>
{
	inline static const auto Name = (std::string(typeid(_InType_).name()) + "[" + std::to_string(In_ArraySize) + "]");
};

template<typename _InType_, size_t In_ArraySize>
struct GetTypeInfo<_InType_[In_ArraySize]>
{
	using Type_ = Array<_InType_,In_ArraySize>;
	static constexpr size_t Size = sizeof(_InType_[In_ArraySize]);
};

template<typename _InType_, size_t In_ArraySize>
class ArrayFactory
{
public:
	using Type_ = _InType_;
	static ArrayFactory& Instance()
	{
		static ArrayFactory inst;
		ArrayFactory* instPtr = &inst;
		return inst;
	}
	typename GetTypeInfo<_InType_[In_ArraySize]>::Type_& Info() { return *info_; }
private:
	typename GetTypeInfo<_InType_[In_ArraySize]>::Type_* info_;
	ArrayFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<GetTypeInfo<_InType_[In_ArraySize]>::Type_>(TypeName<Type_[In_ArraySize]>::Name, sizeof(_InType_[In_ArraySize]), GetType_container<_InType_>());
	}
};

template<typename _InType_,size_t In_ArraySize>
class FactoryGetter<_InType_[In_ArraySize]>
{
public:
	using GetFactory = ArrayFactory<_InType_,In_ArraySize>;
};
