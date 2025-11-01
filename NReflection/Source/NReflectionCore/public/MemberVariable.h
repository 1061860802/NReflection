#pragma once
#include "Type.h"
#include "TypeFactory.h"
#include "BaseNumeric.h"
#include "Bool.h"
#include "SerializeBase.h"
#include "Variable_traits.h"
#include <sstream>

class ClassBase;

struct TestStruct
{
	int t1;
	int t2;
	static float ttt;
};

class MemberVariableBase : public ClassMember
{
public:
	virtual ~MemberVariableBase() {};
	const std::string& GetName() const { return name_; };
	const Variable_Type_Container<Type>& GetTypeContainer() const { return Type_; };
	const ClassBase* GetOwnerPtr() const { return &OwnerClass_; };
	template<typename T, typename Pt>
	T* FromInstantGetVariablePtr(Pt* OwnerPtr)
	{
		ClassBase* realType = static_cast<ClassBase*>(GetObjRealType(OwnerPtr));
		void* realPtr = realType->CastFromParent(&GetType<Pt>(), OwnerPtr);
		if (realPtr == nullptr)
		{
			return nullptr;
		}
		void* Ptr = realType->CastToParent(&OwnerClass_, realPtr);
		if (Ptr == nullptr)
		{
			return nullptr;
		}
		if (Type_.GetTypeConst() != GetType<T>())
		{
			return nullptr;
		}
		return (T*)FromOwnerPtrGetPtr(Ptr);
	}
	template<typename T, typename Pt>
	const T* FromInstantGetVariablePtrConst(const Pt* OwnerPtr)
	{
		ClassBase* realType = static_cast<ClassBase*>(GetObjRealType(OwnerPtr));
		const void* realPtr = realType->CastFromParent(&GetType<Pt>(), OwnerPtr);
		if (realPtr == nullptr)
		{
			return nullptr;
		}
		const void* Ptr = realType->CastToParent(&OwnerClass_, realPtr);
		if (Ptr == nullptr)
		{
			return nullptr;
		}
		if (Type_.GetTypeConst() != GetType<T>())
		{
			return nullptr;
		}
		return (const T*)FromOwnerPtrGetPtr(Ptr);
	}
	virtual std::string prints(const void* OwnerPtr) const = 0;
	virtual const void* FromOwnerPtrGetPtr(const void* Owner) const = 0;
	virtual bool SetValueFromString(void* Owner, const std::string& valstring) const = 0;
protected:
	const std::string name_;
	const Variable_Type_Container<Type> Type_;
	const ClassBase& OwnerClass_;
	MemberVariableBase(const std::string& name, const Variable_Type_Container<Type>& Type, const ClassBase& OwnerClass) :
		name_(name), Type_(Type), OwnerClass_(OwnerClass)
	{
		return;
	};
	virtual void* FromOwnerPtrGetPtr(void* Owner) const = 0;
};

template<typename T>
struct MemberVariable :public MemberVariableBase
{
public:
	using MemberPtrType_ = typename T;
	using ValueType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<T>::return_type>::type;
	using OwnerType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<T>::class_type>::type;
	static constexpr bool IsConst = VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<T>::return_type>::is_const;
	MemberVariable(const T VariablePtr, const Variable_Type_Container<Type>& Type_container_,const std::string& name) :
		MemberVariableBase(name,Type_container_, GetType<OwnerType_>())
	{
		VariablePtr_ = VariablePtr;
	};
	virtual std::string prints(const void* OwnerPtr) const override
	{
		typename VReflectionTraits::variable_traits<T>::return_type* Ptr = (typename VReflectionTraits::variable_traits<T>::return_type*)FromOwnerPtrGetPtr(OwnerPtr);
		std::stringstream ss;
		ss << "<" << std::string(GetTypeContainer()) << ">";
		ss << GetName() << ":";
		std::string VarString;
		print(VarString, *Ptr);
		ss << VarString << ";\n";
		return ss.str();
	};
	virtual bool SetValueFromString(void* Owner, const std::string& valstring) const override
	{
		void* ValPtr = FromOwnerPtrGetPtr(Owner);
		return Type_.SetValueFromString(ValPtr, valstring);
	}
protected:
	virtual void* FromOwnerPtrGetPtr(void* Owner) const override
	{
		OwnerType_* OwnerPtr = (OwnerType_*)Owner;
		void* result = (void*)&(OwnerPtr->*VariablePtr_);
		return result;
	};
	virtual const void* FromOwnerPtrGetPtr(const void* Owner) const override
	{
		OwnerType_* OwnerPtr = (OwnerType_*)Owner;
		void* result = (void*)&(OwnerPtr->*VariablePtr_);
		return result;
	};
public:
	void MembersGCProcess(const void* In_OwnerObject, std::set<const void*>& OP_Inspecteds, std::set<IGCObjectInterface*>& OP_SomethingFound) const
	{
		const MemberPtrType_* VarPtr = static_cast<const MemberPtrType_*>(FromOwnerPtrGetPtr(In_OwnerObject));
		Type* RealType = GetObjRealType(VarPtr);
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
private:
	typename MemberPtrType_ VariablePtr_;
};