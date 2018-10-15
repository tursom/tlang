#include <utility>

//
// Created by tursom on 18-10-11.
//

#ifndef UNTITLED1_COMMANDS_H
#define UNTITLED1_COMMANDS_H


#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>

/**
 * 用于表示程序异常
 */
class CommandException;

/**
 * 用于表示一个值
 */
class Value;

/**
 * 用于表示一条命令，在CommandReader.h中定义
 */
struct Command;
/**
 * 用于表示一个堆栈，最大长度为128
 */
struct Stack;
/**
 * 用于表示当前程序的运行环境
 * 用于存储变量和标准堆栈
 * 标准堆栈应全局唯一
 */
struct Environment;
/**
 * 用于储存用户自定义函数
 */
extern std::map<std::string, std::vector<std::shared_ptr<Command>>> userFunc;

/**
 * 用户正在自定义的函数
 * @return 函数名，未在自定义函数则返回nullptr
 */
char *definingUserFunc();

/**
 * 设置正在自定义的函数名
 * 如果definingUserFunc不为nullptr则无效
 */
void setDefiningUserFunc(const char *);

/**
 * 结束自定义函数
 * @param funcBody 函数体
 */
void userFuncDefineEnd(const std::vector<std::shared_ptr<Command>> &funcBody);

/**
 * 用于表示解释器执行过程中的异常
 */
class CommandException : public std::exception {
public:
	/**
	 * 默认构造函数
	 */
	CommandException();
	
	/**
	 * @param message 错误信息
	 */
	explicit CommandException(const std::string &message);
	
	/**
	 * @param message 错误信息
	 * @param where 出错位置
	 */
	explicit CommandException(const std::string &message, const std::string &where);
	
	/**
	 * @param message 错误信息
	 * @param where 出错位置
	 */
	explicit CommandException(const char *message, const char *where = nullptr);
	
	/**
	 * 析构函数
	 */
	~CommandException() override;
	
	/**
	 * 错误信息
	 */
	std::string *message = nullptr;
	/**
	 * 出错位置
	 */
	std::string *where = nullptr;
};

/**
 * 用于表示程序运行中的变量的值
 * 不储存变量名
 */
class Value {
public:
	enum Type {
		Null,
		Int,
		Double,
		String,
		Bool,
		Stack
	};
	
	Value();
	
	explicit Value(std::nullptr_t value);
	
	explicit Value(void *value);
	
	explicit Value(long value);
	
	explicit Value(double value);
	
	explicit Value(const std::string &value);
	
	explicit Value(bool value);
	
	explicit Value(const char *value);
	
	explicit Value(const std::shared_ptr<struct Stack> &value);
	
	void *getP() const;
	
	long getInt() const;
	
	double getDouble() const;
	
	const std::string &getStr() const;
	
	bool getBool() const;
	
	const struct Stack &getStack() const;
	
	struct Stack &getStack();
	
	Type getType() const;
	
	Value &operator=(const Value &value1);
	
	Value &operator=(const char *value1);
	
	std::string toString() const;
	
	const char *toCString() const;

private:
	union ValueUnion {
		ValueUnion();
		
		ValueUnion(void *pVoid1) : pVoid(pVoid1) {}
		
		ValueUnion(long i) : i(i) {}
		
		ValueUnion(double d) : d(d) {}
		
		ValueUnion(const char *str) : str(str) {}
		
		ValueUnion(const std::string &str) : str(str) {}
		
		ValueUnion(bool b) : b(b) {}
		
		ValueUnion(const std::shared_ptr<struct Stack> &stack) : stack(stack) {}
		
		~ValueUnion();
		
		void *pVoid = nullptr;
		long i;
		double d;
		std::string str;
		bool b;
		std::shared_ptr<struct Stack> stack;
	};
	
	class ValueClass {
	public:
		ValueClass();
		
		explicit ValueClass(std::nullptr_t value);
		
		explicit ValueClass(void *value);
		
		explicit ValueClass(long value);
		
		explicit ValueClass(double value);
		
		explicit ValueClass(const std::string &value);
		
		explicit ValueClass(bool value);
		
		explicit ValueClass(const char *value);
		
		explicit ValueClass(const std::shared_ptr<struct Stack> &value);
		
		void *getP() const;
		
		long getInt() const;
		
		double getDouble() const;
		
		const std::string &getStr() const;
		
		bool getBool() const;
		
		const struct Stack &getStack() const;
		
		struct Stack &getStack();
		
		Type getType() const;
		
		~ValueClass();
		
		const std::string &toString() const;
		
		const char *toCString() const;
	
	private:
		Type type{Null};
		ValueUnion value;
		std::string *str = new std::string();
		bool *strFlag = new bool(false);
	};
	
	std::shared_ptr<ValueClass> value;
};

struct Stack {
	Value &operator[](int index);
	
	Value operator[](int index) const;
	
	void push(Value &value1);
	
	Value *pop();
	
	Value *peek();
	
	size_t size();

private:
	Value value[128];
	size_t top = 0;
	size_t stack_size;
};

struct Environment {
	Environment();
	
	Environment(const Environment &env);
	
	std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>>, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value>>>::const_iterator
	find(const std::string &key) const;
	
	std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>>, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value>>>::const_iterator
	end() const;
	
	Value &operator[](const std::string &key);
	
	const Value &operator[](const std::string &key) const;
	
	Value getValue(char *str) const;
	
	Value getValue(const std::string &valueName) const;
	
	double getFloat(char *valueName) const;
	
	long getInt(char *valueName) const;
	
	std::shared_ptr<struct Stack> stack;
	std::map<std::string, Value> env;
};

typedef std::map<const std::string, std::function<void(Environment &env, char *brk[128])>> FuncMap;

extern FuncMap funcMap;

//void getValue(char *str, const Environment &env, Value &value);

#endif //UNTITLED1_COMMANDS_H
