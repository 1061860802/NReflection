#pragma once
#include "Type.h"
#include "TypeManager.h"
#include "TypeFactory.h"
#include "ReferenceBase.h"

namespace NLEngine
{

	template<typename _InType_>
	class TReference : public CReferenceBase
	{
	public:
		TReference(const std::string& name, size_t size, const TVariableTypeContainer<CType> Inner) : CReferenceBase(name, RealTypeName<_InType_&>::Name, typeid(_InType_&), size, Inner) {};
		virtual void GCProcess(const void* OP_TargetObject, std::set<const void*>& OP_Inspecteds, std::set<IGCObjectInterface*>& OP_SomethingFound) const override
		{
			if (OP_Inspecteds.find(OP_TargetObject) != OP_Inspecteds.end()) return;
			CType::GCProcess(OP_TargetObject, OP_Inspecteds, OP_SomethingFound);
			const _InType_* InnerPtr = static_cast<const _InType_*>(OP_TargetObject);
			Inner.Type_.GCProcess(InnerPtr, OP_Inspecteds, OP_SomethingFound);
		}
	};

	template<typename _InType_>
	struct TTypeName<const _InType_&>
	{
		inline static const auto Name = ("const_" + std::string(typeid(_InType_).name()) + "_Reference");
	};

	template<typename Type_T>
	struct RealTypeName<const Type_T&>
	{
		inline static const auto Name = ("const_" + std::string(typeid(Type_T&).name()) + "&");
	};

	template<typename _InType_>
	struct TTypeName<_InType_&>
	{
		inline static const auto Name = (std::string(typeid(_InType_).name()) + "_Reference");
	};

	template<typename Type_T>
	struct RealTypeName<Type_T&>
	{
		inline static const auto Name = (std::string(typeid(Type_T&).name()) + "&");
	};

	template<typename _InType_>
	struct TGetTypeInfo<_InType_&>
	{
		using Type_ = TReference<_InType_>;
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

		typename TGetTypeInfo<Type_>::Type_& Info() { return *info_; }
	private:
		typename TGetTypeInfo<Type_>::Type_* info_;
		LReferenceFactory()
		{
			info_ = GetTypeManager().GetOrRegistor<TGetTypeInfo<Type_>::Type_>(TTypeName<Type_>::Name, 0, GetType_container<_InType_>());
		}
	};

	template<typename _InType_>
	class TFactoryGetter<_InType_&>
	{
	public:
		using GetFactory = LReferenceFactory<_InType_>;
	};

	template<typename _InType_>
	class RReference : public CReferenceBase
	{
	public:
		RReference(const std::string& name, size_t size, const TVariableTypeContainer<CType> Inner) : CReferenceBase(name, RealTypeName<_InType_&&>::Name, typeid(_InType_&&), size, Inner) {};
		virtual void GCProcess(const void* OP_TargetObject, std::set<const void*>& OP_Inspecteds, std::set<IGCObjectInterface*>& OP_SomethingFound) const override
		{
			if (OP_Inspecteds.find(OP_TargetObject) != OP_Inspecteds.end()) return;
			CType::GCProcess(OP_TargetObject, OP_Inspecteds, OP_SomethingFound);
			const _InType_* InnerPtr = static_cast<const _InType_*>(OP_TargetObject);
			Inner.Type_.GCProcess(InnerPtr, OP_Inspecteds, OP_SomethingFound);
		}
	};

	template<typename _InType_>
	struct TTypeName<const _InType_&&>
	{
		inline static const auto Name = ("const_" + std::string(typeid(_InType_).name()) + "_RReference");
	};

	template<typename Type_T>
	struct RealTypeName<const Type_T&&>
	{
		inline static const auto Name = ("const_" + std::string(typeid(Type_T&&).name()) + "&&");
	};

	template<typename _InType_>
	struct TTypeName<_InType_&&>
	{
		inline static const auto Name = (std::string(typeid(_InType_).name()) + "_RReference");
	};

	template<typename Type_T>
	struct RealTypeName<Type_T&&>
	{
		inline static const auto Name = (std::string(typeid(Type_T&).name()) + "&&");
	};

	template<typename _InType_>
	struct TGetTypeInfo<_InType_&&>
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

		typename TGetTypeInfo<Type_>::Type_& Info() { return *info_; }
	private:
		typename TGetTypeInfo<Type_>::Type_* info_;
		RReferenceFactory()
		{
			info_ = GetTypeManager().GetOrRegistor<TGetTypeInfo<Type_>::Type_>(TTypeName<Type_>::Name, 0, GetType_container<_InType_>());
		}
	};

	template<typename _InType_>
	class TFactoryGetter<_InType_&&>
	{
	public:
		using GetFactory = RReferenceFactory<_InType_>;
	};

}