#ifndef KLuaWrapH
#define KLuaWrapH

/*
KLuaWrap类，本类实现了对lua基本操作的封装，包括
调用脚本中的函数，向Lua中输出对象，以及断点调试
等功能。

使用方法：
定义唯一的 KLuaWrap 对象
1、通过 KLuaWrap::InitScript() 方法完成初始化即可使用
2、通过 KLuaWrap::DoScriptFunc()方法执行lua函数
3、通过 KLuaWrap::LoadScripts() 方法载入lua脚本文件
4、通过 KLuaWrap::doScripts() 方法执行脚本
5、通过 KLuaWrap::releaseScript() 方法释放不用的脚本

6、可通过 [] 访问或设置脚本中的任何值
如：

KLuaWrap lw;
lw.InitScript();

lw["t1"] = TABLE_;
建立了一个名为t1的空表

lw["t1"]["x"] = 100;
lw["t1"]["y"] = 200;
lw["t1"]["name"] = "test";
给表中的x,y,name赋值
如同lua中的 t1={x=100, y=200, name="test"}

7、通过[]访问脚本中的数据:
int x = lw["t1"]["x"];
int y = lw["t1"]["y"];
char* name = lw["t1"]["name"];

8、同过[]调用函数，只能调用全局函数,参数最多5个，返回值一个可任意类型
int r = lw[ts](1,2);
如需多返回值，使用DoScriptFunc方法

9、输出函数到脚本
有函数 int add(int a, int b){ return a+b; }
可如此输出 lw["add"] = add;
然后在脚本中 n = add(100, 200)即可

10、输出对象到脚本的方法：
class CTestLua
{
public:
CTestLua(){ m=0; }
int add(int a, int b)
{
m = a + b;
return m;
}

int m;

BeginDefLuaClass(CTestLua) // 定义类
DefMemberFunc(c)		// 成员函数
DefMemberVar(m)			// 成员变量，在lua中只读
EndDef
};
如上面的类定义

在任何地方执行
lw.RegisterClass<CTestLua>(); 即可在lua中使用CTestLua对象

lua中脚本如下：
t=CTestLua()
t:add(1,2)

可以给全局变量赋予对象值，但是不能给表中的成员赋对象值
给表中的成员赋对象值仅可用于读取，不能在lua中调用对象的成员
如：
CTestLua t;
lw["t1"] = t;

这样在lua中可以使用t1:add()方法

但是
CTestLua t;
lw["tb"] = TABLE_;
lw["tb"]["t1"] = t;
此时，在lua中调用 tb.t1:add()是没有效果的。
但是可以同过 lw["tb"]["t1"] 取出 t 的指针

11、成员函数多返回值方法
注册和声明和普通成员函数一样
只是在函数返回时，先把多个返回值用LuaPush压栈
在将返回值个数压栈，函数返回MULT_RET
如某成员函数
int k()
{

LuaPush(100);
LuaPush(200);
LuaPush(300);
LuaPush("400");
// 以上四行将返回值压栈
LuaPush(4); // 本行将返回值个数压栈
return MULT_RET; // 返回MULT_RET告诉luawrap是多返回值函数
}

*/
#include "pgestd.h"

extern "C"
{
#include "Lua/lauxlib.h"
#include "Lua/lua.h"
#include "Lua/lualib.h"
#include "lstate.h"
}

using namespace std;

#define lwp_error -1
#define lwp_success 0
#define MULT_RET 0x1fff9876
#define TABLE_ 0x1fff9875

namespace LMREG
{
	void	enum_stack(lua_State *L);
	int		on_error(lua_State *L);
	void	print_error(lua_State *L, const char* fmt, ...);

	// dynamic type extention
	struct lua_value
	{
		virtual void to_lua(lua_State *L) = 0;
	};

	// type trait
	template<typename T> struct class_name;
	struct table;

	template<bool C, typename A, typename B> struct if_ {};
	template<typename A, typename B>		struct if_<true, A, B> { typedef A type; };
	template<typename A, typename B>		struct if_<false, A, B> { typedef B type; };

	template<typename A>
	struct is_ptr { static const bool value = false; };
	template<typename A>
	struct is_ptr<A*> { static const bool value = true; };

	template<typename A>
	struct is_ref { static const bool value = false; };
	template<typename A>
	struct is_ref<A&> { static const bool value = true; };

	template<typename A>
	struct remove_const { typedef A type; };
	template<typename A>
	struct remove_const<const A> { typedef A type; };

	template<typename A>
	struct base_type { typedef A type; };
	template<typename A>
	struct base_type<A*> { typedef A type; };
	template<typename A>
	struct base_type<A&> { typedef A type; };

	template<typename A>
	struct class_type { typedef typename remove_const<typename base_type<A>::type>::type type; };

	/////////////////////////////////
	enum { no = 1, yes = 2 }; 
	typedef char (& no_type )[no]; 
	typedef char (& yes_type)[yes]; 

	struct int_conv_type { int_conv_type(int); }; 

	no_type int_conv_tester (...); 
	yes_type int_conv_tester (int_conv_type); 

	no_type vfnd_ptr_tester (const volatile char *); 
	no_type vfnd_ptr_tester (const volatile short *); 
	no_type vfnd_ptr_tester (const volatile int *); 
	no_type vfnd_ptr_tester (const volatile long *); 
	no_type vfnd_ptr_tester (const volatile double *); 
	no_type vfnd_ptr_tester (const volatile float *); 
	no_type vfnd_ptr_tester (const volatile bool *); 
	yes_type vfnd_ptr_tester (const volatile void *); 

