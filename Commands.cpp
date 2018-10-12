//
// Created by tursom on 18-10-11.
//

#include <sys/time.h>
#include <iostream>
#include <cstring>
#include "Commands.h"

using namespace std;

std::shared_ptr<SmartPointer>Value::dft = make_shared<SmartPointer>(nullptr);

struct Double {
	double value;
};

struct Int {
	long value;
};

typedef std::map<
		std::string,
		std::function<Value(
				Environment &env,
				char *brk[])> > CalcMap;

/**
 *
 * @param value
 * @param res
 * @param buffer lager than 20 without stack
 * @param bufferSize
 */
void toCString(const Value &value, const char **res, char *buffer) {
	static char True[] = "true", False[] = "false", space[] = "";
	switch (value.getType()) {
		case Int:
			sprintf(buffer, "%d", value.getValue()->i);
			*res = buffer;
			break;
		case Double:
			sprintf(buffer, "%f", value.getValue()->d);
			*res = buffer;
			break;
		case String:
			*res = value.getValue()->str->c_str();
			break;
		case Bool:
			*res = value.getValue()->b ? True : False;
			break;
//		case CString:
//			*res = value.getValue()->str->c_str();
//			break;
		default:
			*res = space;
			break;
		case Null:
			break;
		case Stack:
			break;
	}
}

void toString(const Value &value, string &res) {
	static char buffer[20];
	static char True[] = "true", False[] = "false", space[] = "";
	switch (value.getType()) {
		case Int:
			sprintf(buffer, "%li", value.getValue()->i);
			res = buffer;
			break;
		case Double:
			sprintf(buffer, "%f", value.getValue()->d);
			res = buffer;
			break;
		case String:
			res = *value.getValue()->str;
			break;
		case Bool: {
			res = value.getValue()->b ? True : False;
		}
			break;
//		case CString:
//			res = value.getValue()->str;
//			break;
		default:
			res = space;
			break;
	}
}

void getValue(char *str, const Environment &env, Value &value) {
	static char buffer[20];
	switch (str[0]) {
		case '$': {
			auto res = env.find(str + 1);
			if (res != env.end()) {
				value = res->second;
			} else {
				value = Value(str);
			}
		}
			break;
		case '@': {
			auto res1 = env.find(str + 1);
			if (res1 != env.end()) {
				const char *key = nullptr;
				toCString(res1->second, &key, buffer);
				auto res2 = env.find(key);
				if (res2 != env.end()) {
					value = res2->second;
				} else {
					value = Value(str);
				}
			} else {
				value = Value(str);
			}
		}
			break;
		case '"':
			value = Value(str + 1);
			break;
		case '\'': {
			int strIndex = 0;
			int index;
			for (index = 0; str[index] != 0; ++index) {
				str[strIndex] = str[index];
				if (str[index] == '\\' && str[index + 1] == ' ') {
					str[strIndex] = ' ';
					++index;
				}
				++strIndex;
			}
			str[strIndex] = str[index];
		}
			value = Value(str);
			break;
		default:
			value = Value(str);
			break;
	}
}

void getFloatOrNull(char *valueName, Environment &env, struct Double &ans) {
	static Value value;
	static char *str;
	getValue(valueName, env, value);
	switch (value.getType()) {
		case Int:
			ans.value = value.getValue()->i;
			break;
		case Double:
			ans.value = value.getValue()->d;
			break;
		case String:
			ans.value = strtod(value.getValue()->str->c_str(), &str);
			if (ans.value == 0.0 && *value.getValue()->str == str)
				throw CommandException("无法将变量转换到Double");
			break;
		case Bool:
			ans.value = value.getValue()->b;
			break;
//		case CString:
//			ans.value = strtod((char *) value.getValue(), &str);
//			if (ans.value == 0.0 && strlen((char *) value.getValue()) == strlen(str))
//				throw CommandException("无法将变量转换到Double");
//			break;
		default:
			break;
	}
}

