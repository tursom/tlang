//
// Created by tursom on 18-10-11.
//

#ifndef UNTITLED1_COMMANDS_H
#define UNTITLED1_COMMANDS_H


#include <string>
#include <map>
#include <functional>


enum Type {
	Null,
	Int,
	Double,
	String,
	Bool,
	CString,
	Stack
};

struct SmartPointer {
	SmartPointer() = default;
	
	SmartPointer(void *value, Type type1) : value(value), type(type1), count(1) {}
	
	~SmartPointer();
	
	void *value = nullptr;
	int count = 1;
	Type type{Null};
};

struct Value {
	Value() : pointer(new SmartPointer(nullptr, Null)) {}
	
	Value(void *value, Type type1) : pointer(new SmartPointer(value, type1)) {}
	
	~Value() {
		if (pointer != nullptr) {
			--pointer->count;
			if (pointer->count == 0)delete pointer;
		}
	}
	
	Value &operator=(const Value value1) {
		if (pointer != nullptr) {
			--pointer->count;
			if (pointer->count == 0)delete pointer;
		}
		pointer = value1.pointer;
		if (pointer != nullptr)++pointer->count;
		return *this;
	}
	
	void *&getValue() {
		return pointer->value;
	}
	
	
	void *getValue() const {
		return pointer->value;
	}
	
	Type &getType() const {
		return pointer->type;
	}
	
	SmartPointer *pointer;
};

struct Stack {
	Value operator[](int index);
	
	void push(Value &value1) {
		value[top++] = value1;
	}
	
	Value *pop() {
		if (top <= 0)return nullptr;
		return value - --top;
	}
	
	Value *peek() {
		if (top < 0)return nullptr;
		return value + top;
	}
	
	Value value[128];
	int top = 0;
};

struct Environment {
	Environment() { // NOLINT(modernize-use-equals-default)
		env["@"] = Value((void *) &stack, Stack);
	}
	
	struct Stack stack;
	std::map<std::string, Value> env;
	
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
};

typedef std::map<std::string, std::function<void(Environment &env,
                                                 char *command,
                                                 size_t brk[128],
                                                 size_t brkIndex)>> FuncMap;

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


#endif //UNTITLED1_COMMANDS_H