	template <typename T> T* add_ptr(T&); 

	template <bool C> struct bool_to_yesno { typedef no_type type; }; 
	template <> struct bool_to_yesno<true> { typedef yes_type type; }; 

	template <typename T> 
	struct is_enum 
	{ 
		static T arg; 
		static const bool value = ( (sizeof(int_conv_tester(arg)) == sizeof(yes_type)) && (sizeof(vfnd_ptr_tester(add_ptr(arg))) == sizeof(yes_type)) ); 
	}; 
	/////////////////////////////////

	// from lua
	template<typename T>
	struct void2val { static T invoke(void* input){ return *(T*)input; } };
	template<typename T>
	struct void2ptr { static T* invoke(void* input){ return (T*)input; } };
	template<typename T>
	struct void2ref { static T& invoke(void* input){ return *(T*)input; } };

	template<typename T>  
	struct void2type
	{
		static T invoke(void* ptr)
		{
			return	if_<is_ptr<T>::value
				,void2ptr<typename base_type<T>::type>
				,typename if_<is_ref<T>::value
				,void2ref<typename base_type<T>::type>
				,void2val<typename base_type<T>::type>
				>::type
			>::type::invoke(ptr);
		}
	};

	struct user
	{
		user(void* p) : m_p(p) {}
		virtual ~user() {}
		void* m_p;
	};

	template<typename T>  
	struct user2type { static T invoke(lua_State *L, int index) { return void2type<T>::invoke(lua_touserdata(L, index)); } };

	template<typename T>
	struct lua2enum { static T invoke(lua_State *L, int index) { return (T)(int)lua_tonumber(L, index); } };

	template<typename T>
	struct lua2object
	{ 
		static T invoke(lua_State *L, int index) 
		{ 
			if(!lua_isuserdata(L,index))
			{
				lua_pushstring(L, "no class at first argument. (forgot ':' expression ?)");
				lua_error(L);
			}
			return void2type<T>::invoke(user2type<user*>::invoke(L,index)->m_p); 
		} 
	};

	template<typename T>
		T lua2type(lua_State *L, int index)
	{
		return	if_<is_enum<T>::value
			,lua2enum<T>
			,lua2object<T> 
		>::type::invoke(L, index);
	}

	template<typename T>
	struct val2user : user
	{
		val2user() : user(new T) {}

		template<typename T1>
			val2user(T1 t1) : user(new T(t1)) {}

			template<typename T1, typename T2>
				val2user(T1 t1, T2 t2) : user(new T(t1, t2)) {}

				template<typename T1, typename T2, typename T3>
					val2user(T1 t1, T2 t2, T3 t3) : user(new T(t1, t2, t3)) {}

					template<typename T1, typename T2, typename T3, typename T4>
						val2user(T1 t1, T2 t2, T3 t3, T4 t4) : user(new T(t1, t2, t3,t4)) {}

						template<typename T1, typename T2, typename T3, typename T4, typename T5>
							val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) : user(new T(t1, t2, t3,t4,t5)) {}

