#include <iostream>
#include "BaseNumeric.h"
#include "ClassFactory.h"
#include "Any.h"
#include "Array.h"
#include "MemberFunction.h"
#include "Reference.h"
#include "GCSystemBase.h"
#include "Variable_traits.h"

struct AA
{
    int a1 = 114514;
};

struct MyStruct2
{
    int t1 = 123;
    int t2 = 456;
    int t3;
    const int t4 = 123456;
    static float ttt;
    static void RegisterStruct()
    {
        NLEngine::Register<MyStruct2>().Info()
            .ApplyVariable(&MyStruct2::t1, "t1")
            .ApplyVariable(&MyStruct2::t2, "t2")
            .ApplyVariable(&MyStruct2::t4, "t4");
    }
};

struct MyStruct3
{
    int qwe;
};

struct InterfaceTestBase
{
    float aaa;
    virtual ~InterfaceTestBase() {};
};

struct InterfaceTestBase2
{
    float bbb;
    virtual ~InterfaceTestBase2() {};
};

struct InterfaceTest : public InterfaceTestBase, public InterfaceTestBase2
{
    float ccc;
    static void RegisterStruct()
    {
        NLEngine::Register<InterfaceTest>().Info()
            .ApplyVariable(&InterfaceTest::ccc, "ccc");
    }
};
PARENTS(InterfaceTest, InterfaceTestBase, InterfaceTestBase2)

struct MyStruct
{
    MyStruct() {};
    virtual ~MyStruct() {};
    std::vector<int> MyStruct3M;
    MyStruct2 MyStruct2_;
    int t1 = 10086;
    const int64_t t2 = 1919810;
    const int64_t array[10]{ 0 };
    double t3 = 0;
    double f1 = 11452;
    const int t4 = 123456;
    static float ttt;
    float TestFunc() { return 0; };
    virtual float& TestFunc1(float& cc, double ca) { std::cout << "MyStruct Call aaaaaaa" << std::endl; return ttt; };
    static void StaticFuncTest(int i)
    {
        std::cout << "StaticFuncTest:" << i << std::endl;
    }
    static void RegisterStruct()
    {
        NLEngine::Register<MyStruct>().Info()
            .ApplyConstructor<>()
            .ApplyVariable(&MyStruct::MyStruct3M, "MyStruct3M")
            .ApplyVariable(&MyStruct::MyStruct2_, "MyStruct2_")
            .ApplyFunction(&MyStruct::TestFunc, "TestFunc")
            .ApplyFunction(&MyStruct::TestFunc1, "TestFunc1")
            .ApplyFunction(&MyStruct::StaticFuncTest, "StaticFuncTest")
            .ApplyVariable(&MyStruct::t1, "t1")
            .ApplyVariable(&MyStruct::t2, "t2")
            .ApplyVariable(&MyStruct::array, "array")
            .ApplyVariable(&MyStruct::t3, "t3")
            .ApplyVariable(&MyStruct::f1, "f1")
            .ApplyVariable(&MyStruct::t4, "t4")
            .ApplyVariable(&MyStruct::ttt, "ttt")
            .ApplyVariable(new int(114), "OO");
    }
};

float aaaCCC = 12345;

struct MyStructC : public MyStruct
{
    MyStructC()
    {
        std::cout << "C Init" << std::endl;
    };
    MyStructC(const int& Prit)
    {
        std::cout <<"C" << Prit << std::endl;
    }
    virtual float& TestFunc1(float& cc, double ca) override
    {
        cc = 10236;
        std::cout << "MyStructC Call" << std::endl;
        for (size_t i = 0; i < 10; i++)
        {
            ArrayTest[i] = i;
        }
        return aaaCCC;
    }
    static void StaticFuncTest2(float& cc, double ca)
    {
        cc = 19198;
        std::cout << "StaticFuncTest2:" << cc << std::endl;
    }
    float ArrayTest[10];
    static void RegisterStruct()
    {
        using ValueType_ = typename NLEngine::VReflectionTraits::remove_const<typename NLEngine::VReflectionTraits::variable_traits<decltype(&MyStructC::ArrayTest)>::return_type>::type;
        auto& I = NLEngine::Register<MyStructC>().Info();
        I.ApplyConstructor<const int&>()
            .ApplyConstructor<>()
            .ApplyFunction(&MyStructC::StaticFuncTest2, "StaticFuncTest2")
            .ApplyFunction(&MyStructC::TestFunc1,"TestFunc1")
            .ApplyVariable(&MyStructC::ArrayTest, "ArrayTest");
        return;
    }
};
PARENTS(MyStructC, MyStruct)

