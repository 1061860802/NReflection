#pragma once
#include "Type.h"
#include "TypeFactory.h"
#include "BaseNumeric.h"
#include "Bool.h"
#include "SerializeBase.h"
#include "Variable_traits.h"
#include <sstream>
#include "ClassMember.h"
#include "MemberVariableMeta.h"

namespace NLEngine
{

	class CClassBase;

	class CMemberVariableBase : public CClassMember
	{
	public:
		virtual ~CMemberVariableBase() {};
		void ApplyMeta(CMemberVariableMetaBase* IN_Meta)
		{
			Metas.push_back(IN_Meta);
			IN_Meta->AttachedtoType(this);
		}
		void ApplyMetas(const std::vector<CMemberVariableMetaBase*>& IN_Metas)
		{
			for (auto i : IN_Metas)
			{
				Metas.push_back(i);
				i->AttachedtoType(this);
			}
		}
		const std::string& GetName() const { return name_; };
		const TVariableTypeContainer<CType>& GetTypeContainer() const { return Type_; };
		const CClassBase* GetOwnerPtr() const { return &OwnerClass_; };
		template<typename T, typename Pt>
		T* GetVariablePtr(Pt* OwnerPtr)
		{
			CClassBase* realType = static_cast<CClassBase*>(GetObjRealType(OwnerPtr));
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
			return (T*)GetPtr(Ptr);
		}
		template<typename T, typename Pt>
		const T* GetVariablePtrConst(const Pt* OwnerPtr)
		{
			CClassBase* realType = static_cast<CClassBase*>(GetObjRealType(OwnerPtr));
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
			return (const T*)GetPtr(Ptr);
		}
		virtual const void* GetPtr(const void* Owner) const = 0;
		virtual bool SetValueFromString(void* Owner, const std::string& valstring) const = 0;
	protected:
		const std::string name_;
		const TVariableTypeContainer<CType> Type_;
		const CClassBase& OwnerClass_;
		std::vector<CMemberVariableMetaBase*> Metas;
		CMemberVariableBase(const std::string& name, const TVariableTypeContainer<CType>& CType, const CClassBase& OwnerClass, const std::vector<CMemberVariableMetaBase*>& IN_Metas) :
			name_(name), Type_(CType), OwnerClass_(OwnerClass), Metas(IN_Metas)
		{
			for (auto i : Metas)
			{
				i->AttachedtoType(this);
			}
		};
		virtual void* GetPtr(void* Owner) const = 0;
	};

	template<typename T>
	struct TMemberVariable :public CMemberVariableBase
	{
	public:
		using MemberPtrType_ = typename T;
		using ValueType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<T>::return_type>::type;
		using OwnerType_ = typename VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<T>::class_type>::type;
		static constexpr bool IsConst = VReflectionTraits::remove_const<typename VReflectionTraits::variable_traits<T>::return_type>::is_const;
		TMemberVariable(const T VariablePtr, const TVariableTypeContainer<CType>& Type_container_, const std::string& name, const std::vector<CMemberVariableMetaBase*>& IN_Metas) :
			CMemberVariableBase(name, Type_container_, GetType<OwnerType_>(), IN_Metas)
		{
			VariablePtr_ = VariablePtr;
		};
		virtual bool SetValueFromString(void* Owner, const std::string& valstring) const override
		{
			void* ValPtr = GetPtr(Owner);
			return Type_.SetValueFromString(ValPtr, valstring);
		}
	protected:
		virtual void* GetPtr(void* Owner) const override
		{
			OwnerType_* OwnerPtr = (OwnerType_*)Owner;
			void* result = (void*)&(OwnerPtr->*VariablePtr_);
			return result;
		};
		virtual const void* GetPtr(const void* Owner) const override
		{
			OwnerType_* OwnerPtr = (OwnerType_*)Owner;
			void* result = (void*)&(OwnerPtr->*VariablePtr_);
			return result;
		};
	public:
		void MembersGCProcess(const void* In_OwnerObject, std::set<const void*>& OP_Inspecteds, std::set<IGCObjectInterface*>& OP_SomethingFound) const
		{
			const MemberPtrType_* VarPtr = static_cast<const MemberPtrType_*>(GetPtr(In_OwnerObject));
			CType* RealType = GetObjRealType(VarPtr);
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
	private:
		typename MemberPtrType_ VariablePtr_;
	};

}