							~val2user() { delete ((T*)m_p); }
	};

	template<typename T>
	struct ptr2user : user
	{
		ptr2user(T* t) : user((void*)t) {}
	};

	template<typename T>
	struct ref2user : user
	{
		ref2user(T& t) : user(&t) {}
	};

	// to lua
	template<typename T>
	struct val2lua { static void invoke(lua_State *L, T& input){ new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(input); } };
	template<typename T>
	struct ptr2lua { static void invoke(lua_State *L, T* input){ if(input) new(lua_newuserdata(L, sizeof(ptr2user<T>))) ptr2user<T>(input); else lua_pushnil(L); } };
	template<typename T>
	struct ref2lua { static void invoke(lua_State *L, T& input){ new(lua_newuserdata(L, sizeof(ref2user<T>))) ref2user<T>(input); } };

	template<typename T>
	struct enum2lua { static void invoke(lua_State *L, T val) { lua_pushnumber(L, (int)val); } };

	template<typename T>
	struct object2lua 
	{ 
		static void invoke(lua_State *L, T val) 
		{ 
			if_<is_ptr<T>::value
				,ptr2lua<typename base_type<T>::type>
				,typename if_<is_ref<T>::value
				,ref2lua<typename base_type<T>::type>
				,val2lua<typename base_type<T>::type>
				>::type
			>::type::invoke(L, val);

			push_meta(L, class_name<typename class_type<T>::type>::name());
			lua_setmetatable(L, -2);
		} 
	};

	template<typename T>
		void type2lua(lua_State *L, T val)
	{
		if_<is_enum<T>::value
			,enum2lua<T>
			,object2lua<T>
		>::type::invoke(L, val);
	}

	// get value from cclosure
	template<typename T>  
		T upvalue_(lua_State *L)
	{
		return user2type<T>::invoke(L, lua_upvalueindex(1));
	}

	// read a value from lua stack 
	template<typename T>  
		T read(lua_State *L, int index)				{ return lua2type<T>(L, index); }

		template<>	char*				read(lua_State *L, int index);
		template<>	const char*			read(lua_State *L, int index);
		template<>	char				read(lua_State *L, int index);
		template<>	unsigned char		read(lua_State *L, int index);
		template<>	short				read(lua_State *L, int index);
		template<>	unsigned short		read(lua_State *L, int index);
		template<>	long				read(lua_State *L, int index);
		template<>	unsigned long		read(lua_State *L, int index);
		template<>	int					read(lua_State *L, int index);
		template<>	unsigned int		read(lua_State *L, int index);
		template<>	float				read(lua_State *L, int index);
		template<>	double				read(lua_State *L, int index);
		template<>	bool				read(lua_State *L, int index);
		template<>	void				read(lua_State *L, int index);
		template<>	__int64			read(lua_State *L, int index);
		template<>	unsigned __int64	read(lua_State *L, int index);
		template<>	table				read(lua_State *L, int index);

		// push a value to lua stack 
		template<typename T>  
			void push(lua_State *L, T ret)					{ type2lua<T>(L, ret); }

			template<>	void push(lua_State *L, char ret);
			template<>	void push(lua_State *L, unsigned char ret);
			template<>	void push(lua_State *L, short ret);
			template<>	void push(lua_State *L, unsigned short ret);
			template<>	void push(lua_State *L, long ret);
			template<>	void push(lua_State *L, unsigned long ret);
			template<>	void push(lua_State *L, int ret);
			template<>	void push(lua_State *L, unsigned int ret);
			template<>	void push(lua_State *L, float ret);
			template<>	void push(lua_State *L, double ret);
			template<>	void push(lua_State *L, char* ret);
			template<>	void push(lua_State *L, const char* ret);
			template<>	void push(lua_State *L, bool ret);
			template<>	void push(lua_State *L, lua_value* ret);
			template<>	void push(lua_State *L, __int64 ret);
			template<>	void push(lua_State *L, unsigned __int64 ret);
			template<>	void push(lua_State *L, table ret);

			// pop a value from lua stack
			template<typename T>  
				T pop(lua_State *L) { T t = read<T>(L, -1); lua_pop(L, 1); return t; }

				template<>	void	pop(lua_State *L);
				template<>	table	pop(lua_State *L);

				// functor
				template<typename RVal, typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void>
				struct functor
				{
					static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5))); return RetValue_(L); }
				};

				template<typename RVal, typename T1, typename T2, typename T3, typename T4>
				struct functor<RVal,T1,T2,T3,T4> 
				{
					static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4))); return RetValue_(L); }
				};

				template<typename RVal, typename T1, typename T2, typename T3>
				struct functor<RVal,T1,T2,T3> 
				{
					static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3))); return RetValue_(L); }
				};

				template<typename RVal, typename T1, typename T2>
				struct functor<RVal,T1,T2> 
				{
					static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2)>(L)(read<T1>(L,1),read<T2>(L,2))); return RetValue_(L); }
				};

				template<typename RVal, typename T1>
				struct functor<RVal,T1> 
				{
					static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1)>(L)(read<T1>(L,1))); return RetValue_(L); }
				};

				template<typename RVal>
				struct functor<RVal>
				{
					static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)()>(L)()); return RetValue_(L); }
				};

				//
				template<typename T1, typename T2, typename T3, typename T4, typename T5>
				struct functor<void, T1, T2, T3, T4, T5>
				{
					static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5)); return 0; }
				};

				template<typename T1, typename T2, typename T3, typename T4>
				struct functor<void, T1, T2, T3, T4>
				{
					static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4)); return 0; }
				};

				template<typename T1, typename T2, typename T3>
				struct functor<void, T1, T2, T3>
				{
					static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3)); return 0; }
				};

				template<typename T1, typename T2>
				struct functor<void, T1, T2>
				{
					static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2)>(L)(read<T1>(L,1),read<T2>(L,2)); return 0; }
				};

				template<typename T1>
				struct functor<void, T1>
				{
					static int invoke(lua_State *L) { upvalue_<void(*)(T1)>(L)(read<T1>(L,1)); return 0; }
				};

				template<>
				struct functor<void>
				{
					static int invoke(lua_State *L) { upvalue_<void(*)()>(L)(); return 0; }
				};

				// push_functor

				template<typename RVal> 
					void push_functor(lua_State *L, RVal* s)
				{

				}


				template<typename RVal> 
					void push_functor(lua_State *L, RVal (*func)())
				{
					lua_pushcclosure(L, functor<RVal>::invoke, 1);
				}

				template<typename RVal, typename T1> 
					void push_functor(lua_State *L, RVal (*func)(T1))
				{ 
					lua_pushcclosure(L, functor<RVal,T1>::invoke, 1);
				}

				template<typename RVal, typename T1, typename T2> 
					void push_functor(lua_State *L, RVal (*func)(T1,T2))
				{ 
					lua_pushcclosure(L, functor<RVal,T1,T2>::invoke, 1);
				}

				template<typename RVal, typename T1, typename T2, typename T3> 
					void push_functor(lua_State *L, RVal (*func)(T1,T2,T3))
				{ 
					lua_pushcclosure(L, functor<RVal,T1,T2,T3>::invoke, 1);
				}

				template<typename RVal, typename T1, typename T2, typename T3, typename T4> 
					void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4))
				{ 
					lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4>::invoke, 1);
				}

				template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5> 
					void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5))
				{ 
					lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5>::invoke, 1);
				}

				// member variable
				struct var_base
				{
					virtual void get(lua_State *L) = 0;
					virtual void set(lua_State *L) = 0;
				};

				template<typename T, typename V>
				struct mem_var : var_base
				{
					V T::*_var;
					mem_var(V T::*val) : _var(val) {}
					void get(lua_State *L)	{ push(L, read<T*>(L,1)->*(_var));		}
					void set(lua_State *L)	{ read<T*>(L,1)->*(_var) = read<V>(L, 3);	}
				};

				int RetValue_(lua_State *L);

				// member function
				template<typename RVal, typename T, typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void>
				struct mem_functor
				{
					static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6))); return RetValue_(L); }
				};

				template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4> 
				struct mem_functor<RVal,T,T1,T2,T3,T4>
				{
					static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5))); return RetValue_(L); }
				};

				template<typename RVal, typename T, typename T1, typename T2, typename T3> 
				struct mem_functor<RVal,T,T1,T2,T3>
				{
					static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4))); return RetValue_(L); }
				};

				template<typename RVal, typename T, typename T1, typename T2> 
				struct mem_functor<RVal,T,T1, T2>
				{
					static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2)>(L))(read<T1>(L,2),read<T2>(L,3))); return RetValue_(L); }
				};

				template<typename RVal, typename T, typename T1> 
				struct mem_functor<RVal,T,T1>
				{
					static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1)>(L))(read<T1>(L,2))); return RetValue_(L); }
				};

				template<typename RVal, typename T> 
				struct mem_functor<RVal,T>
				{
					static int invoke(lua_State *L) 
					{
						push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)()>(L))());
						return RetValue_(L);
					}
				};

				template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
				struct mem_functor<void,T,T1,T2,T3,T4,T5>
				{
					static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6)); return 0; }
				};

				template<typename T, typename T1, typename T2, typename T3, typename T4>
				struct mem_functor<void,T,T1,T2,T3,T4>
				{
					static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5)); return 0; }
				};

				template<typename T, typename T1, typename T2, typename T3>
				struct mem_functor<void,T,T1,T2,T3>
				{
					static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4)); return 0; }
				};

				template<typename T, typename T1, typename T2>
				struct mem_functor<void,T,T1,T2>
				{
					static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2)>(L))(read<T1>(L,2),read<T2>(L,3)); return 0; }
				};

				template<typename T, typename T1>
				struct mem_functor<void,T,T1>
				{
					static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1)>(L))(read<T1>(L,2)); return 0; }
				};

				template<typename T>
				struct mem_functor<void,T>
				{
					static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)()>(L))(); return 0; }
				};

				// push_functor
				template<typename RVal, typename T>
					void push_functor(lua_State *L, RVal (T::*func)()) 
				{ 
					lua_pushcclosure(L, mem_functor<RVal,T>::invoke, 1); 
				}

				template<typename RVal, typename T>
					void push_functor(lua_State *L, RVal (T::*func)() const) 
				{ 
					lua_pushcclosure(L, mem_functor<RVal,T>::invoke, 1); 
				}

				template<typename RVal, typename T, typename T1>
					void push_functor(lua_State *L, RVal (T::*func)(T1)) 
				{ 
					lua_pushcclosure(L, mem_functor<RVal,T,T1>::invoke, 1); 
				}

				template<typename RVal, typename T, typename T1>
					void push_functor(lua_State *L, RVal (T::*func)(T1) const) 
				{ 
					lua_pushcclosure(L, mem_functor<RVal,T,T1>::invoke, 1); 
				}

				template<typename RVal, typename T, typename T1, typename T2>
					void push_functor(lua_State *L, RVal (T::*func)(T1,T2)) 
				{ 
					lua_pushcclosure(L, mem_functor<RVal,T,T1,T2>::invoke, 1); 
				}

				template<typename RVal, typename T, typename T1, typename T2>
					void push_functor(lua_State *L, RVal (T::*func)(T1,T2) const) 
				{ 
					lua_pushcclosure(L, mem_functor<RVal,T,T1,T2>::invoke, 1); 
				}

				template<typename RVal, typename T, typename T1, typename T2, typename T3>
					void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3)) 
				{ 
					lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3>::invoke, 1); 
				}

				template<typename RVal, typename T, typename T1, typename T2, typename T3>
					void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3) const) 
				{ 
					lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3>::invoke, 1); 
				}

				template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
					void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4)) 
				{ 
					lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4>::invoke, 1); 
				}

				template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
					void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4) const) 
				{ 
					lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4>::invoke, 1); 
				}

				template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
					void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5)) 
				{ 
					lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5>::invoke, 1); 
				}

				template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
					void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5) const) 
				{ 
					lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5>::invoke, 1); 
				}

				// constructor
				template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
					int constructor(lua_State *L) 
				{ 
					new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6));
					push_meta(L, class_name<typename class_type<T>::type>::name());
					lua_setmetatable(L, -2);

					return 1; 
				}

				template<typename T, typename T1, typename T2, typename T3, typename T4>
					int constructor(lua_State *L) 
				{ 
					new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5));
					push_meta(L, class_name<typename class_type<T>::type>::name());
					lua_setmetatable(L, -2);

					return 1; 
				}

				template<typename T, typename T1, typename T2, typename T3>
					int constructor(lua_State *L) 
				{ 
					new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4));
					push_meta(L, class_name<typename class_type<T>::type>::name());
					lua_setmetatable(L, -2);

					return 1; 
				}

				template<typename T, typename T1, typename T2>
					int constructor(lua_State *L) 
				{ 
					new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3));
					push_meta(L, class_name<typename class_type<T>::type>::name());
					lua_setmetatable(L, -2);

					return 1; 
				}

				template<typename T, typename T1>
					int constructor(lua_State *L) 
				{ 
					new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2));
					push_meta(L, class_name<typename class_type<T>::type>::name());
					lua_setmetatable(L, -2);

					return 1; 
				}

				template<typename T>
					int constructor(lua_State *L) 
				{ 
					new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>();
					push_meta(L, class_name<typename class_type<T>::type>::name());
					lua_setmetatable(L, -2);

					return 1; 
				}

				// destroyer
				template<typename T>
					int destroyer(lua_State *L) 
				{ 
					((user*)lua_touserdata(L, 1))->~user();
					return 0;
				}

				// global function
				template<typename F> 
					void def(lua_State* L, const char* name, F func)
				{ 
					lua_pushstring(L, name);
					lua_pushlightuserdata(L, (void*)func);
					push_functor(L, func);
					lua_settable(L, LUA_GLOBALSINDEX);
				}

				// global variable
				template<typename T>
					void set(lua_State* L, const char* name, T object)
				{
					lua_pushstring(L, name);
					push(L, object);
					lua_settable(L, LUA_GLOBALSINDEX);
				}

				template<typename T>
					T get(lua_State* L, const char* name)
				{
					lua_pushstring(L, name);
					lua_gettable(L, LUA_GLOBALSINDEX);
					return pop<T>(L);
				}

				template<typename T>
					void decl(lua_State* L, const char* name, T object)
				{
					set(L, name, object);
				}

				// call
				template<typename RVal>
					RVal call(lua_State* L, const char* name)
				{
					lua_pushcclosure(L, on_error, 0);
					int errfunc = lua_gettop(L);

					lua_pushstring(L, name);
					lua_gettable(L, LUA_GLOBALSINDEX);
					if(lua_isfunction(L,-1))
					{
						if(lua_pcall(L, 0, 1, errfunc) != 0)
						{
							lua_pop(L, 1);
						}
					}
					else
					{
						print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
					}

					lua_remove(L, -2);
					return pop<RVal>(L);
				}

				template<typename RVal, typename T1>
					RVal call(lua_State* L, const char* name, T1 arg)
				{
					lua_pushcclosure(L, on_error, 0);
					int errfunc = lua_gettop(L);

					lua_pushstring(L, name);
					lua_gettable(L, LUA_GLOBALSINDEX);
					if(lua_isfunction(L,-1))
					{
						push(L, arg);
						if(lua_pcall(L, 1, 1, errfunc) != 0)
						{
							lua_pop(L, 1);
						}
					}
					else
					{
						print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
					}

					lua_remove(L, -2);
					return pop<RVal>(L);
				}

				template<typename RVal, typename T1, typename T2>
					RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2)
				{
					lua_pushcclosure(L, on_error, 0);
					int errfunc = lua_gettop(L);

					lua_pushstring(L, name);
					lua_gettable(L, LUA_GLOBALSINDEX);
					if(lua_isfunction(L,-1))
					{
						push(L, arg1);
						push(L, arg2);
						if(lua_pcall(L, 2, 1, errfunc) != 0)
						{
							lua_pop(L, 1);
						}
					}
					else
					{
						print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
					}

					lua_remove(L, -2);
					return pop<RVal>(L);
				}

				template<typename RVal, typename T1, typename T2, typename T3>
					RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3)
				{
					lua_pushcclosure(L, on_error, 0);
					int errfunc = lua_gettop(L);

					lua_pushstring(L, name);
					lua_gettable(L, LUA_GLOBALSINDEX);
					if(lua_isfunction(L,-1))
					{
						push(L, arg1);
						push(L, arg2);
						push(L, arg3);
						if(lua_pcall(L, 3, 1, errfunc) != 0)
						{
							lua_pop(L, 1);
						}
					}
					else
					{
						print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
					}

					lua_remove(L, -2);
					return pop<RVal>(L);
				}

				// class helper
				int meta_get(lua_State *L);
				int meta_set(lua_State *L);
				void push_meta(lua_State *L, const char* name);

				// class init
				template<typename T>
					void class_add(lua_State* L, const char* name) 
				{ 
					class_name<T>::name(name);

					lua_pushstring(L, name);
					lua_newtable(L);

					lua_pushstring(L, "__name");
					lua_pushstring(L, name);
					lua_rawset(L, -3);

					lua_pushstring(L, "__index");
					lua_pushcclosure(L, meta_get, 0);
					lua_rawset(L, -3);

					lua_pushstring(L, "__newindex");
					lua_pushcclosure(L, meta_set, 0);
					lua_rawset(L, -3);

					lua_pushstring(L, "__gc");
					lua_pushcclosure(L, destroyer<T>, 0);
					lua_rawset(L, -3);

					lua_settable(L, LUA_GLOBALSINDEX);
				}

				// Tinker Class Inheritence
				template<typename T, typename P>
					void class_inh(lua_State* L)
				{
					push_meta(L, class_name<T>::name());
					if(lua_istable(L, -1))
					{
						lua_pushstring(L, "__parent");
						push_meta(L, class_name<P>::name());
						lua_rawset(L, -3);
					}
					lua_pop(L, 1);
				}

				// Tinker Class Constructor
				template<typename T, typename F>
					void class_con(lua_State* L,F func)
				{
					push_meta(L, class_name<T>::name());
					if(lua_istable(L, -1))
					{
						lua_newtable(L);
						lua_pushstring(L, "__call");
						lua_pushcclosure(L, func, 0);
						lua_rawset(L, -3);
						lua_setmetatable(L, -2);
					}
					lua_pop(L, 1);
				}

				// Tinker Class Functions
				template<typename T, typename F>
					void class_def(lua_State* L, const char* name, F func) 
				{ 
					push_meta(L, class_name<T>::name());
					if(lua_istable(L, -1))
					{
						lua_pushstring(L, name);
						new(lua_newuserdata(L,sizeof(F))) F(func);
						push_functor(L, func);
						lua_rawset(L, -3);
					}
					lua_pop(L, 1);
				}

				// Tinker Class Variables
				template<typename T, typename BASE, typename VAR>
					void class_mem(lua_State* L, const char* name, VAR BASE::*val) 
				{ 
					push_meta(L, class_name<T>::name());
					if(lua_istable(L, -1))
					{
						lua_pushstring(L, name);
						new(lua_newuserdata(L,sizeof(mem_var<BASE,VAR>))) mem_var<BASE,VAR>(val);
						lua_rawset(L, -3);
					}
					lua_pop(L, 1);
				}

				template<typename T>
				struct class_name
				{
					// global name
					static const char* name(const char* name = NULL)
					{
						static char temp[256] = "";
						if(name) strcpy(temp, name);
						return temp;
					}
				};

				// Table Object on Stack
				struct table_obj
				{
					table_obj(lua_State* L, int index);
					~table_obj();

					void inc_ref();
					void dec_ref();

					bool validate();

					template<typename T>
						void set(const char* name, T object)
					{
						if(validate())
						{
							lua_pushstring(m_L, name);
							push(m_L, object);
							lua_settable(m_L, m_index);
						}
					}

					template<typename T>
						T get(const char* name)
					{
						if(validate())
						{
							lua_pushstring(m_L, name);
							lua_gettable(m_L, m_index);
						}
						else
						{
							lua_pushnil(m_L);
						}

						return pop<T>(m_L);
					}

					lua_State*		m_L;
					int				m_index;
					const void*		m_pointer;
					int				m_ref;
				};

				// Table Object Holder
				struct table
				{
					table(lua_State* L);
					table(lua_State* L, int index);
					table(lua_State* L, const char* name);
					table(const table& input);
					~table();

					template<typename T>
						void set(const char* name, T object)
					{
						m_obj->set(name, object);
					}

					template<typename T>
						T get(const char* name)
					{
						return m_obj->get<T>(name);
					}

					table_obj*		m_obj;
				};

} // namespace LMREG

