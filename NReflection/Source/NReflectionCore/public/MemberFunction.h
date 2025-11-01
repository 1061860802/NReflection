#pragma once

#include "Variable_traits.h"
#include "TypeFactory.h"
#include "Type.h"
#include "ClassBase.h"
#include "Any.h"
#include <sstream>
#include <vector>
#include <tuple>
#include <optional>

class ClassBase;

class MemberFunctionBase : public Type
{
public:
	MemberFunctionBase(const Variable_Type_Container<Type>& OwnerClass,const Variable_Type_Container<Type>& Result, std::vector<Variable_Type_Container<Type>> Params,const std::string& Name,const std::string& RealName, const type_info& type_info_) :
		OwnerClass_(OwnerClass), Result_(Result), Params_(Params), Type(Name, RealName,type_info_,0, true) {};
	virtual std::string print() const
	{
		return name_;
	}

	const Variable_Type_Container<Type>& GetTargetClass() const { return OwnerClass_; };
	const Variable_Type_Container<Type>& GetResult() const { return Result_; };
	const std::vector<Variable_Type_Container<Type>>& GetParams() const {return Params_;};
protected:
	const Variable_Type_Container<Type> OwnerClass_;
	const Variable_Type_Container<Type> Result_;
	std::vector<Variable_Type_Container<Type>> Params_;
};

template<typename Ret, typename Class, typename... Params>
class MemberFunction : public MemberFunctionBase
{
public:
	MemberFunction(const Variable_Type_Container<Type>& OwnerClass, const Variable_Type_Container<Type>& Result, std::vector<Variable_Type_Container<Type>> Params) :
		MemberFunctionBase(OwnerClass, Result, Params,typeid(Type_).name(), typeid(Type_).name(), typeid(Type_)) {}
	using Type_ = typename Ret(Class::*)(Params...);
	using Ret_ = typename Ret;
	using ParamsTuple = typename std::tuple<Params...>;
};

template<typename Ret, typename Class, typename... Params>
class MemberFuncFactory
{
public:
	using Type_ = typename Ret(Class::*)(Params...);
	using args_tuple = typename std::tuple<Params...>;

	using RetT = typename VReflectionTraits::remove_const<Ret>::type;
	static constexpr bool RetConst = VReflectionTraits::remove_const<Ret>::is_const;
	using ClassT = typename VReflectionTraits::remove_const<Class>::type;
	static constexpr bool ClassConst = VReflectionTraits::remove_const<Class>::is_const;
	MemberFuncFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<MemberFunction<Ret, Class, Params...>>(Variable_Type_Container<Type>(GetType<ClassT>(), ClassConst), Variable_Type_Container<Type>(GetType<RetT>(), RetConst), GetParams());
	};
	static MemberFuncFactory& Instance()
	{
		static MemberFuncFactory inst;
		MemberFuncFactory* instPtr = &inst;
		return inst;
	}
	MemberFunction<Ret, Class,Params...>& Info() { return *info_; }
private:
	typename MemberFunction<Ret, Class, Params...>* info_;
	static std::vector<Variable_Type_Container<Type>> GetParams() 
	{ 
		std::vector<Variable_Type_Container<Type>> Result;
		cvtTypeList2Vector<0>(Result);
		return Result;
	};
	template<size_t I>
	static void cvtTypeList2Vector(std::vector<Variable_Type_Container<Type>>& Inputs)
	{
		if constexpr (I == std::tuple_size_v<args_tuple>)
		{
			return;
		}
		else
		{
			using TT = typename VReflectionTraits::remove_const<std::tuple_element_t<I,args_tuple>>::type;
			static constexpr bool TTIsConst = VReflectionTraits::remove_const<std::tuple_element_t<I,args_tuple>>::is_const;
			Variable_Type_Container<Type> TC_(GetType<TT>(), TTIsConst);
			Inputs.push_back(TC_);
			cvtTypeList2Vector<I + 1>(Inputs);
		}
	}
};

template<typename Ret, typename Class, typename... Params>
class FactoryGetter<Ret(Class::*)(Params...)>
{
public:
	using GetFactory = MemberFuncFactory<Ret, Class, Params...>;
};

template<typename Ret, typename Class, typename... Params>
class FactoryGetter<Ret(Class::*)(Params...) const>
{
public:
	using GetFactory = MemberFuncFactory<Ret,const Class, Params...>;
};

class MemberFunctionContainerBase : public ClassMember
{
public:
	virtual ~MemberFunctionContainerBase() {};
	MemberFunctionContainerBase(const MemberFunctionBase& Type,const std::string& Name, const ClassBase* OwnerClass):Type_(Type), Name_(Name), OwnerClass_(OwnerClass){}
	const std::string& GetName() const { return Name_; };
	const MemberFunctionBase& GetType() const { return Type_; };
	const ClassBase* GetOwnerPtr() const { return OwnerClass_; };
	template<typename T, typename... Params>
	bool CallFunc(T* Caller, Params&&... args) const
	{
		ClassBase* realType = static_cast<ClassBase*>(GetObjRealType(Caller));
		void* realPtr = realType->CastFromParent(&::GetType<T>(), Caller);
		if (realPtr == nullptr)
		{
			return false;
		}
		void* Ptr = realType->CastToParent(OwnerClass_, realPtr);
		if (Ptr == nullptr)
		{
			return false;
		}
		auto valRet = CallFuncAny(Ptr, {args...});
		if (valRet)
		{
			return true;
		}
		return false;
	};
	template<typename Ret_T,typename T, typename... Params>
	std::optional<Ret_T> CallFunc(T* Caller, Params&&... args) const
	{
		ClassBase* realType = static_cast<ClassBase*>(GetObjRealType(Caller));
		void* realPtr = realType->CastFromParent(&::GetType<T>(), Caller);
		if (realPtr == nullptr)
		{
			return std::nullopt;
		}
		void* Ptr = realType->CastToParent(OwnerClass_, realPtr);
		if (Ptr == nullptr)
		{
			return std::nullopt;
		}
		auto valRet = CallFuncAny(Ptr, { args ... });
		if (valRet)
		{
			return (Ret_T)valRet.value();
		}
		return std::nullopt;
	};
	virtual std::optional<Any> CallFuncAny(void* Caller, const std::vector<AnyRef>& Params) const = 0;
	virtual std::string prints() const = 0;
protected:
	const ClassBase* OwnerClass_;
	const MemberFunctionBase& Type_;
	std::string Name_;
};

