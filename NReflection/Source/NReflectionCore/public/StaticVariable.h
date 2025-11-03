#pragma once
#include <map>
#include <vector>
#include <sstream>
#include "Type.h"
#include "TypeFactory.h"
#include "SerializeBase.h"

class CStaticVariableBase;

class CStaticVariableBase : public CClassMember
{
public:
	virtual ~CStaticVariableBase() {};
	template<typename T>
	T* GetPtr() const
	{
		return (T*)Ptr_;
	}
	template<typename T>
	T* GetPtrSalf() const
	{
		if (Type_ == Register<T>().Info())
		{
			return GetPtr<T>();
		}
		return nullptr;
	}
	template<typename T>
	static T* GetPtrFromString(const std::string& name,size_t Id)
	{
		const std::vector<CStaticVariableBase*>& result = GetPtrFromString(name);
		if (Id >= result.size())
		{
			return nullptr;
		}
		return result[Id]->GetPtrSalf<T>();
	}

	const TVariable_Type_Container<CType>& GetTypeContainer() const { return Type_; };
	const std::string& GetName() const { return name_; };
	bool SetValueFromString(const std::string& valstring) const
	{
		return Type_.SetValueFromString(Ptr_, valstring);
	}
	const CClassBase* GetOwnerPtr() const { return OwnerClass_; };
protected:
	void* Ptr_;
	const std::string name_;
	const TVariable_Type_Container<CType> Type_;
	const CClassBase* OwnerClass_;
	CStaticVariableBase(void* Ptr, const TVariable_Type_Container<CType>& CType, const std::string& name, const CClassBase* OwnerClass) :
		Ptr_(Ptr), Type_(CType), name_(name), OwnerClass_(OwnerClass)
	{
		return;
	};
};

template<typename T>
class TStaticVariable : public CStaticVariableBase 
{
public:
	using MemberPtrType_ = typename T;
	TStaticVariable(void* Ptr, const TVariable_Type_Container<CType>& CType, const std::string& name, const CClassBase* OwnerClass) :
		CStaticVariableBase(Ptr, CType, name, OwnerClass) {};
public:
	void MembersGCProcess(std::set<const void*>& OP_Inspecteds, std::set<IGCObjectInterface*>& OP_SomethingFound) const
	{
		const void* VarPtr = GetPtr<const void*>();
		CType* RealType = GetObjRealType(static_cast<const MemberPtrType_*>(VarPtr));
		CClassBase* RealClassType = dynamic_cast<CClassBase*>(RealType);
		if (RealClassType)
		{
			RealClassType->GCProcess(RealClassType->CastFromParent(Type_.GetTypeConst(), VarPtr), OP_Inspecteds, OP_SomethingFound);
		}
		else
		{
			RealType->GCProcess(VarPtr, OP_Inspecteds, OP_SomethingFound);
		}
	}
};

template<typename T>
TStaticVariable<T>& RegisterStaticVariable(T& Var, const std::string& name = "UnKnowVar", const CClassBase* OwnerClass =nullptr)
{
	using ValueType_ = typename VReflectionTraits::remove_const<T>::type;
	static constexpr bool IsBool = VReflectionTraits::remove_const<T>::is_const;
	TVariable_Type_Container Type_container_(GetType<ValueType_>(), IsBool);
	void* Ptr = const_cast<VReflectionTraits::remove_const<T>::type*>(&Var);
	TStaticVariable<T>* inst = new TStaticVariable<T>(Ptr, Type_container_, name, OwnerClass);
	return *inst;
}