void getIntOrNull(char *valueName, Environment &env, struct Int &ans) {
	static Value value;
	getValue(valueName, env, value);
	switch (value.getType()) {
		case Int:
			ans.value = value.getValue()->i;
			break;
		case Double:
			ans.value = static_cast<int>(value.getValue()->d);
			break;
		case String: {
			ans.value = 0;
			char positive = 2;
			int index = 0;
			for (auto c: *value.getValue()->str) {
				if (c >= '0' && c <= '9') {
					ans.value *= 10;
					ans.value += c - '0';
				} else if (index == 0) {
					switch (c) {
						case '-':
							positive = 0;
							break;
						case '+':
							positive = 1;
							break;
						default:
							throw CommandException("无法将变量转换到Int");
						
					}
				} else {
					if (index == 1 && positive == 1)throw CommandException("无法将变量转换到Int");
					break;
				}
				++index;
			}
			if (positive == 0)ans.value = -ans.value;
		}
			break;
		case Bool:
			ans.value = value.getValue()->b;
			break;
//		case CString: {
//			ans.value = 0;
//			char *string1 = (char *) value.getValue();
//			char c;
//			char positive = 2;
//			for (int i = 0; (c = string1[i]) != 0; i++) {
//				if (c >= '0' && c <= '9') {
//					ans.value *= 10;
//					ans.value += c - '0';
//				} else if (i == 0) {
//					switch (c) {
//						case '-':
//							positive = 0;
//							break;
//						case '+':
//							positive = 1;
//							break;
//						default:
//							throw CommandException("无法将变量转换到Int");
//
//					}
//				} else {
//					if (i == 1 && positive == 1)throw CommandException("无法将变量转换到Int");
//					break;
//				}
//			}
//			if (positive == 0)ans.value = -ans.value;
//		}
//			break;
		default:
			break;
	}
}

Value add(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足");
	static struct Double d1{}, d2{};
	getFloatOrNull(brk[0], env, d1);
	getFloatOrNull(brk[1], env, d2);
	return Value(d1.value + d2.value);
}

Value intAdd(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足");
	static struct Int i1{}, i2{};
	getIntOrNull(brk[0], env, i1);
	getIntOrNull(brk[1], env, i2);
	return Value(i1.value + i2.value);
}

Value sub(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足");
	static struct Double d1{}, d2{};
	getFloatOrNull(brk[0], env, d1);
	getFloatOrNull(brk[1], env, d2);
	return Value(d1.value - d2.value);
}

Value intSub(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足");
	static struct Int i1{}, i2{};
	getIntOrNull(brk[0], env, i1);
	getIntOrNull(brk[1], env, i2);
	return Value(i1.value - i2.value);
}

Value mul(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足");
	static struct Double d1{}, d2{};
	getFloatOrNull(brk[0], env, d1);
	getFloatOrNull(brk[1], env, d2);
	return Value(d1.value * d2.value);
}

Value intMul(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足");
	static struct Int i1{}, i2{};
	getIntOrNull(brk[0], env, i1);
	getIntOrNull(brk[1], env, i2);
	return Value(i1.value * i2.value);
}

Value ddiv(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足");
	static struct Double d1{}, d2{};
	getFloatOrNull(brk[0], env, d1);
	getFloatOrNull(brk[1], env, d2);
	if (d2.value == 0.0) throw CommandException("被除数为0");
	return Value(d1.value / d2.value);
}

Value intDiv(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足");
	static struct Int i1{}, i2{};
	getIntOrNull(brk[0], env, i1);
	getIntOrNull(brk[1], env, i2);
	if (i2.value == 0) throw CommandException("被除数为0");
	return Value(i1.value / i2.value);
}

CalcMap calcMap{
		{"add",    add},
		{"+",      add},
		{"intAdd", intAdd},
		{"i+",     intAdd},
		{"sub",    sub},
		{"-",      sub},
		{"intSub", intSub},
		{"i-",     intSub},
		{"mul",    mul},
		{"*",      mul},
		{"intMul", intMul},
		{"i*",     intMul},
		{"mul",    ddiv},
		{"*",      ddiv},
		{"intDiv", intDiv},
		{"i/",     intDiv},
};

