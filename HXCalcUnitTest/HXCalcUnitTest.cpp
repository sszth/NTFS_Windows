#include "pch.h"
#include "CppUnitTest.h"
#include"../HXCalc/Calculate.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HXCalcUnitTest
{
	TEST_CLASS(HXCalcUnitTest)
	{
	public:

		Calculate calc;		
		TEST_METHOD(Add)//加
		{
			int a = rand();
			int b = rand();
			int exp = a + b;
			int ret = calc.Add(a, b);
			Assert::AreEqual(exp, ret);
		}

		TEST_METHOD(HXAddHX)//除
		{
			int a = rand();
			int b = rand();
			int exp = a + b;
			int ret = HXAdd(a, b);
			Assert::AreEqual(exp, ret);
		}

		TEST_METHOD(Subtract)//减
		{
			int a = rand();
			int b = rand();
			int exp = a - b;
			int ret = calc.Subtract(a, b);
			Assert::AreEqual(exp, ret);
		}

		TEST_METHOD(Multiply)//乘
		{
			int a = rand();
			int b = rand();
			int exp = a * b;
			int ret = calc.Multiply(a, b);
			Assert::AreEqual(exp, ret);
		}

		TEST_METHOD(Divide)//除
		{
			int a = rand();
			int b = rand();
			int exp = a / b;
			int ret = calc.Divide(a, b);
			Assert::AreEqual(exp, ret);
		}
	};
}
