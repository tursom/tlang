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

struct Command;

extern std::map<std::string, std::vector<std::shared_ptr<Command>>> userFunc;

char *definingUserFunc();

void setDefiningUserFunc(const char *);

void userFuncDefineEnd(const std::vector<std::shared_ptr<Command>> &funcBody);

enum Type {
	Null,
	Int,
	Double,
	String,
	Bool,
	Stack
};

struct Stack;

union Val {
	Val();
	
	Val(const char *str);
	
	~Val();
	
	void *pVoid = nullptr;
	long i;
	double d;
	std::string str;
	bool b;
	std::shared_ptr<struct Stack> stack;
};

struct SmartPointer {
	SmartPointer() = default;
	
	explicit SmartPointer(std::nullptr_t value) : value(std::make_shared<Val>()), type(Null) {
		this->value->pVoid = value;
	}
	
	explicit SmartPointer(void *value) : value(std::make_shared<Val>()), type(Null) {
		this->value->pVoid = value;
	}
	
	explicit SmartPointer(long value) : value(std::make_shared<Val>()), type(Int) {
		this->value->i = value;
	}
	
	explicit SmartPointer(double value) : value(std::make_shared<Val>()), type(Double) {
		this->value->d = value;
	}
	
	explicit SmartPointer(const std::string &value) : value(std::make_shared<Val>()), type(String) {
		this->value->str = value;
	}
	
	explicit SmartPointer(bool value) : value(std::make_shared<Val>()), type(Bool) {
		this->value->b = value;
	}
	
	explicit SmartPointer(const char *value) : value(std::make_shared<Val>(value)), type(String) {
		this->value->str = std::string(value);
	}
	
	explicit SmartPointer(std::shared_ptr<struct Stack> value)
			: value(std::make_shared<Val>()), type(Stack) {
		this->value->stack = std::move(value);
	}
	
	std::shared_ptr<Val> value;
	Type type{Null};
};

struct Value {
	Value() : pointer(dft) {}
	
	explicit Value(long value) : pointer(std::make_shared<SmartPointer>(value)) {}
	
	explicit Value(double value) : pointer(std::make_shared<SmartPointer>(value)) {}
	
	explicit Value(const std::string &value) : pointer(std::make_shared<SmartPointer>(value)) {}
	
	explicit Value(bool value) : pointer(std::make_shared<SmartPointer>(value)) {}
	
	explicit Value(const char *value) : pointer(std::make_shared<SmartPointer>(value)) {}
	
	explicit Value(void *value) : pointer(std::make_shared<SmartPointer>(value)) {}
	
	Value(std::shared_ptr<struct Stack> shared_ptr) : pointer(std::make_shared<SmartPointer>(shared_ptr)) {}
	
	Value &operator=(const Value value1) {
		pointer = value1.pointer;
		return *this;
	}
	
	std::shared_ptr<Val> &getValue() {
		return pointer->value;
	}
	
	std::shared_ptr<Val> getValue() const {
		return pointer->value;
	}
	
	Type &getType() {
		return pointer->type;
	}
	
	Type getType() const {
		return pointer->type;
	}
	
	static std::shared_ptr<SmartPointer> dft;
	std::shared_ptr<SmartPointer> pointer;
};

struct Stack {
	Value &operator[](int index) {
		return value[top - index - 1];
	}
	
	Value operator[](int index) const {
		return value[top - index - 1];
	}
	
	void push(Value &value1) {
		value[top++] = value1;
	}
	
	Value *pop() {
		if (top <= 0)return nullptr;
		return value + --top;
	}
	
	Value *peek() {
		if (top < 0)return nullptr;
		return value + top;
	}
	
	Value value[128];
	int top = 0;
};

struct Environment {
	Environment() : stack(std::make_shared<struct Stack>()) { // NOLINT(modernize-use-equals-default)
		env["@"] = Value(stack);
	}
	
	Environment(const Environment &env) : stack(env.stack) { // NOLINT(modernize-use-equals-default)
		this->env["@"] = Value(stack);
	}
	
	std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>>, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value>>>::const_iterator
	find(const std::string &key) const {
		return env.find(key);
	}
	
	std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>>, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value>>>::const_iterator
	end() const {
		return env.end();
	}
	
	Value &operator[](const std::string &key) {
		return env[key];
	}
	
	std::shared_ptr<struct Stack> stack;
	std::map<std::string, Value> env;
};

typedef std::map<const std::string, std::function<void(Environment &env, char *brk[128])>> FuncMap;

extern FuncMap funcMap;

class CommandException : public std::exception {
public:
	CommandException() = default;
	
	explicit CommandException(const std::string &message) : message(new std::string(message)) {}
	
	explicit CommandException(const char *message) : message(new std::string(message)) {}
	
	~CommandException() override {
		delete message;
	}
	
	std::string *message = nullptr;
};

void getValue(char *str, const Environment &env, Value &value);

#endif //UNTITLED1_COMMANDS_H