class CMemberVariableMetaTest : public NLEngine::CMemberVariableMetaBase
{
public:
    CMemberVariableMetaTest() {};
    virtual ~CMemberVariableMetaTest() = default;
protected:
    virtual void AttachedtoType(NLEngine::CMemberVariableBase* OP_Function) override
    {
        std::cout << "Var " << OP_Function->GetName() << "Apply a Meta" << std::endl;
        return;
    }
    virtual void DetachFromType(NLEngine::CMemberVariableBase* OP_Function) override
    {
        return;
    }
};

CMemberVariableMetaTest TestMemberMeta;

struct MyStructD : public MyStructC, public InterfaceTest
{
    MyStructD()
    {
        std::cout << "D Init" << std::endl;
    };
    MyStructD(const int& Prit) : MyStructC(Prit)
    {
        std::cout << Prit << std::endl;
    }
    float Mem = 0;
    static void RegisterStruct()
    {
        InterfaceTest::RegisterStruct();
        NLEngine::Register<MyStructD>().Info()
            .ApplyConstructor<>()
            .ApplyConstructor<const int&>()
            .ApplyVariable(&MyStructD::Mem, "Mem", { &TestMemberMeta });
    }
};
PARENTS(MyStructD, MyStructC, InterfaceTest)

template<>
struct NLEngine::TTypeName<MyStruct>
{
    static constexpr char Name[] = "MyStruct";
};

template<>
struct NLEngine::TTypeName<MyStructC>
{
    static constexpr char Name[] = "MyStructC";
};

int aaa;

int& TestFunc(int& TTT) { return aaa; };

float MyStruct::ttt = 114514;

int main()
{
    MyStruct::RegisterStruct();
    MyStructC::RegisterStruct();
    MyStructD::RegisterStruct();
    auto& T = NLEngine::GetType<MyStructD>();
    for (auto i : T.GetOwnMemberVariables())
    {
        std::cout << i->GetOwnerPtr()->GetTypeName()<<"::" << i->GetName()<<"   "<<i->GetTypeContainer().GetTypeConst().GetTypeName() << std::endl;
    }
    for (auto i : T.GetOwnMemberFunctions())
    {
        std::cout << i->GetOwnerPtr()->GetTypeName() << "::" << i->GetName() << "   " << i->GetType().GetTypeName() << std::endl;
    }
    for (auto i : T.GetOwnStaticVariables())
    {
        std::cout << i->GetOwnerPtr()->GetTypeName() << "::" << i->GetName() << "   " << i->GetTypeContainer().GetTypeConst().GetTypeName() << std::endl;
    }
    for (auto i : T.GetOwnStaticFunctions())
    {
        std::cout << i->GetOwnerPtr()->GetTypeName() << "::" << i->GetName() << "   " << i->GetType().GetTypeName() << std::endl;
    }
    InterfaceTest* CPtr = NLEngine::GetStaticType<MyStructD>()->Create<InterfaceTest>();
    auto* bkptr = NLEngine::GetStaticType<MyStruct>();
    float t = 0;
    T.GetMemberFunction("TestFunc1")->CallFunc(CPtr,t,12.0);
    auto arrayrow = T.GetMemberVariable("ArrayTest")->GetVariablePtr<float[10]>(CPtr);
    (*arrayrow)[8] = 0;
    auto array = T.GetMemberVariable("ArrayTest")->GetVariablePtrConst<float[10]>(CPtr);
    for (size_t i = 0; i < 10; i++)
    {
        std::cout << (*array)[i] << std::endl;
    }
    auto val = T.GetMemberVariable("f1")->GetVariablePtrConst<double>(CPtr);
    std::cout << (*val) << std::endl;
    auto staticVal = T.GetStaticVariable("ttt")->GetPtr<float>();
    std::cout << (*staticVal) << std::endl;
    return 0;
}