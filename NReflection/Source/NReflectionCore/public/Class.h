#pragma once
#include "Type.h"
#include "ClassBase.h"
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
struct EG_ClassParentsHelper;

template<typename Class_T>
struct EG_ClassParentsHelper_CT;

template<typename Class_T>
struct EG_ClassParentsHelper_CT
{
	using CType = EG_ClassParentsHelper<Class_T>;
};

template<typename Class_T>
using EG_ClassParentsHelper_T = typename EG_ClassParentsHelper_CT<Class_T>::CType;

template<typename Class_T, typename Parent> 
struct EG_ClassParentsHelper<Class_T, Parent> 
{ 
	using ParentsT = std::tuple<EG_ClassParentsHelper_T<Parent>>; 
	using SelfT = Class_T; 
	template<typename OutClass_T, size_t I = 0>
	static void ParentClassesGenHelper(std::map<const CClassBase*,const CParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			CClassBase* TargetClass = static_cast<CClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.insert(std::pair(TargetClass, new TParentClass<OutClass_T, ParentT::SelfT>(TargetClass)));
			ParentT::ParentClassesGenHelper<OutClass_T>(ParentClasses);
			ParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
	template<typename OutClass_T, size_t I = 0>
	static void DirectParentClassesGenHelper(std::vector<const CParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			CClassBase* TargetClass = static_cast<CClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.push_back(new TParentClass<OutClass_T, ParentT::SelfT>(TargetClass));
			DirectParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
};

template<typename Class_T>
struct EG_ClassParentsHelper<Class_T>
{
	using ParentsT = std::tuple<>;
	using SelfT = Class_T;
	template<typename OutClass_T, size_t I = 0>
	static void ParentClassesGenHelper(std::map<const CClassBase*,const CParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			CClassBase* TargetClass = static_cast<CClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.insert(std::pair(TargetClass, new TParentClass<OutClass_T, ParentT::SelfT>(TargetClass)));
			ParentT::ParentClassesGenHelper<OutClass_T>(ParentClasses);
			ParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
	template<typename OutClass_T, size_t I = 0>
	static void DirectParentClassesGenHelper(std::vector<const CParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			CClassBase* TargetClass = static_cast<CClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.push_back(new TParentClass<OutClass_T, ParentT::SelfT>(TargetClass));
			DirectParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
};

template<typename Class_T, typename FirstParent, typename... RestParents> 
struct EG_ClassParentsHelper<Class_T, FirstParent, RestParents...>
{
	using ParentsT = typename decltype(std::tuple_cat(std::tuple<EG_ClassParentsHelper_T<FirstParent>>(), typename EG_ClassParentsHelper<Class_T, RestParents...>::ParentsT()));
	using SelfT = typename Class_T;

	template<typename OutClass_T, size_t I = 0>
	static void ParentClassesGenHelper(std::map<const CClassBase*,const CParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			CClassBase* TargetClass = static_cast<CClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.insert(std::pair(TargetClass, new TParentClass<OutClass_T, ParentT::SelfT>(TargetClass)));
			ParentT::ParentClassesGenHelper<OutClass_T>(ParentClasses);
			ParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
	template<typename OutClass_T, size_t I = 0>
	static void DirectParentClassesGenHelper(std::vector<const CParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			CClassBase* TargetClass = static_cast<CClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.push_back(new TParentClass<OutClass_T, ParentT::SelfT>(TargetClass));
			DirectParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
};

template<typename Class_T,typename... Parents>
struct EG_ParentTestHelper;

template<typename Class_T, typename Parent,typename... Parents>
struct EG_ParentTestHelper<Class_T, Parent, Parents...>
{
	static constexpr bool Success = std::is_base_of_v<Parent, Class_T>&&EG_ParentTestHelper<Class_T,Parents...>::Success;
};

template<typename Class_T, typename Parent>
struct EG_ParentTestHelper<Class_T, Parent>
{
	static constexpr bool Success = std::is_base_of_v<Parent, Class_T>;
};

#define PARENTS(InClass,...) \
static_assert(bool(EG_ParentTestHelper<InClass,__VA_ARGS__>::Success),"Please check input types");\
template<>\
struct EG_ClassParentsHelper_CT<InClass>\
{\
	using CType = EG_ClassParentsHelper<InClass,__VA_ARGS__>;\
};

template<typename T>
class TClass : public CClassBase
{
public:
	using ClassT_ = typename T;
	TClass():CClassBase(TTypeName<ClassT_>::Name,typeid(T).name(), typeid(T), sizeof(ClassT_))
	{
		EG_ClassParentsHelper_T<ClassT_>::ParentClassesGenHelper<ClassT_>(ParentClasses_);
		EG_ClassParentsHelper_T<ClassT_>::DirectParentClassesGenHelper<ClassT_>(DirectParentClasses_);
	}
public:
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
	template<typename VariableT>
	TClass& ApplyVariable(VariableT ClassT_::* VarPtr,const std::string& Name)
	{
		if (VarPtr == nullptr) return *this;
		constexpr bool IsFunc = VReflectionTraits::function_traits<VariableT*>::is_func;
		static_assert(!IsFunc, "Input Isnot Variable");
		auto aaaaa = VarPtr;
		using MemberPtrType_ = VariableT ClassT_::*;
		using ValueType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::return_type>::type;
		using OwnerType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::class_type>::type;
		static constexpr bool IsConst = VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::return_type>::is_const;
		const TVariable_Type_Container Type_container_(GetType<ValueType_>(), IsConst);
		CMemberVariableBase* MemberVariablePtr = new TMemberVariable<VariableT ClassT_::*>(VarPtr, Type_container_, Name);
		MemberVariable_.push_back(MemberVariablePtr);
		return *this;
	}
	template<typename VariableT,size_t In_Size>
	TClass& ApplyVariable(VariableT (ClassT_::* VarPtr)[In_Size], const std::string& Name)
	{
		if (VarPtr == nullptr) return *this;
		constexpr bool IsFunc = VReflectionTraits::function_traits<VariableT*>::is_func;
		static_assert(!IsFunc, "Input Isnot Variable");
		auto aaaaa = VarPtr;
		using MemberPtrType_ = VariableT (ClassT_::*)[In_Size];
		using ValueType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::return_type>::type;
		using OwnerType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::class_type>::type;
		static constexpr bool IsConst = VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::return_type>::is_const;
		const TVariable_Type_Container Type_container_(GetType<ValueType_>(), IsConst);
		auto& I = GetType<ValueType_>();
		CMemberVariableBase* MemberVariablePtr = new TMemberVariable<VariableT(ClassT_::*)[In_Size]>(VarPtr, Type_container_, Name);
		MemberVariable_.push_back(MemberVariablePtr);
		return *this;
	}
	template<typename VariableT>
	TClass& ApplyVariable(VariableT* Var, const std::string& Name)
	{
		if (Var == nullptr) return *this;
		constexpr bool IsFunc = VReflectionTraits::function_traits<VariableT*>::is_func;
		static_assert(!IsFunc,"Input Isnot Variable");
		CStaticVariableBase& Variable = RegisterStaticVariable<VariableT>(*Var, Name, this);
		StaticVariable_.push_back(&Variable);
		return *this;
	}
	template<typename Ret, typename... Params>
	TClass& ApplyFunction(Ret(ClassT_::*FuncPtr)(Params...), const std::string& Name)
	{
		if (FuncPtr == nullptr) return *this;
		CMemberFunctionBase& FuncType = GetType<Ret(ClassT_::*)(Params...)>();
		CMemberFunctionContainerBase* MemberFuncPtr = new TMemberFunctionContainer<Ret, ClassT_, Params...>(FuncPtr, this, FuncType, Name);
		MemberFunc_.push_back(MemberFuncPtr);
		return *this;
	}
	template<typename Ret, typename... Params>
	TClass& ApplyFunction(Ret(ClassT_::* FuncPtr)(Params...) const, const std::string& Name)
	{
		if (FuncPtr == nullptr) return *this;
		CMemberFunctionBase& FuncType = GetType<Ret(ClassT_::*)(Params...) const>();
		CMemberFunctionContainerBase* MemberFuncPtr = new TMemberFunctionContainerConst<Ret, ClassT_, Params...>(FuncPtr, this, FuncType, Name);
		MemberFunc_.push_back(MemberFuncPtr);
		return *this;
	}
	template<typename Ret,typename... Params>
	TClass& ApplyFunction(Ret(*FuncPtr)(Params...), const std::string& Name)
	{
		if (FuncPtr == nullptr) return *this;
		FStaticFunctionBase& FuncType = GetType<Ret(*)(Params...)>();
		CStaticFunctionContainerBase* MemberFuncPtr = new TStaticFunctionContainer<Ret, ClassT_, Params...>(FuncPtr,this,FuncType, Name);
		StaticFunc_.push_back(MemberFuncPtr);
		return *this;
	}
	template<typename... Params>
	TClass& ApplyConstructor()
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
		CStaticFunctionContainerBase* MemberFuncPtr = new TStaticFunctionContainer<CAny, ClassT_, Params...>(FuncPtr, this, FuncType,"ConstructorFunc");
		CreateFunc_.push_back(MemberFuncPtr);
		return *this;
	}
};