struct TxName
{
	TxName(char* c = NULL)
	{
		name[0] = 0;
		if (c) strcpy(name, c);
	}
	char name[128];
};

class namestack
{
public:
	namestack(){}
	namestack(lua_State* l):m_l(l){}
	void push(char* c) 
	{
		m_name.push_front(TxName(c));
	}

	//void push(string& c)
	//{
	//	m_name.push_front(c);
	//}

	char* top(char* s = NULL)
	{
		if (!m_name.size()) return NULL;
		list<TxName>::iterator it = m_name.begin();
		if (s) 
		{
			strcpy(s, it->name);
			return s;
		}
		return it->name;
	}

	void pop()
	{
		list<TxName>::iterator it = m_name.begin();
		if (it != m_name.end()) m_name.erase(it);
	}

	void clare()
	{
		m_name.clear();
	}

	bool empty()
	{
		return m_name.empty();
	}
	list<TxName> m_name;
	lua_State* m_l;

};

class LuaCallRst
{
public:
	LuaCallRst( lua_State* l ):m_l(l), m_bGeted(false){}
	~LuaCallRst()
	{
		if (!m_bGeted && m_l) lua_pop(m_l, 1);
	}

	template<typename T>
		operator T ()
	{
		if (m_l)
		{
			m_bGeted = true;
			return LMREG::pop<T>(m_l);
		}
		else
		{
			return (T)0;
		}

	}
	lua_State* m_l;
	bool m_bGeted;
};