void echo(Environment &env, char *brk[]) {
	static char buffer[20];
	static const char *str = nullptr;
	Value value;
	for (size_t index = 0; brk[index] != nullptr; index++) {
		getValue(brk[index], env, value);
		if (value.getType() != Stack) {
			toCString(value, &str, buffer);
			printf("%s ", str);
		} else {
			auto stack = *value.getValue()->stack;
			printf("[ ");
			for (int i = 0; i < stack.top; ++i) {
				toCString(stack.value[i], &str, buffer);
				printf("%s ", str);
			}
			printf("]");
		}
	}
	printf("\n");
}

void set(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足");
	static char buffer[20];
	static const char *str = nullptr;
	static Value value;
	char *target = brk[0];
	char *arg0 = brk[1];
	if (!((target[0] == '@' && target[1] == 0)
	      || strcmp("_STACK_", target) == 0
	      || strcmp("_PARENT_", target) == 0)) {
		auto func = calcMap.find(arg0);
		if (func != calcMap.end()) {
			Value res = func->second(env, brk + 2);
			if (res.getType() != Null && res.getValue() != nullptr) {
				env[target] = res;
			}
		} else {
			getValue(arg0, env, value);
			toCString(value, &str, buffer);
			env[target] = Value(str);
		}
	}
}

void stack_push(Environment &env, char *brk[], struct Stack &stack) {
	if (brk[0] == nullptr)throw CommandException("操作数不足");
	for (int i = 0; brk[i] != nullptr; ++i) {
		Value value;
		getValue(brk[i], env, value);
		stack.push(value);
	}
}

void stack_pop(Environment &env, char *brk[], struct Stack &stack) {
	if (brk[0] == nullptr)throw CommandException("操作数不足");
	Value target, *ans;
	string str;
	getValue(brk[0], env, target);
	toString(target, str);
	ans = stack.pop();
	if (ans != nullptr)
		env[str] = *ans;
}

void stack_peek(Environment &env, char *brk[], struct Stack &stack) {
	if (brk[0] == nullptr)throw CommandException("操作数不足");
	static char buffer[20];
	Value target, *ans;
	string str;
	for (int i = 0; brk[i] != nullptr; ++i) {
		getValue(brk[0], env, target);
		toString(target, str);
		ans = stack.peek();
		if (ans != nullptr)
			env[str] = *ans;
	}
}

void stack_empty(Environment &env, char *brk[], struct Stack &stack) {}

void stack_search(Environment &env, char *brk[], struct Stack &stack) {}


typedef std::map<std::string, std::function<void(Environment &env, char *brk[], struct Stack &stack)>> StackFuncMap;
StackFuncMap stackFuncMap{
		{"push", stack_push},
		{"pop",  stack_pop},
		{"peek", stack_peek}
};

void stack(Environment &env, char *brk[]) {
	Value target, *ans;
	string str;
	getValue(brk[0], env, target);
	toString(target, str);
	auto stack = env.find(str);
	if (stack != env.end() && stack->second.getType() == Stack) {
		auto func = stackFuncMap.find(brk[0]);
		if (func != stackFuncMap.end()) {
			func->second(env, brk + 1, *stack->second.getValue()->stack);
		} else {
			throw CommandException("未定义操作");
		}
	} else {
		throw CommandException("无法找到栈");
	}
}

void stdStack(Environment &env, char *brk[]) {
	if (brk[0] == nullptr)throw CommandException("操作数不足");
	auto func = stackFuncMap.find(brk[0]);
	if (func != stackFuncMap.end()) {
		func->second(env, brk + 1, *env.stack);
	} else {
		throw CommandException("未定义操作");
	}
}


void getCurrentTime(Environment &env, char *brk[]) {
	if (brk[0] == nullptr)throw CommandException("操作数不足");
	struct timeval tv{};
	gettimeofday(&tv, nullptr);
	Value target;
	string str;
	getValue(brk[0], env, target);
	toString(target, str);
	env[str] = Value(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

FuncMap funcMap{
		{"echo",  echo},
		{"set",   set},
		{"stack", stack},
		{"@",     stdStack},
};


Val::Val() {
	pVoid = nullptr;
}

Val::~Val() {
}
