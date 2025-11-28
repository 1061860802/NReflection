#include "ClassBase.h"
#include "MemberFunction.h"
#include "MemberVariable.h"
#include "StaticFunction.h"
#include "StaticVariable.h"

using namespace NLEngine;

CClassInfoBase::~CClassInfoBase()
{
	for (auto i = ParentClasses_.begin(); i != ParentClasses_.end(); i++)
	{
		delete i->second;
	}
	for (auto i = DirectParentClasses_.begin(); i != DirectParentClasses_.end(); i++)
	{
		delete *i;
	}
	for (auto i = MemberVariable_.begin(); i != MemberVariable_.end(); i++)
	{
		delete* i;
	}
	for (auto i = StaticVariable_.begin(); i != StaticVariable_.end(); i++)
	{
		delete* i;
	}
	for (auto i = MemberFunc_.begin(); i != MemberFunc_.end(); i++)
	{
		delete* i;
	}
	for (auto i = StaticFunc_.begin(); i != StaticFunc_.end(); i++)
	{
		delete* i;
	}
	for (auto i = CreateFunc_.begin(); i != CreateFunc_.end(); i++)
	{
		delete* i;
	}
}

CClassBase::~CClassBase()
{
	delete Info;
}

const std::vector<CMemberVariableBase*> CClassBase::GetOwnMemberVariables() const
{
	std::vector<CMemberVariableBase*> result;
	for (size_t i = 0; i < Info->DirectParentClasses_.size(); i++)
	{
		std::vector<CMemberVariableBase*> Presult = Info->DirectParentClasses_[i]->GetTargetClass()->GetOwnMemberVariables();
		result.insert(result.end(), Presult.begin(), Presult.end());
	}
	result.insert(result.end(), Info->MemberVariable_.begin(), Info->MemberVariable_.end());
	return result;
}

const std::vector<CStaticVariableBase*> CClassBase::GetOwnStaticVariables() const
{
	std::vector<CStaticVariableBase*> result;
	result.insert(result.end(), Info->StaticVariable_.begin(), Info->StaticVariable_.end());
	for (size_t i = 0; i < Info->DirectParentClasses_.size(); i++)
	{
		std::vector<CStaticVariableBase*> Presult = Info->DirectParentClasses_[i]->GetTargetClass()->GetOwnStaticVariables();
		result.insert(result.end(), Presult.begin(), Presult.end());
	}
	return result;
}

const std::vector<CMemberFunctionContainerBase*> CClassBase::GetOwnMemberFunctions() const
{
	std::vector<CMemberFunctionContainerBase*> result;
	for (size_t i = 0; i < Info->DirectParentClasses_.size(); i++)
	{
		std::vector<CMemberFunctionContainerBase*> Presult = Info->DirectParentClasses_[i]->GetTargetClass()->GetOwnMemberFunctions();
		result.insert(result.end(), Presult.begin(), Presult.end());
	}
	result.insert(result.end(), Info->MemberFunc_.begin(), Info->MemberFunc_.end());
	return result;
}

const std::vector<CStaticFunctionContainerBase*> CClassBase::GetOwnStaticFunctions() const
{
	std::vector<CStaticFunctionContainerBase*> result;
	for (size_t i = 0; i < Info->DirectParentClasses_.size(); i++)
	{
		std::vector<CStaticFunctionContainerBase*> Presult = Info->DirectParentClasses_[i]->GetTargetClass()->GetOwnStaticFunctions();
		result.insert(result.end(), Presult.begin(), Presult.end());
	}
	result.insert(result.end(), Info->StaticFunc_.begin(), Info->StaticFunc_.end());
	return result;
}

CMemberVariableBase* CClassBase::GetMemberVariable(const std::string& Name) const
{
	for (size_t i = 0; i < Info->MemberVariable_.size(); i++)
	{
		if (Info->MemberVariable_[i]->GetName() == Name)
		{
			return Info->MemberVariable_[i];
		}
	}
	for (size_t i = 0; i < Info->DirectParentClasses_.size(); i++)
	{
		CMemberVariableBase* Ptr = Info->DirectParentClasses_[i]->GetTargetClass()->GetMemberVariable(Name);
		if (Ptr)
		{
			return Ptr;
		}
	}
	return nullptr;
}

CStaticVariableBase* CClassBase::GetStaticVariable(const std::string& Name) const
{
	for (size_t i = 0; i < Info->StaticVariable_.size(); i++)
	{
		if (Info->StaticVariable_[i]->GetName() == Name)
		{
			return Info->StaticVariable_[i];
		}
	}
	for (size_t i = 0; i < Info->DirectParentClasses_.size(); i++)
	{
		CStaticVariableBase* Ptr = Info->DirectParentClasses_[i]->GetTargetClass()->GetStaticVariable(Name);
		if (Ptr)
		{
			return Ptr;
		}
	}
	return nullptr;
}

