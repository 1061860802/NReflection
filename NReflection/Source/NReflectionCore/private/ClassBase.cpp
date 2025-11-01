#include "ClassBase.h"
#include "MemberFunction.h"
#include "MemberVariable.h"
#include "StaticFunction.h"
#include "StaticVariable.h"


ClassBase::~ClassBase()
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

const std::vector<MemberVariableBase*> ClassBase::GetMemberVariablesIncludeParent() const
{
	std::vector<MemberVariableBase*> result;
	for (size_t i = 0; i < DirectParentClasses_.size(); i++)
	{
		std::vector<MemberVariableBase*> Presult = DirectParentClasses_[i]->GetTargetClass()->GetMemberVariablesIncludeParent();
		result.insert(result.end(), Presult.begin(), Presult.end());
	}
	result.insert(result.end(), MemberVariable_.begin(), MemberVariable_.end());
	return result;
}

const std::vector<StaticVariableBase*> ClassBase::GetStaticVariablesIncludeParent() const
{
	std::vector<StaticVariableBase*> result;
	result.insert(result.end(), StaticVariable_.begin(), StaticVariable_.end());
	for (size_t i = 0; i < DirectParentClasses_.size(); i++)
	{
		std::vector<StaticVariableBase*> Presult = DirectParentClasses_[i]->GetTargetClass()->GetStaticVariablesIncludeParent();
		result.insert(result.end(), Presult.begin(), Presult.end());
	}
	return result;
}

const std::vector<MemberFunctionContainerBase*> ClassBase::GetMemberFunctionsIncludeParent() const
{
	std::vector<MemberFunctionContainerBase*> result;
	for (size_t i = 0; i < DirectParentClasses_.size(); i++)
	{
		std::vector<MemberFunctionContainerBase*> Presult = DirectParentClasses_[i]->GetTargetClass()->GetMemberFunctionsIncludeParent();
		result.insert(result.end(), Presult.begin(), Presult.end());
	}
	result.insert(result.end(), MemberFunc_.begin(), MemberFunc_.end());
	return result;
}

const std::vector<StaticFunctionContainerBase*> ClassBase::GetStaticFunctionsIncludeParent() const
{
	std::vector<StaticFunctionContainerBase*> result;
	for (size_t i = 0; i < DirectParentClasses_.size(); i++)
	{
		std::vector<StaticFunctionContainerBase*> Presult = DirectParentClasses_[i]->GetTargetClass()->GetStaticFunctionsIncludeParent();
		result.insert(result.end(), Presult.begin(), Presult.end());
	}
	result.insert(result.end(), StaticFunc_.begin(), StaticFunc_.end());
	return result;
}

MemberVariableBase* ClassBase::GetMemberVariable(const std::string& Name) const
{
	for (size_t i = 0; i < MemberVariable_.size(); i++)
	{
		if (MemberVariable_[i]->GetName() == Name)
		{
			return MemberVariable_[i];
		}
	}
	for (size_t i = 0; i < DirectParentClasses_.size(); i++)
	{
		MemberVariableBase* Ptr = DirectParentClasses_[i]->GetTargetClass()->GetMemberVariable(Name);
		if (Ptr)
		{
			return Ptr;
		}
	}
	return nullptr;
}

StaticVariableBase* ClassBase::GetStaticVariable(const std::string& Name) const
{
	for (size_t i = 0; i < StaticVariable_.size(); i++)
	{
		if (StaticVariable_[i]->GetName() == Name)
		{
			return StaticVariable_[i];
		}
	}
	for (size_t i = 0; i < DirectParentClasses_.size(); i++)
	{
		StaticVariableBase* Ptr = DirectParentClasses_[i]->GetTargetClass()->GetStaticVariable(Name);
		if (Ptr)
		{
			return Ptr;
		}
	}
	return nullptr;
}

MemberFunctionContainerBase* ClassBase::GetMemberFunction(const std::string& Name) const
{
	for (size_t i = 0; i < MemberFunc_.size(); i++)
	{
		if (MemberFunc_[i]->GetName() == Name)
		{
			return MemberFunc_[i];
		}
	}
	for (size_t i = 0; i < DirectParentClasses_.size(); i++)
	{
		MemberFunctionContainerBase* Ptr = DirectParentClasses_[i]->GetTargetClass()->GetMemberFunction(Name);
		if (Ptr)
		{
			return Ptr;
		}
	}
	return nullptr;
}

StaticFunctionContainerBase* ClassBase::GetStaticFunction(const std::string& Name) const
{
	for (size_t i = 0; i < StaticFunc_.size(); i++)
	{
		if (StaticFunc_[i]->GetName() == Name)
		{
			return StaticFunc_[i];
		}
	}
	for (size_t i = 0; i < DirectParentClasses_.size(); i++)
	{
		StaticFunctionContainerBase* Ptr = DirectParentClasses_[i]->GetTargetClass()->GetStaticFunction(Name);
		if (Ptr)
		{
			return Ptr;
		}
	}
	return nullptr;
}

std::optional<Any> ClassBase::CreateAny(const std::vector<AnyRef> Params) const
{
	for (size_t i = 0; i < CreateFunc_.size(); i++)
	{
		auto CreateFunc = CreateFunc_[i];
		auto Ret = CreateFunc->CallFuncAny(Params);
		if (Ret)
		{
			return Ret;
		}
	}
	return std::nullopt;
}

void* ClassBase::CastToParent(const ClassBase* TargetParentType, void* Self) const
{
	if (TargetParentType == this)
	{
		return Self;
	}
	auto CasterItr = ParentClasses_.find(TargetParentType);
	if (CasterItr == ParentClasses_.end())
	{
		return nullptr;
	}
	return CasterItr->second->CastToParent(Self);
}

const void* ClassBase::CastToParent(const ClassBase* TargetParentType,const void* Self) const
{
	if (TargetParentType == this)
	{
		return Self;
	}
	auto CasterItr = ParentClasses_.find(TargetParentType);
	if (CasterItr == ParentClasses_.end())
	{
		return nullptr;
	}
	return CasterItr->second->CastToParent(Self);
}

void* ClassBase::CastFromParent(const ClassBase* InPtrType, void* ParentPtr) const
{
	if (InPtrType == this)
	{
		return ParentPtr;
	}
	auto CasterItr = ParentClasses_.find(InPtrType);
	if (CasterItr == ParentClasses_.end())
	{
		return nullptr;
	}
	return CasterItr->second->CastFrom(ParentPtr);
}

const void* ClassBase::CastFromParent(const ClassBase* InPtrType, const void* ParentPtr) const
{
	if (InPtrType == this)
	{
		return ParentPtr;
	}
	auto CasterItr = ParentClasses_.find(InPtrType);
	if (CasterItr == ParentClasses_.end())
	{
		return nullptr;
	}
	return CasterItr->second->CastFrom(ParentPtr);
}

void* ClassBase::CastTo(const ClassBase* TargetParentType, void* Self) const
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

const void* ClassBase::CastTo(const ClassBase* TargetParentType, const void* Self) const
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

bool ClassBase::IsBaseOf(const Type& other) const
{
	if (this == &other)
	{
		return true;
	}
	for (auto& i : ParentClasses_)
	{
		if (i.second->GetTargetClass() == &other)
		{
			return true;
		}
	}
	return false;
}