class LuaItem
{
public:
	LuaItem(namestack* stk):m_stk(stk){}

	enum null_value
	{
		null_number = 0xffffffff
	};

	LuaItem operator[](const char* ss)
	{
		char* s = (char*)ss;
		m_stk->push(s);
		if (lua_istable(m_stk->m_l, -1))
		{
			lua_pushstring(m_stk->m_l, s);
			lua_gettable(m_stk->m_l, -2); 
		}
		return LuaItem(m_stk);
	}

	LuaItem& operator = (int n)
	{
		lua_pop(m_stk->m_l, 1);
		
		while(!m_stk->empty())
		{
			char* s = m_stk->top();
			lua_pushstring(m_stk->m_l, 
				s);
			m_stk->pop();
		}
		if (n == TABLE_)
			lua_createtable(m_stk->m_l, 0, 0);
		else
		{
			LMREG::push(m_stk->m_l, n);
			lua_settable(m_stk->m_l, LUA_GLOBALSINDEX);
		}
		return (*this);
	}

	template<typename T>
		LuaItem& operator = (T object)
	{
		lua_pop(m_stk->m_l, 1);
		while(!m_stk->empty())
		{
			char* s = m_stk->top();
			lua_pushstring(m_stk->m_l, 
				s);
			m_stk->pop();
		}
#ifdef _WIN32
		if (FindChars(typeid(object).name(), "_cdecl", 0) != -1)
#else
		if ((typeid(object).name())[0]=='F')
#endif
		{
			lua_pushlightuserdata(m_stk->m_l, (void*)object);
			LMREG::push_functor(m_stk->m_l, object);
			lua_settable(m_stk->m_l, LUA_GLOBALSINDEX);
		}
		else
		{
			LMREG::push(m_stk->m_l, object);
			lua_settable(m_stk->m_l, LUA_GLOBALSINDEX);
		}

		return (*this);
	}

