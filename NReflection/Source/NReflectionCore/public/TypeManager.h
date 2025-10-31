#pragma once
#include <vector>
#include <map>
#include<string>
#include <atomic>
#include "Type.h"

class NREFLECTIONCORE_API TypeManager
{
private:
	class SpinLock {
	public:
		SpinLock() = default;
		SpinLock(const SpinLock&) = delete;
		SpinLock& operator=(const SpinLock&) = delete;

		void lock() {
			while (flag.test_and_set(std::memory_order_acquire)) {}
		}

		void unlock() {
			flag.clear(std::memory_order_release);
		}

	private:
		std::atomic_flag flag = ATOMIC_FLAG_INIT;
	};
	mutable SpinLock Lock;
public:
	TypeManager();
	~TypeManager();
	Type* FromNameFindTypeImp(const std::string& Name) const;
	Type* FromRealNameFindTypeImp(const std::string& Name) const;
	size_t TypesNum() const;
	template<typename OutType>
	OutType* FromNameFindType(const std::string& Name) const
	{
		Lock.lock();
		if (Type* tmp = FromNameFindTypeImp(Name))
		{
			Lock.unlock();
			return dynamic_cast<OutType*>(tmp);
		}
		Lock.unlock();
		return nullptr;
	}
	template<typename OutType>
	OutType* FromRealNameFindType(const std::string& Name) const
	{
		Lock.lock();
		if (Type* tmp = FromRealNameFindTypeImp(Name))
		{
			Lock.unlock();
			return dynamic_cast<OutType*>(tmp);
		}
		Lock.unlock();
		return nullptr;
	}
	template<typename OutType>
	OutType* FromRealNameFindTypeNoLock(const std::string& Name) const
	{
		if (Type* tmp = FromRealNameFindTypeImp(Name))
		{
			return dynamic_cast<OutType*>(tmp);
		}
		return nullptr;
	}
	template<typename T_OutType, typename T_TargetType>
	T_OutType* FindTypeByType() const
	{
		Lock.lock();
		const std::string Name = RealTypeName<T_TargetType>::Name;
		if (Type* tmp = FromRealNameFindTypeImp(Name))
		{
			Lock.unlock();
			return dynamic_cast<T_OutType*>(tmp);
		}
		Lock.unlock();
		return nullptr;
	}
	template<typename OutType,typename... Params>
	OutType* GetOrRegistor(Params... args)
	{
		OutType* NewType = new OutType(args...);
		bool success = TryApplyType(NewType);
		if (!success)
		{
			const std::string Name = NewType->GetRealName();
			OutType* result = FromRealNameFindType<OutType>(Name);
			//delete NewType;
			if (result)
			{
				return result;
			}
			return nullptr;
		}
		return NewType;
	}
	static void RegisterTypeManager();
public:
	std::map<std::string,Type*> TypesMap;
	bool TryApplyType(Type* NewType);
};

NREFLECTIONCORE_API TypeManager& GetTypeManager();

template<typename T_InObjType>
Type* GetObjRealType(T_InObjType* In_Target)
{
	if constexpr (std::is_polymorphic_v<T_InObjType>)
	{
		TypeManager& TypeManagerObj = GetTypeManager();
		Type* Result = TypeManagerObj.FromRealNameFindTypeImp(typeid(*In_Target).name());
		if (Result)
		{
			return Result;
		}
		else
		{
			return &GetType<T_InObjType>();
		}
	}
	else
	{
		return &GetType<T_InObjType>();
	}
}

template<typename T_InObjType>
auto* GetStaticType()
{
	return &GetType<T_InObjType>();
}
