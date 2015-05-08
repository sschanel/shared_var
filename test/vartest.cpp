#include "stdafx.h"
#include "CppUnitTest.h"
#include "shared_var.h"
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace shared_vartest
{		
	TEST_CLASS(VarTest)
	{
	public:
		
		TEST_METHOD(AssignCompareInteger)
		{
         shared_var v(3);
         Assert::IsTrue(v == 3);
         Assert::IsTrue(v.is<int>());
         Assert::IsFalse(v.empty());
         v = 1;
         Assert::IsTrue(v.as<int>() == 1);
      }

      TEST_METHOD(AssignCompareDouble) {
         shared_var v(3.2);
         Assert::IsTrue(v == 3.2);
         Assert::IsTrue(v.is<double>());
         v = 4.3;
         Assert::IsFalse(v.empty());
      }

      TEST_METHOD(AssignCompareString) {

         shared_var v("hello");
         Assert::IsTrue(v == "hello");
         Assert::IsTrue(v == std::string("hello"));
         v = "yo";
         Assert::IsTrue(v.is<std::string>());
         Assert::IsFalse(v.empty());
      }

      TEST_METHOD(AssignCompareNull) {

         shared_var v(nullptr);
         Assert::IsTrue(v == nullptr);
         Assert::IsTrue(nullptr == v);
         Assert::IsTrue(v.empty());
         v = nullptr;
         Assert::IsTrue(v.empty());
      }

      TEST_METHOD(AssignCopy) {

         shared_var v(20);
         shared_var x = v;

         Assert::IsTrue(20 == x);         
         Assert::IsTrue(x.is<int>());

         Assert::IsTrue(x == v);

         x = 20;

         Assert::IsTrue(x == v);
      }

      TEST_METHOD(VectorOfAnys) {

         std::vector<shared_var> shared_vars;

         shared_vars.push_back(shared_var(2));
         shared_vars.push_back(shared_var("hello"));
         shared_vars.push_back(shared_var(L"wstring"));

         Assert::IsTrue(shared_vars[0].is<int>());
         Assert::IsTrue(shared_vars[1].is<std::string>());
         Assert::IsTrue(shared_vars[2].is<std::wstring>());
      }

      TEST_METHOD(ListOfVars) {
         std::list<shared_var> shared_vars;
         shared_vars.push_back(shared_var(2));
         shared_vars.push_back(shared_var("hello"));
         shared_vars.push_back(shared_var(L"wstring"));

      }

      TEST_METHOD(AsWithDefaultValue) {
         shared_var x(14.0);

         int y = x.as(0);

         Assert::IsTrue(y == 0);
         Assert::IsTrue(x == 14.0);
      }

      TEST_METHOD(Cast) {
         shared_var x(14);
         int y = x.as<int>();

         Assert::IsTrue(y == 14);
      }

      TEST_METHOD(Pair) {

         std::pair<shared_var, shared_var> values = std::make_pair(shared_var(4), shared_var("Hello"));
       
         Assert::IsTrue(values.first.is<int>());
         Assert::IsTrue(values.second.is<std::string>());

         values.first = 42.0;

         Assert::IsTrue(values.first.is<double>());

         Assert::IsTrue(values == std::make_pair(shared_var(42.0), shared_var("Hello")));
      }

      TEST_METHOD(TestStrings) {
         shared_var v1("Hello");
         shared_var v2;

         v2 = v1;

         v2 = "Goodbye";

         Assert::IsTrue("Hello" == v1);
         Assert::IsTrue(v2 == "Goodbye");
      }
      
      TEST_METHOD(MapOfAnys) {

         std::map<std::string, shared_var> obj;
         
         obj["x"] = 4;
         obj["y"] = "Hello";
         obj["z"] = std::vector < bool > { false, true, false };

         Assert::IsTrue(obj["x"] == 4);
         Assert::IsTrue(obj["y"] == "Hello");
         Assert::IsTrue(obj["z"].is<std::vector<bool>>());
      }      
   };
}