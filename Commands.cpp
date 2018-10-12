//
// Created by tursom on 18-10-11.
//

#include <iostream>
#include <cstring>
#include "Commands.h"

using namespace std;

struct Double {
	double value;
};

struct Int {
	int value;
};

typedef std::map<
		std::string,
		std::function<Value(
				Environment &env,
				char *command,
				size_t brk[128],
				size_t brkIndex)> > CalcMap;

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
		case Int: {
			sprintf(buffer, "%d", *(int *) value.getValue());
			*res = buffer;
		}
			break;
		case Double: {
			sprintf(buffer, "%f", *(double *) value.getValue());
			*res = buffer;
		}
			break;
		case String:
			*res = ((string *) value.getValue())->c_str();
			break;
		case Bool: {
			*res = *(bool *) value.getValue() ? True : False;
		}
			break;
		case CString:
			*res = (char *) value.getValue();
			break;
		default:
			*res = space;
			break;
	}
}

/**
 *
 * @param value
 * @param res
 * @param buffer lager than 20
 * @param bufferSize
 */
void toString(const Value &value, string &res) {
	static char buffer[20];
	static char True[] = "true", False[] = "false", space[] = "";
	switch (value.getType()) {
		case Int: {
			sprintf(buffer, "%d", *(int *) value.getValue());
			res = buffer;
		}
			break;
		case Double: {
			sprintf(buffer, "%f", *(double *) value.getValue());
			res = buffer;
		}
			break;
		case String:
			res = *(string *) value.getValue();
			break;
		case Bool: {
			res = *(bool *) value.getValue() ? True : False;
		}
			break;
		case CString:
			res = (char *) value.getValue();
			break;
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
				value.getValue() = str;
				value.getType() = CString;
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
					value.getValue() = str;
					value.getType() = CString;
				}
			} else {
				value.getValue() = str;
				value.getType() = CString;
			}
		}
			break;
		case '"':
			value.getValue() = str + 1;
			value.getType() = CString;
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
			value.getValue() = str;
			value.getType() = CString;
			break;
		default:
			value.getValue() = str;
			value.getType() = CString;
			break;
	}
}

void getFloatOrNull(char *valueName, Environment &env, struct Double &ans) {
	static Value value;
	static char *str;
	getValue(valueName, env, value);
	switch (value.getType()) {
		case Int:
			ans.value = *(int *) value.getValue();
			break;
		case Double:
			ans.value = *(double *) value.getValue();
			break;
		case String:
			ans.value = strtod(((string *) value.getValue())->c_str(), &str);
			if (ans.value == 0.0 && strcmp((char *) value.getValue(), str) == 0)
				throw CommandException("无法将变量转换到Double");
			break;
		case Bool:
			ans.value = *(bool *) value.getValue();
			break;
		case CString:
			ans.value = strtod((char *) value.getValue(), &str);
			if (ans.value == 0.0 && strlen((char *) value.getValue()) == strlen(str))
				throw CommandException("无法将变量转换到Double");
			break;
		default:
			break;
	}
}