CMemberFunctionContainerBase* CClassBase::GetMemberFunction(const std::string& Name) const
{
	for (size_t i = 0; i < Info->MemberFunc_.size(); i++)
	{
		if (Info->MemberFunc_[i]->GetName() == Name)
		{
			return Info->MemberFunc_[i];
		}
	}
	for (size_t i = 0; i < Info->DirectParentClasses_.size(); i++)
	{
		CMemberFunctionContainerBase* Ptr = Info->DirectParentClasses_[i]->GetTargetClass()->GetMemberFunction(Name);
		if (Ptr)
		{
			return Ptr;
		}
	}
	return nullptr;
}

CStaticFunctionContainerBase* CClassBase::GetStaticFunction(const std::string& Name) const
{
	for (size_t i = 0; i < Info->StaticFunc_.size(); i++)
	{
		if (Info->StaticFunc_[i]->GetName() == Name)
		{
			return Info->StaticFunc_[i];
		}
	}
	for (size_t i = 0; i < Info->DirectParentClasses_.size(); i++)
	{
		CStaticFunctionContainerBase* Ptr = Info->DirectParentClasses_[i]->GetTargetClass()->GetStaticFunction(Name);
		if (Ptr)
		{
			return Ptr;
		}
	}
	return nullptr;
}

std::optional<CAny> CClassBase::CreateAny(const std::vector<AnyRef> Params) const
{
	for (size_t i = 0; i < Info->CreateFunc_.size(); i++)
	{
		auto CreateFunc = Info->CreateFunc_[i];
		auto Ret = CreateFunc->CallFuncAny(Params);
		if (Ret)
		{
			return Ret;
		}
	}
	return std::nullopt;
}

void* CClassBase::CastToParent(const CClassBase* TargetParentType, void* Self) const
{
	if (TargetParentType == this)
	{
		return Self;
	}
	auto CasterItr = Info->ParentClasses_.find(TargetParentType);
	if (CasterItr == Info->ParentClasses_.end())
	{
		return nullptr;
	}
	return CasterItr->second->CastToParent(Self);
}

const void* CClassBase::CastToParent(const CClassBase* TargetParentType,const void* Self) const
{
	if (TargetParentType == this)
	{
		return Self;
	}
	auto CasterItr = Info->ParentClasses_.find(TargetParentType);
	if (CasterItr == Info->ParentClasses_.end())
	{
		return nullptr;
	}
	return CasterItr->second->CastToParent(Self);
}

void* CClassBase::CastFromParent(const CClassBase* InPtrType, void* ParentPtr) const
{
	if (InPtrType == this)
	{
		return ParentPtr;
	}
	auto CasterItr = Info->ParentClasses_.find(InPtrType);
	if (CasterItr == Info->ParentClasses_.end())
	{
		return nullptr;
	}
	return CasterItr->second->CastFrom(ParentPtr);
}

const void* CClassBase::CastFromParent(const CClassBase* InPtrType, const void* ParentPtr) const
{
	if (InPtrType == this)
	{
		return ParentPtr;
	}
	auto CasterItr = Info->ParentClasses_.find(InPtrType);
	if (CasterItr == Info->ParentClasses_.end())
	{
		return nullptr;
	}
	return CasterItr->second->CastFrom(ParentPtr);
}

void* CClassBase::CastTo(const CClassBase* TargetParentType, void* Self) const
{
	if (void* ptr = TargetParentType->CastFromParent(this,Self))
	{
		return ptr;
	}
	if (void* ptr = CastToParent(TargetParentType, Self))
	{
		return ptr;
	}
	return nullptr;
}

const void* CClassBase::CastTo(const CClassBase* TargetParentType, const void* Self) const
{
	if (const void* ptr = TargetParentType->CastFromParent(this, Self))
	{
		return ptr;
	}
	if (const void* ptr = CastToParent(TargetParentType, Self))
	{
		return ptr;
	}
	return nullptr;
}

bool CClassBase::IsBaseOf(const CType& other) const
{
	if (this == &other)
	{
		return true;
	}
	for (auto& i : Info->ParentClasses_)
	{
		if (i.second->GetTargetClass() == &other)
		{
			return true;
		}
	}
	return false;
}

void CClassBase::SetClassInfo(CClassInfoBase* IN_Info)
{
	Info = IN_Info;
}