	template<typename T>
		operator T ()
	{
		if (m_stk->m_l->status == LUA_YIELD)
		{
			return LuaCallRst(0);
		}
		return LuaCallRst(m_stk->m_l); 
	}

	LuaCallRst operator()()const
	{
		if (m_stk->m_l->status == LUA_YIELD)
		{
			return LuaCallRst(0);
		}
		lua_pop(m_stk->m_l, 1);
		char* s = m_stk->top(); 
		lua_pushcclosure(m_stk->m_l, LMREG::on_error, 0); 
		int errfunc = lua_gettop(m_stk->m_l); 
		lua_pushstring(m_stk->m_l, s); 
		lua_gettable(m_stk->m_l, LUA_GLOBALSINDEX); 
		if(lua_isfunction(m_stk->m_l,-1)) 
		{ 
			if(lua_pcall(m_stk->m_l, 0, 1, errfunc) != 0)
			{ 
				lua_pop(m_stk->m_l, 1);  
			} 
		} 
		lua_remove(m_stk->m_l, -2); 
		return LuaCallRst(m_stk->m_l); 
	}

	BOOL IsFunction()
	{
		lua_pop(m_stk->m_l, 1);
		char* s = m_stk->top(); 
		lua_pushcclosure(m_stk->m_l, LMREG::on_error, 0); 
		int errfunc = lua_gettop(m_stk->m_l); 
		lua_pushstring(m_stk->m_l, s); 
		lua_gettable(m_stk->m_l, LUA_GLOBALSINDEX); 
		return lua_isfunction(m_stk->m_l,-1);

	}