void getIntOrNull(char *valueName, Environment &env, struct Int &ans) {
	static Value value;
	getValue(valueName, env, value);
	switch (value.getType()) {
		case Int:
			ans.value = *(int *) value.getValue();
			break;
		case Double:
			ans.value = static_cast<int>(*(double *) value.getValue());
			break;
		case String: {
			ans.value = 0;
			char positive = 2;
			int index = 0;
			for (auto c:*(string *) value.getValue()) {
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
			ans.value = *(bool *) value.getValue();
			break;
		case CString: {
			ans.value = 0;
			char *string1 = (char *) value.getValue();
			char c;
			char positive = 2;
			for (int i = 0; (c = string1[i]) != 0; i++) {
				if (c >= '0' && c <= '9') {
					ans.value *= 10;
					ans.value += c - '0';
				} else if (i == 0) {
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
					if (i == 1 && positive == 1)throw CommandException("无法将变量转换到Int");
					break;
				}
			}
			if (positive == 0)ans.value = -ans.value;
		}
			break;
		default:
			break;
	}
}

Value add(Environment &env, char *command, size_t brk[128], size_t brkIndex) {
	if (brkIndex < 2)throw CommandException("操作数不足");
	static struct Double d1{}, d2{};
	getFloatOrNull(command + brk[0], env, d1);
	getFloatOrNull(command + brk[1], env, d2);
	return Value(new double(d1.value + d2.value), Double);
}

Value intAdd(Environment &env, char *command, size_t brk[128], size_t brkIndex) {
	if (brkIndex < 2)throw CommandException("操作数不足");
	static struct Int i1{}, i2{};
	getIntOrNull(command + brk[0], env, i1);
	getIntOrNull(command + brk[1], env, i2);
	return Value(new int(i1.value + i2.value), Int);
}

Value sub(Environment &env, char *command, size_t brk[128], size_t brkIndex) {
	if (brkIndex < 2)throw CommandException("操作数不足");
	static struct Double d1{}, d2{};
	getFloatOrNull(command + brk[0], env, d1);
	getFloatOrNull(command + brk[1], env, d2);
	return Value(new double(d1.value - d2.value), Double);
}

Value intSub(Environment &env, char *command, size_t brk[128], size_t brkIndex) {
	if (brkIndex < 2)throw CommandException("操作数不足");
	static struct Int i1{}, i2{};
	getIntOrNull(command + brk[0], env, i1);
	getIntOrNull(command + brk[1], env, i2);
	return Value(new int(i1.value - i2.value), Int);
}

Value mul(Environment &env, char *command, size_t brk[128], size_t brkIndex) {
	if (brkIndex < 2)throw CommandException("操作数不足");
	static struct Double d1{}, d2{};
	getFloatOrNull(command + brk[0], env, d1);
	getFloatOrNull(command + brk[1], env, d2);
	return Value(new double(d1.value * d2.value), Double);
}

Value intMul(Environment &env, char *command, size_t brk[128], size_t brkIndex) {
	if (brkIndex < 2)throw CommandException("操作数不足");
	static struct Int i1{}, i2{};
	getIntOrNull(command + brk[0], env, i1);
	getIntOrNull(command + brk[1], env, i2);
	return Value(new int(i1.value * i2.value), Int);
}

Value ddiv(Environment &env, char *command, size_t brk[128], size_t brkIndex) {
	if (brkIndex < 2)throw CommandException("操作数不足");
	static struct Double d1{}, d2{};
	getFloatOrNull(command + brk[0], env, d1);
	getFloatOrNull(command + brk[1], env, d2);
	if (d2.value == 0.0) throw CommandException("被除数为0");
	return Value(new double(d1.value / d2.value), Double);
}

Value intDiv(Environment &env, char *command, size_t brk[128], size_t brkIndex) {
	if (brkIndex < 2)throw CommandException("操作数不足");
	static struct Int i1{}, i2{};
	getIntOrNull(command + brk[0], env, i1);
	getIntOrNull(command + brk[1], env, i2);
	if (i2.value == 0) throw CommandException("被除数为0");
	return Value(new int(i1.value / i2.value), Int);
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

void echo(Environment &env, char *command, size_t brk[128], size_t brkIndex) {
	static char buffer[20];
	static const char *str = nullptr;
	Value value(nullptr, CString);
	for (size_t index = 0; index < brkIndex; index++) {
		getValue(command + brk[index], env, value);
		if (value.getType() != Stack) {
			toCString(value, &str, buffer);
			printf("%s ", str);
		} else {
			auto stack = *(struct Stack *) value.getValue();
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

void set(Environment &env, char *command, size_t brk[128], size_t brkIndex) {
	if (brkIndex < 2)throw CommandException("操作数不足");
	static char buffer[20];
	static const char *str = nullptr;
	static Value value(nullptr, CString);
	if (brkIndex < 2)
		return;
	char *target = command + brk[0];
	char *arg0 = command + brk[1];
	if (!((target[0] == '@' && target[1] == 0)
	      || strcmp("_STACK_", target) == 0
	      || strcmp("_PARENT_", target) == 0)) {
		auto func = calcMap.find(arg0);
		if (func != calcMap.end()) {
			Value res = func->second(env, command, brk + 2, brkIndex - 2);
			if (res.getType() != Null && res.getValue() != nullptr) {
				env[target] = res;
			}
		} else {
			getValue(arg0, env, value);
			toCString(value, &str, buffer);
			env[target] = Value(new string(str), String);
		}
	}
}

void stack_push(Environment &env, char *command, size_t brk[128], size_t brkIndex, struct Stack &stack) {
	if (brkIndex < 1)throw CommandException("操作数不足");
	Value value;
	for (int i = 0; i < brkIndex; ++i) {
		getValue(command + brk[i], env, value);
		if (value.getType() == CString) {
			value = Value(new string((char *) value.getValue()), String);
			stack.push(value);
		} else {
			stack.push(value);
		}
	}
}

void stack_pop(Environment &env, char *command, size_t brk[128], size_t brkIndex, struct Stack &stack) {
	if (brkIndex < 1)throw CommandException("操作数不足");
	static char buffer[20];
	Value target, *ans;
	string str;
	getValue(command + brk[0], env, target);
	toString(target, str);
	ans = stack.pop();
	if (ans != nullptr)
		env[str] = *ans;
}

void stack_peek(Environment &env, char *command, size_t brk[128], size_t brkIndex, struct Stack &stack) {
	if (brkIndex < 1)throw CommandException("操作数不足");
	static char buffer[20];
	Value target, *ans;
	string str;
	for (int i = 0; i < brkIndex; ++i) {
		getValue(command + brk[0], env, target);
		toString(target, str);
		ans = stack.peek();
		if (ans != nullptr)
			env[str] = *ans;
	}
}

void stack_empty(Environment &env, char *command, size_t brk[128], size_t brkIndex, struct Stack &stack) {}

void stack_search(Environment &env, char *command, size_t brk[128], size_t brkIndex, struct Stack &stack) {}


typedef std::map<std::string, std::function<void(Environment &env,
                                                 char *command,
                                                 size_t brk[128],
                                                 size_t brkIndex, struct Stack &stack)>> StackFuncMap;
StackFuncMap stackFuncMap{
		{"push", stack_push},
		{"pop",  stack_pop},
		{"peek", stack_peek}
};

void stack(Environment &env, char *command, size_t brk[128], size_t brkIndex) {
	Value target, *ans;
	string str;
	getValue(command + brk[0], env, target);
	toString(target, str);
	auto stack = env.find(str);
	if (stack != env.end() && stack->second.getType() == Stack) {
		auto func = stackFuncMap.find(command + brk[0]);
		if (func != stackFuncMap.end()) {
			func->second(env, command, brk + 1, brkIndex - 1,
			             *(struct Stack *) stack->second.getValue());
		} else {
			throw CommandException("未定义操作");
		}
	} else {
		throw CommandException("无法找到栈");
	}
}

void stdStack(Environment &env, char *command, size_t brk[128], size_t brkIndex) {
	auto func = stackFuncMap.find(command + brk[0]);
	if (func != stackFuncMap.end()) {
		func->second(env, command, brk + 1, brkIndex - 1, env.stack);
	} else {
		throw CommandException("未定义操作");
	}
}

FuncMap funcMap{
		{"echo",  echo},
		{"set",   set},
		{"stack", stack},
		{"@",     stdStack},
};


Value Stack::operator[](int index) {
	return value[128 - index];
}

SmartPointer::~SmartPointer() {
	switch (type) {
		case Int:
			delete (int *) value;
			break;
		case Double:
			delete (double *) value;
			break;
		case String:
			delete (std::string *) value;
			break;
		case Bool:
			delete (bool *) value;
			break;
		case Stack:
			delete (struct Stack *) value;
			break;
		default:
			break;
	};
}
