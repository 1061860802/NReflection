#pragma once
#include "Type.h"
#include "ClassBase.h"
#include "MemberVariable.h"
#include "StaticVariable.h"
#include "MemberFunction.h"
#include "StaticFunction.h"
#include "Pointer.h"
#include "Reference.h"
#include "Array.h"
#include <vector>

template<typename Class_T, typename... Parents> 
struct NLEG_ClassParentsHelper;

template<typename Class_T>
struct NLEG_ClassParentsHelper_CT;

template<typename Class_T>
struct NLEG_ClassParentsHelper_CT
{
	using CType = NLEG_ClassParentsHelper<Class_T>;
};

template<typename Class_T>
using NLEG_ClassParentsHelper_T = typename NLEG_ClassParentsHelper_CT<Class_T>::CType;

template<typename Class_T, typename Parent> 
struct NLEG_ClassParentsHelper<Class_T, Parent>
{ 
	using ParentsT = std::tuple<NLEG_ClassParentsHelper_T<Parent>>;
	using SelfT = Class_T; 
	template<typename OutClass_T, size_t I = 0>
	static void ParentClassesGenHelper(std::map<const NLEngine::CClassBase*,const NLEngine::CParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			NLEngine::CClassBase* TargetClass = static_cast<NLEngine::CClassBase*>(NLEngine::GetStaticType<ParentT::SelfT>());
			ParentClasses.insert(std::pair(TargetClass, new NLEngine::TParentClass<OutClass_T, ParentT::SelfT>(TargetClass)));
			ParentT::ParentClassesGenHelper<OutClass_T>(ParentClasses);
			ParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
	template<typename OutClass_T, size_t I = 0>
	static void DirectParentClassesGenHelper(std::vector<const NLEngine::CParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			NLEngine::CClassBase* TargetClass = static_cast<NLEngine::CClassBase*>(NLEngine::GetStaticType<ParentT::SelfT>());
			ParentClasses.push_back(new NLEngine::TParentClass<OutClass_T, ParentT::SelfT>(TargetClass));
			DirectParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
};

template<typename Class_T>
struct NLEG_ClassParentsHelper<Class_T>
{
	using ParentsT = std::tuple<>;
	using SelfT = Class_T;
	template<typename OutClass_T, size_t I = 0>
	static void ParentClassesGenHelper(std::map<const NLEngine::CClassBase*,const NLEngine::CParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			NLEngine::CClassBase* TargetClass = static_cast<NLEngine::CClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.insert(std::pair(TargetClass, new NLEngine::TParentClass<OutClass_T, ParentT::SelfT>(TargetClass)));
			ParentT::ParentClassesGenHelper<OutClass_T>(ParentClasses);
			ParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
	template<typename OutClass_T, size_t I = 0>
	static void DirectParentClassesGenHelper(std::vector<const NLEngine::CParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			NLEngine::CClassBase* TargetClass = static_cast<CClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.push_back(new NLEngine::TParentClass<OutClass_T, ParentT::SelfT>(TargetClass));
			DirectParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
};

template<typename Class_T, typename FirstParent, typename... RestParents> 
struct NLEG_ClassParentsHelper<Class_T, FirstParent, RestParents...>
{
	using ParentsT = typename decltype(std::tuple_cat(std::tuple<NLEG_ClassParentsHelper_T<FirstParent>>(), typename NLEG_ClassParentsHelper<Class_T, RestParents...>::ParentsT()));
	using SelfT = typename Class_T;

	template<typename OutClass_T, size_t I = 0>
	static void ParentClassesGenHelper(std::map<const NLEngine::CClassBase*,const NLEngine::CParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			NLEngine::CClassBase* TargetClass = static_cast<NLEngine::CClassBase*>(NLEngine::GetStaticType<ParentT::SelfT>());
			ParentClasses.insert(std::pair(TargetClass, new NLEngine::TParentClass<OutClass_T, ParentT::SelfT>(TargetClass)));
			ParentT::ParentClassesGenHelper<OutClass_T>(ParentClasses);
			ParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
	template<typename OutClass_T, size_t I = 0>
	static void DirectParentClassesGenHelper(std::vector<const NLEngine::CParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			NLEngine::CClassBase* TargetClass = static_cast<NLEngine::CClassBase*>(NLEngine::GetStaticType<ParentT::SelfT>());
			ParentClasses.push_back(new NLEngine::TParentClass<OutClass_T, ParentT::SelfT>(TargetClass));
			DirectParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
};

template<typename Class_T,typename... Parents>
struct NLEG_ParentTestHelper;

template<typename Class_T, typename Parent,typename... Parents>
struct NLEG_ParentTestHelper<Class_T, Parent, Parents...>
{
	static constexpr bool Success = std::is_base_of_v<Parent, Class_T>&& NLEG_ParentTestHelper<Class_T,Parents...>::Success;
};

template<typename Class_T, typename Parent>
struct NLEG_ParentTestHelper<Class_T, Parent>
{
	static constexpr bool Success = std::is_base_of_v<Parent, Class_T>;
};

#define PARENTS(InClass,...) \
static_assert(bool(NLEG_ParentTestHelper<InClass,__VA_ARGS__>::Success),"Please check input types");\
template<>\
struct NLEG_ClassParentsHelper_CT<InClass>\
{\
	using CType = NLEG_ClassParentsHelper<InClass,__VA_ARGS__>;\
};

namespace NLEngine
{
	template<typename T>
	class TClassInfo : public CClassInfoBase
	{
	};

	template<typename TClassT>
	struct PostConstructorProcessUtility
	{
		PostConstructorProcessUtility() = delete;
		static void PostConstructorProcess(TClassT* IN_Class, TClassInfo<typename TClassT::ClassT_>* IN_ClassInfo)
		{
		}
	};

	template<typename T>
	class TClass final : public CClassBase
	{
	public:
		using ClassT_ = typename T;
		TClass() :CClassBase(TTypeName<ClassT_>::Name, typeid(T).name(), typeid(T), sizeof(ClassT_))
		{
			TClassInfo<ClassT_>* NewInfo = new TClassInfo<ClassT_>;
			SetClassInfo(NewInfo);
			NLEG_ClassParentsHelper_T<ClassT_>::ParentClassesGenHelper<ClassT_>(Info->ParentClasses_);
			NLEG_ClassParentsHelper_T<ClassT_>::DirectParentClassesGenHelper<ClassT_>(Info->DirectParentClasses_);
		}
		void PostConstructorProcess()
		{
			if (BuildCalled == false)
			{
				BuildCalled = true;
				TClassInfo<ClassT_>* NewInfo = static_cast<TClassInfo<ClassT_>*>(Info);
				PostConstructorProcessUtility<TClass<ClassT_>>::PostConstructorProcess(this, NewInfo);
				Info->PostConstructorProcess();
			}
		}
	public:
		bool BuildCalled = false;
		virtual bool BableToBinary() const override
		{
			if constexpr (std::is_trivially_copyable_v<T>)
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
				reuslt.resize(size_);
				memcpy(reuslt.data(), self, size_);
				return reuslt;
			}
			return std::vector<uint8_t>();
		}
	public:
		virtual void Reactivate() override
		{
			delete Info;
			CClassInfoBase* NewInfo = new TClassInfo<ClassT_>;
			SetClassInfo(NewInfo);
			return;
		};
		template<typename VariableT>
		TClass& ApplyVariable(VariableT ClassT_::* VarPtr, const std::string& Name, const std::vector<CMemberVariableMetaBase*>& IN_Metas = {})
		{
			if (VarPtr == nullptr) return *this;
			constexpr bool IsFunc = VReflectionTraits::function_traits<VariableT*>::is_func;
			static_assert(!IsFunc, "Input Is not Variable");
			auto aaaaa = VarPtr;
			using MemberPtrType_ = VariableT ClassT_::*;
			using ValueType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::return_type>::type;
			using OwnerType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::class_type>::type;
			static constexpr bool IsConst = VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::return_type>::is_const;
			const TVariableTypeContainer Type_container_(GetType<ValueType_>(), IsConst);
			CMemberVariableBase* MemberVariablePtr = new TMemberVariable<VariableT ClassT_::*>(VarPtr, Type_container_, Name, IN_Metas);
			Info->MemberVariable_.push_back(MemberVariablePtr);
			Info->MemberVariableMap_.emplace(MemberVariablePtr->GetName(), MemberVariablePtr);
			return *this;
		}
		template<typename VariableT, size_t In_Size>
		TClass& ApplyVariable(VariableT(ClassT_::* VarPtr)[In_Size], const std::string& Name, const std::vector<CMemberVariableMetaBase*>& IN_Metas = {})
		{
			if (VarPtr == nullptr) return *this;
			constexpr bool IsFunc = VReflectionTraits::function_traits<VariableT*>::is_func;
			static_assert(!IsFunc, "Input Is not Variable");
			auto aaaaa = VarPtr;
			using MemberPtrType_ = VariableT(ClassT_::*)[In_Size];
			using ValueType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::return_type>::type;
			using OwnerType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::class_type>::type;
			static constexpr bool IsConst = VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::return_type>::is_const;
			const TVariableTypeContainer Type_container_(GetType<ValueType_>(), IsConst);
			auto& I = GetType<ValueType_>();
			CMemberVariableBase* MemberVariablePtr = new TMemberVariable<VariableT(ClassT_::*)[In_Size]>(VarPtr, Type_container_, Name, IN_Metas);
			Info->MemberVariable_.push_back(MemberVariablePtr);
			Info->MemberVariableMap_.emplace(MemberVariablePtr->GetName(), MemberVariablePtr);
			return *this;
		}
		template<typename VariableT>
		TClass& ApplyVariable(VariableT* Var, const std::string& Name, const std::vector<CStaticVariableMetaBase*>& IN_Metas = {})
		{
			if (Var == nullptr) return *this;
			constexpr bool IsFunc = VReflectionTraits::function_traits<VariableT*>::is_func;
			static_assert(!IsFunc, "Input Is not Variable");
			CStaticVariableBase& Variable = RegisterStaticVariable<VariableT>(*Var, Name, this, IN_Metas);
			Info->StaticVariable_.push_back(&Variable);
			Info->StaticVariableMap_.emplace(Variable.GetName(), &Variable);
			return *this;
		}
		template<typename Ret, typename... Params>
		TClass& ApplyFunction(Ret(ClassT_::* FuncPtr)(Params...), const std::string& Name, const std::vector<CMemberFunctionMetaBase*>& IN_Metas = {})
		{
			if (FuncPtr == nullptr) return *this;
			CMemberFunctionBase& FuncType = GetType<Ret(ClassT_::*)(Params...)>();
			CMemberFunctionContainerBase* MemberFuncPtr = new TMemberFunctionContainer<Ret, ClassT_, Params...>(FuncPtr, this, FuncType, Name, IN_Metas);
			Info->MemberFunc_.push_back(MemberFuncPtr);
			Info->MemberFuncMap_.emplace(MemberFuncPtr->GetName(), MemberFuncPtr);
			return *this;
		}
		template<typename Ret, typename... Params>
		TClass& ApplyFunction(Ret(ClassT_::* FuncPtr)(Params...) const, const std::string& Name, const std::vector<CMemberFunctionMetaBase*>& IN_Metas = {})
		{
			if (FuncPtr == nullptr) return *this;
			CMemberFunctionBase& FuncType = GetType<Ret(ClassT_::*)(Params...) const>();
			CMemberFunctionContainerBase* MemberFuncPtr = new TMemberFunctionContainerConst<Ret, ClassT_, Params...>(FuncPtr, this, FuncType, Name, IN_Metas);
			Info->MemberFunc_.push_back(MemberFuncPtr);
			Info->MemberFuncMap_.emplace(MemberFuncPtr->GetName(), MemberFuncPtr);
			return *this;
		}
		template<typename Ret, typename... Params>
		TClass& ApplyFunction(Ret(*FuncPtr)(Params...), const std::string& Name, const std::vector<CStaticFunctionMetaBase*>& IN_Metas = {})
		{
			if (FuncPtr == nullptr) return *this;
			FStaticFunctionBase& FuncType = GetType<Ret(*)(Params...)>();
			CStaticFunctionContainerBase* MemberFuncPtr = new TStaticFunctionContainer<Ret, ClassT_, Params...>(FuncPtr, this, FuncType, Name, IN_Metas);
			Info->StaticFunc_.push_back(MemberFuncPtr);
			Info->StaticFuncMap_.emplace(MemberFuncPtr->GetName(), MemberFuncPtr);
			return *this;
		}
		template<typename... Params>
		TClass& ApplyConstructor(const std::vector<CStaticFunctionMetaBase*>& IN_Metas = {})
		{
			struct ClassConstructorHelper
			{
				static CAny CreateFunc(Params... args)
				{
					return new ClassT_(args...);
				}
			};
			auto FuncPtr = &ClassConstructorHelper::CreateFunc;
			FStaticFunctionBase& FuncType = GetType<CAny(*)(Params...)>();
			CStaticFunctionContainerBase* MemberFuncPtr = new TStaticFunctionContainer<CAny, ClassT_, Params...>(FuncPtr, this, FuncType, "ConstructorFunc", IN_Metas);
			Info->CreateFunc_.push_back(MemberFuncPtr);
			return *this;
		}
	};
}