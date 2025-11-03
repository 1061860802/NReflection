#pragma once
#include "Type.h"
#include "BaseNumeric.h"
#include "Class.h"
#include "TypeManager.h"
#include "TypeFactory.h"


template<typename _InType_>
struct TTypeName<const _InType_*>
{
	inline static const auto Name = ("const_" + std::string(typeid(_InType_*).name()) + "*");
};

template<typename _InType_>
struct TTypeName<_InType_*>
{
	inline static const auto Name = (std::string(typeid(_InType_).name()) + "*");
};

class CPointerBase : public CType
{
public:
	CPointerBase(const std::string& name, const std::string& realname, const type_info& type_info_,size_t size, const TVariable_Type_Container<CType> Inner) : CType(name, realname,type_info_,size), Inner(Inner) {};
	const TVariable_Type_Container<CType> Inner;
	const TVariable_Type_Container<CType> GetInner() const
	{
		return Inner;
	}
	virtual bool InputRequire(bool SelfConst, const CType& other, bool OtherConst) const
	{
		if (!other.Useful())  return false;
		if (other == *this)
		{
			return true;
		}
		if (const CPointerBase* PtrBasePtr = dynamic_cast<const CPointerBase*>(&other))
		{
			const CPointerBase& RefBase = *PtrBasePtr;
			if (!Inner.bConst() && RefBase.Inner.bConst()) return false;
			if (Inner.GetTypeConst() == RefBase.Inner.GetTypeConst()) return true;
		}
		return false;
	}
	virtual bool ConstRequire() const { return true; };
};

template<typename _InType_>
class TPointer : public CPointerBase
{
public:
	TPointer(const std::string& name, size_t size, const TVariable_Type_Container<CType> Inner) : CPointerBase(name, typeid(_InType_*).name(), typeid(_InType_*), size, Inner) {};
	virtual void GCProcess(const void* OP_TargetObject, std::set<const void*>& OP_Inspecteds, std::set<IGCObjectInterface*>& OP_SomethingFound) const override
	{
		if (OP_Inspecteds.find(OP_TargetObject) != OP_Inspecteds.end()) return;
		CType::GCProcess(OP_TargetObject, OP_Inspecteds, OP_SomethingFound);
		void* voidPtr = const_cast<void*>(OP_TargetObject);
		const _InType_** PtrPtr = static_cast<const _InType_**>(voidPtr);
		const _InType_* TargetPtr = *PtrPtr;
		Inner.Type_.GCProcess(TargetPtr, OP_Inspecteds, OP_SomethingFound);
	}
};

template<typename _InType_>
struct TGetTypeInfo<_InType_*>
{
	using Type_ = TPointer<_InType_>;
	static constexpr size_t Size = sizeof(void*);
};

template<typename _InType_>
class PointerFactory
{
public:
	using Type_ = _InType_*;
	static PointerFactory& Instance()
	{
		static PointerFactory inst;
		PointerFactory* instPtr = &inst;
		return inst;
	}

	typename TGetTypeInfo<Type_>::Type_& Info() { return *info_; }
private:
	typename TGetTypeInfo<Type_>::Type_* info_;
	PointerFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<TGetTypeInfo<Type_>::Type_>(TTypeName<Type_>::Name, sizeof(void*), GetType_container<_InType_>());
	}
};

template<typename _InType_>
class TFactoryGetter<_InType_*>
{
public:
	using GetFactory = PointerFactory<_InType_>;
};
