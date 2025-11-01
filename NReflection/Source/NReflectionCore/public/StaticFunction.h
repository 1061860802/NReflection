#pragma once

#include "Variable_traits.h"
#include "TypeFactory.h"
#include "Type.h"
#include <vector>
#include <tuple>
#include <optional>

class ClassBase;

class StaticFunctionBase : public Type
{
public:
	StaticFunctionBase(const Variable_Type_Container<Type>& Result, std::vector<Variable_Type_Container<Type>> Params,const std::string& Name, const std::string& RealName, const type_info& type_info_) :
		Result_(Result), Params_(Params), Type(Name, RealName, type_info_, 0, true) {};
	virtual std::string print() const
	{
		return name_;
	}
	const Variable_Type_Container<Type>& GetResult() const { return Result_; };
	const std::vector<Variable_Type_Container<Type>>& GetParams() const { return Params_; };
protected:
	const Variable_Type_Container<Type> Result_;
	std::vector<Variable_Type_Container<Type>> Params_;
};

template<typename Ret,typename... Params>
class StaticFunction : public StaticFunctionBase
{
public:
	StaticFunction(const Variable_Type_Container<Type>& Result, std::vector<Variable_Type_Container<Type>> Params) :
		StaticFunctionBase(Result, Params,typeid(Type_).name(),typeid(Type_).name(), typeid(Type_)) {}
	using Type_ = typename Ret(*)(Params...);
	using Ret_ = typename Ret;
	using ParamsTuple = typename std::tuple<Params...>;
};

template<typename Ret,typename... Params>
class StaticFuncFactory
{
public:
	using Type_ = typename Ret(*)(Params...);
	using args_tuple = typename std::tuple<Params...>;

	using RetT = typename VReflectionTraits::remove_const<Ret>::type;
	static constexpr bool RetConst = VReflectionTraits::remove_const<Ret>::is_const;
	StaticFuncFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<StaticFunction<Ret, Params...>>(Variable_Type_Container<Type>(GetType<RetT>(), RetConst), GetParams());
	};
	static StaticFuncFactory& Instance()
	{
		static StaticFuncFactory inst;
		StaticFuncFactory* instPtr = &inst;
		return inst;
	}
	StaticFunction<Ret, Params...>& Info() { return *info_; }
private:
	typename StaticFunction<Ret, Params...> *info_;
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
			using TT = typename VReflectionTraits::remove_const<std::tuple_element_t<I, args_tuple>>::type;
			static constexpr bool TTIsConst = VReflectionTraits::remove_const<std::tuple_element_t<I, args_tuple>>::is_const;
			Variable_Type_Container<Type> TC_(GetType<TT>(), TTIsConst);
			Inputs.push_back(TC_);
			cvtTypeList2Vector<I + 1>(Inputs);
		}
	}
};

template<typename Ret,typename... Params>
class FactoryGetter<Ret(*)(Params...)>
{
public:
	using GetFactory = StaticFuncFactory<Ret, Params...>;
};

class StaticFunctionContainerBase : public ClassMember
{
public:
	virtual ~StaticFunctionContainerBase() {};
	StaticFunctionContainerBase(const ClassBase* OwnerClass,const StaticFunctionBase& Type, const std::string& Name) :OwnerClass_(OwnerClass),Type_(Type), Name_(Name) {}
	const std::string& GetName() const { return Name_; };
	const StaticFunctionBase& GetType() const { return Type_; };
	const ClassBase* GetOwnerPtr() const { return OwnerClass_; };
	template<typename... Params>
	bool CallFunc(Params&&... args) const
	{
		auto valRet = CallFuncAny({ args... });
		if (valRet)
		{
			return true;
		}
		return false;
	};
	template<typename Ret_T,typename... Params>
	std::optional<Ret_T> CallFunc(Params&&... args) const
	{
		auto valRet = CallFuncAny({ args ... });
		if (valRet)
		{
			return (Ret_T)valRet.value();
		}
		return std::nullopt;
	};
	virtual std::optional<Any> CallFuncAny(const std::vector<AnyRef>& Params) const = 0;
	virtual std::string prints() const = 0;
protected:
	const ClassBase* OwnerClass_;
	const StaticFunctionBase& Type_;
	std::string Name_;
};

template<typename Ret, typename Class, typename... Params>
class StaticFunctionContainer : public StaticFunctionContainerBase
{
public:
	using param_ = typename std::tuple<Params...>;
	using class_ = typename Class;
	using NoConstRet = typename VReflectionTraits::remove_const<Ret>::type;
	using Ret_ = typename Ret;
	virtual ~StaticFunctionContainer() {};
	StaticFunctionContainer(Ret(*FuncPtr_)(Params...), const ClassBase* OwnerClass, const StaticFunctionBase& Type, const std::string& Name) :FuncPtr_(FuncPtr_), StaticFunctionContainerBase(OwnerClass,Type, Name) {}
	virtual std::optional<Any> CallFuncAny(const std::vector<AnyRef>& Params) const override
	{
		if (Params.size() != Type_.GetParams().size()) return std::nullopt;
		for (size_t i = 0; i < Type_.GetParams().size(); i++)
		{
			if (!(Type_.GetParams()[i].InputRequire(Params[i].GetValType()))) return std::nullopt;
		}
		return CallFuncAnyImp(Params, std::make_index_sequence<std::tuple_size_v<param_>>());
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
	Ret(*FuncPtr_)(Params...);
	template<size_t... Idx>
	Any CallFuncAnyImp(const std::vector<AnyRef>& params, std::index_sequence<Idx...>) const
	{
		if constexpr (std::is_same_v<NoConstRet, void>)
		{
			(*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...);
			return Any();
		}
		else
		{
			if constexpr (VReflectionTraits::remove_reference<Ret_>::is_reference)
			{
				return Any::GetRefAny((*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...));
			}
			else
			{
				return (*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...);
			}
		}
	};
};