	template<class T1>
		LuaCallRst operator()(const T1& a1)const
	{
		if (m_stk->m_l->status == LUA_YIELD)
		{
			return LuaCallRst(0);
		}
		lua_pop(m_stk->m_l, 1);
		char* s = m_stk->top(); 
		lua_pushcclosure(m_stk->m_l, LMREG::on_error, 0); 
		int errfunc = lua_gettop(m_stk->m_l); 
		lua_pushstring(m_stk->m_l, s); 
		lua_gettable(m_stk->m_l, LUA_GLOBALSINDEX); 
		if(lua_isfunction(m_stk->m_l,-1)) 
		{ 
			LMREG::push(m_stk->m_l, a1);
			if(lua_pcall(m_stk->m_l, 1, 1, errfunc) != 0)
			{ 
				lua_pop(m_stk->m_l, 1);  
			} 
		} 
		lua_remove(m_stk->m_l, -2); 
		return LuaCallRst(m_stk->m_l); 
	}

	template<class T1,class T2>
		LuaCallRst operator()(const T1& a1,const T2& a2)const
	{
		if (m_stk->m_l->status == LUA_YIELD)
		{
			return LuaCallRst(0);
		}
		lua_pop(m_stk->m_l, 1);
		char* s = m_stk->top(); 
		lua_pushcclosure(m_stk->m_l, LMREG::on_error, 0); 
		int errfunc = lua_gettop(m_stk->m_l); 
		lua_pushstring(m_stk->m_l, s); 
		lua_gettable(m_stk->m_l, LUA_GLOBALSINDEX); 
		if(lua_isfunction(m_stk->m_l,-1)) 
		{ 
			LMREG::push(m_stk->m_l, a1);
			LMREG::push(m_stk->m_l, a2);
			if(lua_pcall(m_stk->m_l, 2, 1, errfunc) != 0)
			{ 
				lua_pop(m_stk->m_l, 1);  
			} 
		} 
		lua_remove(m_stk->m_l, -2); 
		return LuaCallRst(m_stk->m_l); 
	}

	template<class T1,class T2, class T3>
		LuaCallRst operator()(const T1& a1,const T2& a2, const T3& a3)const
	{
		if (m_stk->m_l->status == LUA_YIELD)
		{
			return LuaCallRst(0);
		}
		lua_pop(m_stk->m_l, 1);
		char* s = m_stk->top(); 
		lua_pushcclosure(m_stk->m_l, LMREG::on_error, 0); 
		int errfunc = lua_gettop(m_stk->m_l); 
		lua_pushstring(m_stk->m_l, s); 
		lua_gettable(m_stk->m_l, LUA_GLOBALSINDEX); 
		if(lua_isfunction(m_stk->m_l,-1)) 
		{ 
			LMREG::push(m_stk->m_l, a1);
			LMREG::push(m_stk->m_l, a2);
			LMREG::push(m_stk->m_l, a3);
			if(lua_pcall(m_stk->m_l, 3, 1, errfunc) != 0)
			{ 
				lua_pop(m_stk->m_l, 1);  
			} 
		} 
		lua_remove(m_stk->m_l, -2); 
		return LuaCallRst(m_stk->m_l); 
	}

	template<class T1,class T2, class T3, class T4>
		LuaCallRst operator()(const T1& a1,const T2& a2,const T3& a3, const T4& a4)const
	{
		if (m_stk->m_l->status == LUA_YIELD)
		{
			return LuaCallRst(0);
		}
		lua_pop(m_stk->m_l, 1);
		char* s = m_stk->top(); 
		lua_pushcclosure(m_stk->m_l, LMREG::on_error, 0); 
		int errfunc = lua_gettop(m_stk->m_l); 
		lua_pushstring(m_stk->m_l, s); 
		lua_gettable(m_stk->m_l, LUA_GLOBALSINDEX); 
		if(lua_isfunction(m_stk->m_l,-1)) 
		{ 
			LMREG::push(m_stk->m_l, a1);
			LMREG::push(m_stk->m_l, a2);
			LMREG::push(m_stk->m_l, a3);
			LMREG::push(m_stk->m_l, a4);
			if(lua_pcall(m_stk->m_l, 4, 1, errfunc) != 0)
			{ 
				lua_pop(m_stk->m_l, 1);  
			} 
		} 
		lua_remove(m_stk->m_l, -2); 
		return LuaCallRst(m_stk->m_l); 
	}

