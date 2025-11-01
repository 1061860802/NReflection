#pragma once
#include <map>
#include <vector>
#include <sstream>
#include "Type.h"
#include "TypeFactory.h"
#include "SerializeBase.h"

class StaticVariableBase;

class StaticVariableBase : public ClassMember
{
public:
	virtual ~StaticVariableBase() {};
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
		const std::vector<StaticVariableBase*>& result = GetPtrFromString(name);
		if (Id >= result.size())
		{
			return nullptr;
		}
		return result[Id]->GetPtrSalf<T>();
	}

	const Variable_Type_Container<Type>& GetTypeContainer() const { return Type_; };
	const std::string& GetName() const { return name_; };
	virtual operator std::string() const = 0;
	bool SetValueFromString(const std::string& valstring) const
	{
		return Type_.SetValueFromString(Ptr_, valstring);
	}
	const ClassBase* GetOwnerPtr() const { return OwnerClass_; };
protected:
	void* Ptr_;
	const std::string name_;
	const Variable_Type_Container<Type> Type_;
	const ClassBase* OwnerClass_;
	StaticVariableBase(void* Ptr, const Variable_Type_Container<Type>& Type, const std::string& name, const ClassBase* OwnerClass) :
		Ptr_(Ptr), Type_(Type), name_(name), OwnerClass_(OwnerClass)
	{
		return;
	};
};

template<typename T>
class StaticVariable : public StaticVariableBase 
{
public:
	using MemberPtrType_ = typename T;
	StaticVariable(void* Ptr, const Variable_Type_Container<Type>& Type, const std::string& name, const ClassBase* OwnerClass) :
		StaticVariableBase(Ptr, Type, name, OwnerClass) {};
	virtual operator std::string() const override
	{
		std::stringstream ss;
		T* Vptr = GetPtr<T>();
		ss << "<" << std::string(GetTypeContainer()) << ">";
		ss << GetName() << ":";
		std::string VarString;
		print(VarString,*Vptr);
		ss << VarString << ";\n";
		return ss.str();
	};
public:
	void MembersGCProcess(std::set<const void*>& OP_Inspecteds, std::set<IGCObjectInterface*>& OP_SomethingFound) const
	{
		const void* VarPtr = GetPtr<const void*>();
		Type* RealType = GetObjRealType(static_cast<const MemberPtrType_*>(VarPtr));
		ClassBase* RealClassType = dynamic_cast<ClassBase*>(RealType);
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
StaticVariable<T>& RegisterStaticVariable(T& Var, const std::string& name = "UnKnowVar", const ClassBase* OwnerClass =nullptr)
{
	using ValueType_ = typename VReflectionTraits::remove_const<T>::type;
	static constexpr bool IsBool = VReflectionTraits::remove_const<T>::is_const;
	Variable_Type_Container Type_container_(GetType<ValueType_>(), IsBool);
	void* Ptr = const_cast<VReflectionTraits::remove_const<T>::type*>(&Var);
	StaticVariable<T>* inst = new StaticVariable<T>(Ptr, Type_container_, name, OwnerClass);
	return *inst;
}