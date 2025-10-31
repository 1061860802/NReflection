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
struct EG_ClassParentsHelper;

template<typename Class_T>
struct EG_ClassParentsHelper_CT;

template<typename Class_T>
struct EG_ClassParentsHelper_CT
{
	using Type = EG_ClassParentsHelper<Class_T>;
};

template<typename Class_T>
using EG_ClassParentsHelper_T = typename EG_ClassParentsHelper_CT<Class_T>::Type;

template<typename Class_T, typename Parent> 
struct EG_ClassParentsHelper<Class_T, Parent> 
{ 
	using ParentsT = std::tuple<EG_ClassParentsHelper_T<Parent>>; 
	using SelfT = Class_T; 
	template<typename OutClass_T, size_t I = 0>
	static void ParentClassesGenHelper(std::map<const ClassBase*, ParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			ClassBase* TargetClass = static_cast<ClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.insert(std::pair(TargetClass, new ParentClass<OutClass_T, ParentT::SelfT>(TargetClass)));
			ParentT::ParentClassesGenHelper<OutClass_T>(ParentClasses);
			ParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
	template<typename OutClass_T, size_t I = 0>
	static void DirectParentClassesGenHelper(std::vector<const ParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			ClassBase* TargetClass = static_cast<ClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.push_back(new ParentClass<OutClass_T, ParentT::SelfT>(TargetClass));
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
	static void ParentClassesGenHelper(std::map<const ClassBase*, ParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			ClassBase* TargetClass = static_cast<ClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.insert(std::pair(TargetClass, new ParentClass<OutClass_T, ParentT::SelfT>(TargetClass)));
			ParentT::ParentClassesGenHelper<OutClass_T>(ParentClasses);
			ParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
	template<typename OutClass_T, size_t I = 0>
	static void DirectParentClassesGenHelper(std::vector<const ParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			ClassBase* TargetClass = static_cast<ClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.push_back(new ParentClass<OutClass_T, ParentT::SelfT>(TargetClass));
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
	static void ParentClassesGenHelper(std::map<const ClassBase*, ParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			ClassBase* TargetClass = static_cast<ClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.insert(std::pair(TargetClass, new ParentClass<OutClass_T, ParentT::SelfT>(TargetClass)));
			ParentT::ParentClassesGenHelper<OutClass_T>(ParentClasses);
			ParentClassesGenHelper<OutClass_T, I + 1>(ParentClasses);
		}
	}
	template<typename OutClass_T, size_t I = 0>
	static void DirectParentClassesGenHelper(std::vector<const ParentClassBase*>& ParentClasses)
	{
		if constexpr (I == std::tuple_size_v<ParentsT>)
		{
			return;
		}
		else
		{
			using ParentT = std::tuple_element_t<I, ParentsT>;
			ClassBase* TargetClass = static_cast<ClassBase*>(GetStaticType<ParentT::SelfT>());
			ParentClasses.push_back(new ParentClass<OutClass_T, ParentT::SelfT>(TargetClass));
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
	using Type = EG_ClassParentsHelper<InClass,__VA_ARGS__>;\
};

template<typename T>
class Class : public ClassBase
{
public:
	using ClassT_ = typename T;
	Class():ClassBase(TypeName<ClassT_>::Name,typeid(T).name(), typeid(T), sizeof(ClassT_))
	{
		EG_ClassParentsHelper_T<ClassT_>::ParentClassesGenHelper<ClassT_>(ParentClasses_);
		EG_ClassParentsHelper_T<ClassT_>::DirectParentClassesGenHelper<ClassT_>(DirectParentClasses_);
	}
public:
	virtual void GCProcess(const void* OP_TargetObject, std::set<const void*>& OP_Inspecteds, std::set<IGCObjectInterface*>& OP_SomethingFound) const override
	{
		if (OP_Inspecteds.find(OP_TargetObject) != OP_Inspecteds.end()) return;
		ClassBase::GCProcess(OP_TargetObject, OP_Inspecteds, OP_SomethingFound);
		const ClassT_* TargetPtr = static_cast<const ClassT_*>(OP_TargetObject);
		if constexpr (std::is_base_of_v<IGCObjectInterface,ClassT_>)
		{
			IGCObjectInterface* InterfacePtr = const_cast<IGCObjectInterface*>(TargetPtr);
			OP_SomethingFound.insert(InterfacePtr);
		}
		MembersGCProcess(OP_TargetObject, OP_Inspecteds, OP_SomethingFound);
	}
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
	Class& ApplyVariable(VariableT ClassT_::* VarPtr,const std::string& Name)
	{
		if (VarPtr == nullptr) return *this;
		constexpr bool IsFunc = VReflectionTraits::function_traits<VariableT*>::is_func;
		static_assert(!IsFunc, "Input Isnot Variable");
		auto aaaaa = VarPtr;
		using MemberPtrType_ = VariableT ClassT_::*;
		using ValueType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::return_type>::type;
		using OwnerType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::class_type>::type;
		static constexpr bool IsConst = VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::return_type>::is_const;
		const Variable_Type_Container Type_container_(GetType<ValueType_>(), IsConst);
		MemberVariableBase* MemberVariablePtr = new MemberVariable<VariableT ClassT_::*>(VarPtr, Type_container_, Name);
		MemberVariable_.push_back(MemberVariablePtr);
		return *this;
	}
	template<typename VariableT,size_t In_Size>
	Class& ApplyVariable(VariableT (ClassT_::* VarPtr)[In_Size], const std::string& Name)
	{
		if (VarPtr == nullptr) return *this;
		constexpr bool IsFunc = VReflectionTraits::function_traits<VariableT*>::is_func;
		static_assert(!IsFunc, "Input Isnot Variable");
		auto aaaaa = VarPtr;
		using MemberPtrType_ = VariableT (ClassT_::*)[In_Size];
		using ValueType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::return_type>::type;
		using OwnerType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::class_type>::type;
		static constexpr bool IsConst = VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<MemberPtrType_>::return_type>::is_const;
		const Variable_Type_Container Type_container_(GetType<ValueType_>(), IsConst);
		auto& I = GetType<ValueType_>();
		MemberVariableBase* MemberVariablePtr = new MemberVariable<VariableT(ClassT_::*)[In_Size]>(VarPtr, Type_container_, Name);
		MemberVariable_.push_back(MemberVariablePtr);
		return *this;
	}
	template<typename VariableT>
	Class& ApplyVariable(VariableT* Var, const std::string& Name)
	{
		if (Var == nullptr) return *this;
		constexpr bool IsFunc = VReflectionTraits::function_traits<VariableT*>::is_func;
		static_assert(!IsFunc,"Input Isnot Variable");
		StaticVariableBase& Variable = RegisterStaticVariable<VariableT>(*Var, Name, this);
		StaticVariable_.push_back(&Variable);
		return *this;
	}
	template<typename Ret, typename... Params>
	Class& ApplyFunction(Ret(ClassT_::*FuncPtr)(Params...), const std::string& Name)
	{
		if (FuncPtr == nullptr) return *this;
		MemberFunctionBase& FuncType = GetType<Ret(ClassT_::*)(Params...)>();
		MemberFunctionContainerBase* MemberFuncPtr = new MemberFunctionContainer<Ret, ClassT_, Params...>(FuncPtr, this, FuncType, Name);
		MemberFunc_.push_back(MemberFuncPtr);
		return *this;
	}
	template<typename Ret, typename... Params>
	Class& ApplyFunction(Ret(ClassT_::* FuncPtr)(Params...) const, const std::string& Name)
	{
		if (FuncPtr == nullptr) return *this;
		MemberFunctionBase& FuncType = GetType<Ret(ClassT_::*)(Params...) const>();
		MemberFunctionContainerBase* MemberFuncPtr = new MemberFunctionContainerConst<Ret, ClassT_, Params...>(FuncPtr, this, FuncType, Name);
		MemberFunc_.push_back(MemberFuncPtr);
		return *this;
	}
	template<typename Ret,typename... Params>
	Class& ApplyFunction(Ret(*FuncPtr)(Params...), const std::string& Name)
	{
		if (FuncPtr == nullptr) return *this;
		StaticFunctionBase& FuncType = GetType<Ret(*)(Params...)>();
		StaticFunctionContainerBase* MemberFuncPtr = new StaticFunctionContainer<Ret, ClassT_, Params...>(FuncPtr,this,FuncType, Name);
		StaticFunc_.push_back(MemberFuncPtr);
		return *this;
	}
	template<typename... Params>
	Class& ApplyConstructor()
	{
		struct ClassConstructorHelper
		{
			static Any CreateFunc(Params... args)
			{
				return new ClassT_(args...);
			}
		};
		auto FuncPtr = &ClassConstructorHelper::CreateFunc;
		StaticFunctionBase& FuncType = GetType<Any(*)(Params...)>();
		StaticFunctionContainerBase* MemberFuncPtr = new StaticFunctionContainer<Any, ClassT_, Params...>(FuncPtr, this, FuncType,"ConstructorFunc");
		CreateFunc_.push_back(MemberFuncPtr);
		return *this;
	}
	virtual std::string print( const void* Ptr) const override
	{
		std::stringstream ss;
		ss << "<:";
		ss << std::string(*this);
		ss << ":>\n{\n";
		for (size_t i = 0; i < DirectParentClasses_.size(); i++)
		{
			ss << "Parent:\n";
			ss << DirectParentClasses_[i]->GetTargetClass()->print(Ptr);
		}
		for (size_t i = 0; i < MemberVariable_.size(); i++)
		{
			ss << MemberVariable_[i]->prints(Ptr);
		}
		for (size_t i = 0; i < StaticVariable_.size(); i++)
		{
			auto a = StaticVariable_[i];
			ss << std::string(*StaticVariable_[i]);
		}
		for (size_t i = 0; i < MemberFunc_.size(); i++)
		{
			ss << MemberFunc_[i]->prints();
		}
		for (size_t i = 0; i < StaticFunc_.size(); i++)
		{
			ss << StaticFunc_[i]->prints();
		}
		ss << "}\n";
		return ss.str();
	}
};