	template<class T1,class T2, class T3, class T4,class T5>
		LuaCallRst operator()(const T1& a1,const T2& a2,const T3& a3, const T4& a4, const T5& a5)const
	{
		if (m_stk->m_l->status == LUA_YIELD)
		{
			return LuaCallRst(0);
		}
		lua_pop(m_stk->m_l, 1);
		char* s = m_stk->top(); 
		lua_pushcclosure(m_stk->m_l, LMREG::on_error, 0); 
		int errfunc = lua_gettop(m_stk->m_l); 
		lua_pushstring(m_stk->m_l, s); 
		lua_gettable(m_stk->m_l, LUA_GLOBALSINDEX); 
		if(lua_isfunction(m_stk->m_l,-1)) 
		{ 
			LMREG::push(m_stk->m_l, a1);
			LMREG::push(m_stk->m_l, a2);
			LMREG::push(m_stk->m_l, a3);
			LMREG::push(m_stk->m_l, a4);
			LMREG::push(m_stk->m_l, a5);
			if(lua_pcall(m_stk->m_l, 5, 1, errfunc) != 0)
			{ 
				lua_pop(m_stk->m_l, 1);  
			} 
		} 
		lua_remove(m_stk->m_l, -2); 
		return LuaCallRst(m_stk->m_l); 
	}

	namestack* m_stk;
};

class KLuaWrap //: public   KSingleton<KLuaWrap>
{
public:

	KLuaWrap(void);
	~KLuaWrap(void);

	// 初始化Lua
	int InitScript(int bDebugMode = 1);
	// 释放Lua
	void Destory();

	// 调用脚本中的函数，file是要调用的脚本名，值可为空
	// func是函数名，不可为空
	// fmt为参数列表顺序，s表示字符串、d表示整数、f表示浮点数、>表示返回值开始
	// 使用方法如：DoScriptFunc("d:\\a.lua", "test", "sdd>d", "a", 3, 5, &x);
	// 表示，调用脚本"d:\\a.lua"中的函数test,参数为"a"、3、5，返回值存到x中
	int DoScriptFunc(char* file, char* func, char* fmt, ...);

	// 执行一个脚本文件
	int doScripts(char* name);
	int doDirScripts(char* dir);
	// 将一个脚本文件载入内存，但不执行
	int LoadScripts(char* name);

	// 执行一段文本
	int doString(char* script);
	// 将文本调用内存，但不执行
	int LoadString(char* script);

	// 释放掉不用的脚本，必须保证此脚本中的函数不再被调用，
	// 否则可能会出现运行时错误
	void releaseScript(char* name);
	void releaseScript(int id);

	// 脚本是否已经被载入内存
	bool isScriptLoaded(char* name);

	// 重载[]操作符，用于提取lua变量
	LuaItem operator[](char* s);


	// 外部调试
#ifdef _WIN32
	void ExportDebugOn();
	void ExportDebugOff();
	BOOL m_bDebugOn;
	void OnPeekDebugMsg();
	DWORD m_dwDebugThId;
	HANDLE m_hDebugTh;
#endif

	// 注册对象
	template<typename T>
		void RegisterClass()
	{
		T::luaRegister(m_l);
	}

	static KLuaWrap* GetInstance()
	{
		static KLuaWrap* klw = NULL;
		if (!klw) klw = new KLuaWrap();
		return klw;
	}

	namestack m_stk;
	lua_State* m_l;
	map<DWORD, DWORD> m_mpScripts;
	DWORD m_dwScripts;
	const char* _LastError;
};

///////////////////定义lua类
#define BeginDefLuaClass(c) \
	static void luaRegister(lua_State* L) \
{ \
	typedef c cls; \
	LMREG::class_add<cls>(L, #c); \
	LMREG::class_con<cls>(L,LMREG::constructor<cls>); \

#define DefMemberFunc(f) \
	LMREG::class_def<cls>(L, #f, &cls::f); \

#define DefMemberVar(v) \
	LMREG::class_mem<cls>(L, #v, &cls::v); \

#define InheritFrom(bc) \
	LMREG::class_inh<cls, bc>(L); \


///////////////////////使用别名定义lua类
#define BeginDefLuaClassA(c, nm) \
	static void luaRegister(lua_State* L) \
{ \
	typedef c cls; \
	LMREG::class_add<cls>(L, #nm); \
	LMREG::class_con<cls>(L,LMREG::constructor<cls>); \

#define DefMemberFuncA(f, nm) \
	LMREG::class_def<cls>(L, #nm, &cls::f); \

#define DefMemberVarA(v, nm) \
	LMREG::class_mem<cls>(L, #nm, &cls::v); \

#define EndDef }
/////////////////////////////////////////

#define LuaPush(x) LMREG::push(LuaWraper.m_l, x)
#define LuaPop() lua_pop(LuaWraper.m_l, 1)
#define LuaRead() LMREG::pop(LuaWraper.m_l)

#define LuaWraper (*(KLuaWrap::GetInstance()))

#endif