template<typename Ret, typename Class, typename... Params>
class MemberFunctionContainer : public MemberFunctionContainerBase
{
public:
	using param_ = typename std::tuple<Params...>;
	using class_ = typename Class;
	using NoConstRet = typename VReflectionTraits::remove_const<Ret>::type;
	using Ret_ = typename Ret;
	virtual ~MemberFunctionContainer() {};
	MemberFunctionContainer(Ret(Class::* FuncPtr_)(Params...), const ClassBase* OwnerClass ,const MemberFunctionBase& Type, const std::string& Name) :FuncPtr_(FuncPtr_), MemberFunctionContainerBase(Type, Name, OwnerClass) {}
	virtual std::string prints() const override
	{
		std::stringstream ss;
		ss << "<Function:";
		ss << GetName() << ">";
		ss << Type_.print() << ";\n";
		return ss.str();
	};
	virtual std::optional<Any> CallFuncAny(void* Caller, const std::vector<AnyRef>& Params) const override
	{
		if (Params.size() != Type_.GetParams().size()) return std::nullopt;
		class_* CallerPtr = (class_*)Caller;
		for (size_t i = 0; i < Type_.GetParams().size(); i++)
		{
			if (!(Type_.GetParams()[i].InputRequire(Params[i].GetValType())))
			{
				std::cout << "实参" << std::string(Params[i].GetValType()) << "与形参" << std::string(Type_.GetParams()[i]) << "不兼容" << std::endl;
				return std::nullopt;
			}
		}
		return CallFuncAnyImp(*CallerPtr, Params, std::make_index_sequence<std::tuple_size_v<param_>>());
	}
protected:
	Ret(Class::* FuncPtr_)(Params...);
	template<size_t... Idx>
	Any CallFuncAnyImp(class_& caller, const std::vector<AnyRef>& params, std::index_sequence<Idx...>) const
	{
		if constexpr (std::is_same_v<NoConstRet, void>)
		{
			(caller.*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...);
			return Any();
		}
		else
		{
			if constexpr (VReflectionTraits::remove_reference<Ret_>::is_reference)
			{
				return Any::GetRefAny((caller.*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...));
			}
			else
			{
				return (caller.*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...);
			}
		}
	};
};

template<typename Ret, typename Class, typename... Params>
struct MemberFunctionContainerConst : public MemberFunctionContainerBase
{
public:
	using param_ = typename std::tuple<Params...>;
	using class_ = typename Class;
	using NoConstRet = typename VReflectionTraits::remove_const<Ret>::type;
	using Ret_ = typename Ret;
	virtual ~MemberFunctionContainerConst() {};
	MemberFunctionContainerConst(Ret(Class::* FuncPtr_)(Params...) const, const ClassBase* OwnerClass ,const MemberFunctionBase& Type, const std::string& Name) :FuncPtr_(FuncPtr_), MemberFunctionContainerBase(Type, Name, OwnerClass) {}
	virtual std::optional<Any> CallFuncAny(const AnyRef& Caller, const std::vector<AnyRef>& Params) const override
	{
		if (Params.size() != Type_.GetParams().size()) return std::nullopt;
		const ClassBase* InCallerType = dynamic_cast<const ClassBase*>(&Caller.GetValType().GetTypeConst());
		if (InCallerType == nullptr) return std::nullopt;
		void* Ptr = InCallerType->CastToParent(static_cast<const ClassBase*>(& Type_.GetTargetClass().GetTypeConst()), Caller.GetValue());
		if (Ptr == nullptr) return std::nullopt;
		class_* CallerPtr = (class_*)Ptr;
		for (size_t i = 0; i < Type_.GetParams().size(); i++)
		{
			if (!(Type_.GetParams()[i].InputRequire(Params[i].GetValType())))
			{
				std::cout << "实参" << std::string(Params[i].GetValType()) << "与形参" << std::string(Type_.GetParams()[i]) << "不兼容" << std::endl;
				return std::nullopt;
			}
		}
		return CallFuncAnyImp(*CallerPtr, Params, std::make_index_sequence<std::tuple_size_v<param_>>());
	}
	virtual std::string prints() const override
	{
		std::stringstream ss;
		ss << "<Function:";
		ss << GetName() << ">";
		ss << Type_.print() << ";\n";
		return ss.str();
	};
protected:
	Ret(Class::* FuncPtr_)(Params...) const;
	template<size_t... Idx>
	Any CallFuncAnyImp(class_& caller, const std::vector<AnyRef>& params, std::index_sequence<Idx...>) const
	{
		if constexpr (std::is_same_v<NoConstRet, void>)
		{
			(caller.*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...);
			return Any();
		}
		else
		{
			if constexpr (VReflectionTraits::remove_reference<Ret_>::is_reference)
			{
				return Any::GetRefAny((caller.*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...));
			}
			else
			{
				return (caller.*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...);
			}
		}
	};
};