#pragma once

#include "Type.h"
#include <optional>
#include "Void.h"
#include <vector>
#include <unordered_map>
#include "Any.h"
#include "NReflectionCoreAPI.h"

namespace NLEngine
{
	class CMemberVariableBase;
	class CStaticVariableBase;
	class CMemberFunctionContainerBase;
	class CStaticFunctionContainerBase;
	class CClassBase;

	struct NREFLECTIONCORE_API CParentClassBase
	{
		CParentClassBase(CClassBase* ParentClass) : ParentClass_(ParentClass) {}
		virtual void* CastToParent(void* self) const = 0;
		virtual void* CastFrom(void* Ptr) const = 0;
		virtual const void* CastToParent(const void* self) const = 0;
		virtual const void* CastFrom(const void* Ptr) const = 0;
		CClassBase* GetTargetClass() const
		{
			return ParentClass_;
		}
	private:
		CClassBase* ParentClass_;
	};

	template<typename T_self, typename T_Parent>
	struct TParentClass : CParentClassBase
	{
		using T_SelfType = typename T_self;
		using T_ParnetType = typename T_Parent;
		TParentClass(CClassBase* ParentClass) : CParentClassBase(ParentClass) {}
		virtual void* CastToParent(void* self) const override
		{
			T_SelfType* SelfPtr = (T_SelfType*)self;
			T_ParnetType* ParentPtr = static_cast<T_ParnetType*>(SelfPtr);
			return ParentPtr;
		}
		virtual void* CastFrom(void* Ptr) const override
		{
			T_ParnetType* ParentPtr = (T_ParnetType*)Ptr;
			T_SelfType* Self = static_cast<T_SelfType*>(ParentPtr);
			return Self;
		}
		virtual const void* CastToParent(const void* self) const override
		{
			const T_SelfType* SelfPtr = (const T_SelfType*)self;
			const T_ParnetType* ParentPtr = static_cast<const T_ParnetType*>(SelfPtr);
			return ParentPtr;
		}
		virtual const void* CastFrom(const void* Ptr) const override
		{
			const T_ParnetType* ParentPtr = (const T_ParnetType*)Ptr;
			const T_SelfType* Self = static_cast<const T_SelfType*>(ParentPtr);
			return Self;
		}
	};

	class NREFLECTIONCORE_API CClassInfoBase
	{
	public:
		CClassInfoBase() = default;
		virtual ~CClassInfoBase();
		virtual void PostConstructorProcess();
		std::map<const CClassBase*, const CParentClassBase*> ParentClasses_;
		std::vector<const CParentClassBase*> DirectParentClasses_;
		std::vector<CMemberVariableBase*> MemberVariable_;
		std::vector<CStaticVariableBase*> StaticVariable_;
		std::vector<CMemberFunctionContainerBase*> MemberFunc_;
		std::vector<CStaticFunctionContainerBase*> StaticFunc_;
		std::vector<CStaticFunctionContainerBase*> CreateFunc_;

		std::unordered_map<std::string,CMemberVariableBase*> MemberVariableMap_;
		std::unordered_map<std::string,CStaticVariableBase*> StaticVariableMap_;
		std::unordered_map<std::string, CMemberFunctionContainerBase*> MemberFuncMap_;
		std::unordered_map<std::string, CStaticFunctionContainerBase*> StaticFuncMap_;
	};


	class NREFLECTIONCORE_API CClassBase : public CType
	{
	public:
		virtual ~CClassBase();
		const std::vector<const CParentClassBase*>& GetDriectParents() const { return Info->DirectParentClasses_; };
		const std::vector<CMemberVariableBase*>& GetLocalMemberVariables() const { return Info->MemberVariable_; };
		const std::vector<CStaticVariableBase*>& GetLocalStaticVariables() const { return Info->StaticVariable_; };
		const std::vector<CMemberFunctionContainerBase*>& GetLocalMemberFunctions() const { return Info->MemberFunc_; };
		const std::vector<CStaticFunctionContainerBase*>& GetLocalStaticFunctions() const { return Info->StaticFunc_; };
		const std::vector<CMemberVariableBase*> GetOwnMemberVariables() const;
		const std::vector<CStaticVariableBase*> GetOwnStaticVariables() const;
		const std::vector<CMemberFunctionContainerBase*> GetOwnMemberFunctions() const;
		const std::vector<CStaticFunctionContainerBase*> GetOwnStaticFunctions() const;
		CMemberVariableBase* GetMemberVariable(const std::string& Name) const;
		CStaticVariableBase* GetStaticVariable(const std::string& Name) const;
		CMemberFunctionContainerBase* GetMemberFunction(const std::string& Name) const;
		CStaticFunctionContainerBase* GetStaticFunction(const std::string& Name) const;
		virtual std::optional<CAny> CreateAny(const std::vector<AnyRef> Params) const;
		template<typename T, typename... ParamsT>
		T* Create(ParamsT&&... IN_Args) const
		{
			std::optional<CAny> tmp = CClassBase::CreateAny({ IN_Args... });
			if (tmp)
			{
				const CPointerBase* PointerType = dynamic_cast<const CPointerBase*>(&tmp->GetValType().GetTypeConst());
				if (PointerType == nullptr) return nullptr;
				const CClassBase* CType = dynamic_cast<const CClassBase*>(&PointerType->GetInner().GetTypeConst());
				if (CType == nullptr) return nullptr;
				void* ptr = CType->CastTo(this, *(void**)(tmp->GetValue()));
				if (ptr == nullptr) return nullptr;
				void* result = this->CastTo(&GetType<T>(), ptr);
				if (result == nullptr) return nullptr;
				return (T*)result;
			}
			return nullptr;
		};
		void* CastToParent(const CClassBase* IN_TargetParentType, void* IN_Self) const;
		const void* CastToParent(const CClassBase* IN_TargetParentType, const void* IN_Self) const;
		void* CastFromParent(const CClassBase* IN_InPtrType, void* IN_ParentPtr) const;
		const void* CastFromParent(const CClassBase* IN_InPtrType, const void* IN_ParentPtr) const;
		void* CastTo(const CClassBase* IN_TargetParentType, void* IN_Self) const;
		const void* CastTo(const CClassBase* IN_TargetParentType, const void* IN_Self) const;
		bool IsBaseOf(const CType& IN_Other) const;
	protected:
		CClassBase(const std::string& Name, const std::string& realname, const type_info& type_info_, size_t size) : Info(nullptr), CType(Name, realname, type_info_, size) {}
		void ApplyMemberVariable(CMemberVariableBase* TMemberVariable)
		{
			Info->MemberVariable_.push_back(TMemberVariable);
		}
		void SetClassInfo(CClassInfoBase* IN_Info);
		CClassInfoBase* Info;
	};

}