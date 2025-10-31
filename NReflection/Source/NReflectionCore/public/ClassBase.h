#pragma once

#include "Type.h"
#include <optional>
#include "Void.h"
#include <vector>
#include "Any.h"
#include "NReflectionCoreAPI.h"

class MemberVariableBase;
class StaticVariableBase;
class MemberFunctionContainerBase;
class StaticFunctionContainerBase;
class ClassBase;

struct NREFLECTIONCORE_API ParentClassBase
{
	ParentClassBase(ClassBase* ParentClass) : ParentClass_(ParentClass) {}
	virtual void* CastToParent(void* self) const = 0;
	virtual void* CastFrom(void* Ptr) const = 0;
	virtual const void* CastToParent(const void* self) const = 0;
	virtual const void* CastFrom(const void* Ptr) const = 0;
	ClassBase* GetTargetClass() const
	{
		return ParentClass_;
	}
private:
	ClassBase* ParentClass_;
};

template<typename T_self, typename T_Parent>
struct ParentClass : ParentClassBase
{
	using SelfType = typename T_self;
	using ParnetType = typename T_Parent;
	ParentClass(ClassBase* ParentClass) : ParentClassBase(ParentClass) {}
	virtual void* CastToParent(void* self) const override
	{
		SelfType* SelfPtr = (SelfType*)self;
		ParnetType* ParentPtr = static_cast<ParnetType*>(SelfPtr);
		return ParentPtr;
	}
	virtual void* CastFrom(void* Ptr) const override
	{
		ParnetType* ParentPtr = (ParnetType*)Ptr;
		SelfType* Self = static_cast<SelfType*>(ParentPtr);
		return Self;
	}
	virtual const void* CastToParent(const void* self) const override
	{
		const SelfType* SelfPtr = (const SelfType*)self;
		const ParnetType* ParentPtr = static_cast<const ParnetType*>(SelfPtr);
		return ParentPtr;
	}
	virtual const void* CastFrom(const void* Ptr) const override
	{
		const ParnetType* ParentPtr = (const ParnetType*)Ptr;
		const SelfType* Self = static_cast<const SelfType*>(ParentPtr);
		return Self;
	}
};

class NREFLECTIONCORE_API ClassBase : public Type
{
public:
	virtual ~ClassBase();
	const std::vector<const ParentClassBase*>& GetDriectParents() const { return DirectParentClasses_; };
	const std::vector<MemberVariableBase*>& GetMemberVariables() const { return MemberVariable_; };
	const std::vector<StaticVariableBase*>& GetStaticVariables() const { return StaticVariable_; };
	const std::vector<MemberFunctionContainerBase*>& GetMemberFunctions() const { return MemberFunc_; };
	const std::vector<StaticFunctionContainerBase*>& GetStaticFunctions() const { return StaticFunc_; };
	const std::vector<MemberVariableBase*> GetMemberVariablesIncludeParent() const;
	const std::vector<StaticVariableBase*> GetStaticVariablesIncludeParent() const;
	const std::vector<MemberFunctionContainerBase*> GetMemberFunctionsIncludeParent() const;
	const std::vector<StaticFunctionContainerBase*> GetStaticFunctionsIncludeParent() const;
	MemberVariableBase* GetMemberVariable(const std::string& Name) const;
	StaticVariableBase* GetStaticVariable(const std::string& Name) const;
	MemberFunctionContainerBase* GetMemberFunction(const std::string& Name) const;
	StaticFunctionContainerBase* GetStaticFunction(const std::string& Name) const;
	virtual std::optional<Any> Create(const std::vector<AnyRef> Params) const;
	void* CastToParent(const ClassBase* TargetParentType, void* Self) const;
	const void* CastToParent(const ClassBase* TargetParentType, const void* Self) const;
	void* CastFromParent(const ClassBase* InPtrType, void* ParentPtr) const;
	const void* CastFromParent(const ClassBase* InPtrType, const void* ParentPtr) const;
	bool IsBaseOf(const Type& other) const;
	virtual std::string print(const void* Ptr) const = 0;
protected:
	std::map<const ClassBase*,ParentClassBase*> ParentClasses_;
	std::vector<const ParentClassBase*> DirectParentClasses_;
	std::vector<MemberVariableBase*> MemberVariable_;
	std::vector<StaticVariableBase*> StaticVariable_;
	std::vector<MemberFunctionContainerBase*> MemberFunc_;
	std::vector<StaticFunctionContainerBase*> StaticFunc_;
	std::vector<StaticFunctionContainerBase*> CreateFunc_;
	ClassBase(const std::string& Name, const std::string& realname, const type_info& type_info_, size_t size) :Type(Name, realname,type_info_,size) {}
	void ApplyMemberVariable(MemberVariableBase* MemberVariable)
	{
		MemberVariable_.push_back(MemberVariable);
	}
	void MembersGCProcess(const void* OP_TargetObject, std::set<const void*>& OP_Inspecteds, std::set<IGCObjectInterface*>& OP_SomethingFound) const;
};
