#pragma once
#include <map>
#include <vector>
#include <sstream>
#include "Type.h"
#include "TypeFactory.h"
#include "SerializeBase.h"
#include "StaticMember.h"
#include "StaticVariableMeta.h"

namespace NLEngine
{

	class CStaticVariableBase;

	class CStaticVariableBase : public CStaticMember
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
		static T* GetPtrFromString(const std::string& name, size_t Id)
		{
			const std::vector<CStaticVariableBase*>& result = GetPtrFromString(name);
			if (Id >= result.size())
			{
				return nullptr;
			}
			return result[Id]->GetPtrSalf<T>();
		}

		const TVariableTypeContainer<CType>& GetTypeContainer() const { return Type_; };
		const std::string& GetName() const { return name_; };
		bool SetValueFromString(const std::string& valstring) const
		{
			return Type_.SetValueFromString(Ptr_, valstring);
		}
		const CClassBase* GetOwnerPtr() const { return OwnerClass_; };
	protected:
		void* Ptr_;
		const std::string name_;
		const TVariableTypeContainer<CType> Type_;
		const CClassBase* OwnerClass_;
		std::vector<CStaticVariableMetaBase*> Metas;
		CStaticVariableBase(void* Ptr, const TVariableTypeContainer<CType>& CType, const std::string& name, const CClassBase* OwnerClass) :
			Ptr_(Ptr), Type_(CType), name_(name), OwnerClass_(OwnerClass) {
		}
		CStaticVariableBase(void* Ptr, const TVariableTypeContainer<CType>& CType, const std::string& name, const CClassBase* OwnerClass, const std::vector<CStaticVariableMetaBase*>& IN_Metas) :
			Ptr_(Ptr), Type_(CType), name_(name), OwnerClass_(OwnerClass), Metas(IN_Metas)
		{
			for (auto i : Metas)
			{
				i->AttachedtoType(this);
			}
			return;
		};
	};

	template<typename T>
	class TStaticVariable : public CStaticVariableBase
	{
	public:
		using MemberPtrType_ = typename T;
		TStaticVariable(void* Ptr, const TVariableTypeContainer<CType>& CType, const std::string& name, const CClassBase* OwnerClass) :
			CStaticVariableBase(Ptr, CType, name, OwnerClass) {
		};
		TStaticVariable(void* Ptr, const TVariableTypeContainer<CType>& CType, const std::string& name, const CClassBase* OwnerClass, const std::vector<CStaticVariableMetaBase*>& IN_Metas) :
			CStaticVariableBase(Ptr, CType, name, OwnerClass, IN_Metas) {
		};
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
	TStaticVariable<T>& RegisterStaticVariable(T& Var, const std::string& name = "UnKnowVar", const CClassBase* OwnerClass = nullptr, const std::vector<CStaticVariableMetaBase*>& IN_Metas = {})
	{
		using ValueType_ = typename VReflectionTraits::remove_const<T>::type;
		static constexpr bool IsBool = VReflectionTraits::remove_const<T>::is_const;
		TVariableTypeContainer Type_container_(GetType<ValueType_>(), IsBool);
		void* Ptr = const_cast<VReflectionTraits::remove_const<T>::type*>(&Var);
		TStaticVariable<T>* inst = new TStaticVariable<T>(Ptr, Type_container_, name, OwnerClass, IN_Metas);
		return *inst;
	}

}