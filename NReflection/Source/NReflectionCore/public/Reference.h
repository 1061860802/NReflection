#pragma once
#include "Type.h"
#include "TypeManager.h"
#include "TypeFactory.h"
#include "ReferenceBase.h"

template<typename _InType_>
class Reference : public ReferenceBase
{
public:
	Reference(const std::string& name, size_t size, const Variable_Type_Container<Type> Inner) : ReferenceBase(name, RealTypeName<_InType_&>::Name,typeid(_InType_&), size, Inner) {};
	virtual void GCProcess(const void* OP_TargetObject, std::set<const void*>& OP_Inspecteds, std::set<IGCObjectInterface*>& OP_SomethingFound) const override
	{
		if (OP_Inspecteds.find(OP_TargetObject) != OP_Inspecteds.end()) return;
		Type::GCProcess(OP_TargetObject, OP_Inspecteds, OP_SomethingFound);
		const _InType_* InnerPtr = static_cast<const _InType_*>(OP_TargetObject);
		Inner.Type_.GCProcess(InnerPtr, OP_Inspecteds, OP_SomethingFound);
	}
};

template<typename _InType_>
struct TypeName<const _InType_&>
{	
	inline static const auto Name = ("const_" + std::string(typeid(_InType_).name()) + "_Reference");
};

template<typename Type_T>
struct RealTypeName<const Type_T&>
{
	inline static const auto Name = ("const_" + std::string(typeid(Type_T&).name()) + "&");
};

template<typename _InType_>
struct TypeName<_InType_&>
{
	inline static const auto Name = (std::string(typeid(_InType_).name()) + "_Reference");
};

template<typename Type_T>
struct RealTypeName<Type_T&>
{
	inline static const auto Name = (std::string(typeid(Type_T&).name()) + "&");
};

template<typename _InType_>
struct GetTypeInfo<_InType_&>
{
	using Type_ = Reference<_InType_>;
	static constexpr size_t Size = 0;
};

template<typename _InType_>
class LReferenceFactory
{
public:
	using Type_ = _InType_&;
	static LReferenceFactory& Instance()
	{
		static LReferenceFactory inst;
		LReferenceFactory* instPtr = &inst;
		return inst;
	}

	typename GetTypeInfo<Type_>::Type_& Info() { return *info_; }
private:
	typename GetTypeInfo<Type_>::Type_* info_;
	LReferenceFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<GetTypeInfo<Type_>::Type_>(TypeName<Type_>::Name,0, GetType_container<_InType_>());
	}
};

template<typename _InType_>
class FactoryGetter<_InType_&>
{
public:
	using GetFactory = LReferenceFactory<_InType_>;
};

template<typename _InType_>
class RReference : public ReferenceBase
{
public:
	RReference(const std::string& name, size_t size, const Variable_Type_Container<Type> Inner) : ReferenceBase(name, RealTypeName<_InType_&&>::Name,typeid(_InType_&&), size, Inner) {};
	virtual void GCProcess(const void* OP_TargetObject, std::set<const void*>& OP_Inspecteds, std::set<IGCObjectInterface*>& OP_SomethingFound) const override
	{
		if (OP_Inspecteds.find(OP_TargetObject) != OP_Inspecteds.end()) return;
		Type::GCProcess(OP_TargetObject, OP_Inspecteds, OP_SomethingFound);
		const _InType_* InnerPtr = static_cast<const _InType_*>(OP_TargetObject);
		Inner.Type_.GCProcess(InnerPtr, OP_Inspecteds, OP_SomethingFound);
	}
};

template<typename _InType_>
struct TypeName<const _InType_&&>
{
	inline static const auto Name = ("const_" + std::string(typeid(_InType_).name()) + "_RReference");
};

template<typename Type_T>
struct RealTypeName<const Type_T&&>
{
	inline static const auto Name = ("const_" + std::string(typeid(Type_T&&).name()) + "&&");
};

template<typename _InType_>
struct TypeName<_InType_&&>
{
	inline static const auto Name = (std::string(typeid(_InType_).name()) + "_RReference");
};

template<typename Type_T>
struct RealTypeName<Type_T&&>
{
	inline static const auto Name = (std::string(typeid(Type_T&).name()) + "&&");
};

template<typename _InType_>
struct GetTypeInfo<_InType_&&>
{
	using Type_ = RReference<_InType_>;
	static constexpr size_t Size = 0;
};

template<typename _InType_>
class RReferenceFactory
{
public:
	using Type_ = _InType_&&;
	static RReferenceFactory& Instance()
	{
		static RReferenceFactory inst;
		RReferenceFactory* instPtr = &inst;
		return inst;
	}

	typename GetTypeInfo<Type_>::Type_& Info() { return *info_; }
private:
	typename GetTypeInfo<Type_>::Type_* info_;
	RReferenceFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<GetTypeInfo<Type_>::Type_>(TypeName<Type_>::Name, 0, GetType_container<_InType_>());
	}
};

template<typename _InType_>
class FactoryGetter<_InType_&&>
{
public:
	using GetFactory = RReferenceFactory<_InType_>;
};