#include <utility>

//
// Created by tursom on 18-10-11.
//

#include <sys/time.h>
#include <sys/stat.h>
#include <cstring>
#include <cmath>
#include "Commands.h"
#include "CommandReader.h"

using namespace std;

//std::shared_ptr<Value>Value::dft = make_shared<Value>(nullptr);

std::map<std::string, std::vector<std::shared_ptr<Command>>> userFunc;

char *userFuncName = nullptr;

char *definingUserFunc() {
	return userFuncName;
}

void setDefiningUserFunc(const char *funcName) {
	if (userFuncName == nullptr && funcName != nullptr) {
		userFuncName = new char[strlen(funcName) + 1];
		strcpy(userFuncName, funcName);
	}
}

void userFuncDefineEnd(const std::vector<std::shared_ptr<Command>> &funcBody) {
	if (userFuncName != nullptr)
		userFunc[userFuncName] = funcBody;
	delete userFuncName;
	userFuncName = nullptr;
}

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

double Environment::getFloat(char *valueName) const {
	char *str;
	double ret = 0.0;
	Value value = getValue(valueName);
	switch (value.getType()) {
		case Value::Int:
			return value.getInt();
			break;
		case Value::Double:
			return value.getDouble();
			break;
		case Value::String:
			ret = strtod(value.getStr().c_str(), &str);
			if (ret == 0.0 && value.getStr() == str)
				throw CommandException("无法将变量转换到Double", value.getStr());
			return ret;
		case Value::Bool:
			return value.getBool();
			break;
		default:
			return 0.0;
			break;
	}
}

long Environment::getInt(char *valueName) const {
	Value value = getValue(valueName);
	switch (value.getType()) {
		case Value::Int:
			return value.getInt();
			break;
		case Value::Double:
			return static_cast<int>(value.getDouble());
			break;
		case Value::String: {
			long ret = 0;
			char positive = 2;
			int index = 0;
			for (auto c: value.getStr()) {
				if (c >= '0' && c <= '9') {
					ret *= 10;
					ret += c - '0';
				} else if (index == 0) {
					switch (c) {
						case '-':
							positive = 0;
							break;
						case '+':
							positive = 1;
							break;
						default:
							throw CommandException("无法将变量转换到Int", value.getStr());
						
					}
				} else {
					if (index == 1 && positive == 1)
						throw CommandException("无法将变量转换到Int", value.getStr());
					break;
				}
				++index;
			}
			if (positive == 0)ret = -ret;
			return ret;
		}
			break;
		case Value::Bool:
			return value.getBool();
			break;
		default:
			return 0l;
			break;
	}
}

//void getIntOrNull(char *valueName, Environment &env, struct Int &ans) {
//	Value value = env.getValue(valueName);
//	switch (value.getType()) {
//		case Value::Int:
//			ans.value = value.getInt();
//			break;
//		case Value::Double:
//			ans.value = static_cast<int>(value.getDouble());
//			break;
//		case Value::String: {
//			ans.value = 0;
//			char positive = 2;
//			int index = 0;
//			for (auto c: value.getStr()) {
//				if (c >= '0' && c <= '9') {
//					ans.value *= 10;
//					ans.value += c - '0';
//				} else if (index == 0) {
//					switch (c) {
//						case '-':
//							positive = 0;
//							break;
//						case '+':
//							positive = 1;
//							break;
//						default:
//							throw CommandException("无法将变量转换到Int", value.getStr());
//
//					}
//				} else {
//					if (index == 1 && positive == 1)
//						throw CommandException("无法将变量转换到Int", value.getStr());
//					break;
//				}
//				++index;
//			}
//			if (positive == 0)ans.value = -ans.value;
//		}
//			break;
//		case Value::Bool:
//			ans.value = value.getBool();
//			break;
//		default:
//			break;
//	}
//}

Value add(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足", *brk);
	double d1 = env.getFloat(brk[0]);
	double d2 = env.getFloat(brk[1]);
	return Value(d1 + d2);
}

Value intAdd(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足", *brk);
	long i1 = env.getInt(brk[0]);
	long i2 = env.getInt(brk[1]);
	return Value(i1 + i2);
}

Value sub(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足", *brk);
	double d1 = env.getFloat(brk[0]);
	double d2 = env.getFloat(brk[1]);
	return Value(d1 - d2);
}

Value intSub(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足", *brk);
	long i1 = env.getInt(brk[0]);
	long i2 = env.getInt(brk[1]);
	return Value(i1 - i2);
}

Value mul(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足", *brk);
	double d1 = env.getFloat(brk[0]);
	double d2 = env.getFloat(brk[1]);
	return Value(d1 * d2);
}

Value intMul(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足", *brk);
	long i1 = env.getInt(brk[0]);
	long i2 = env.getInt(brk[1]);
	return Value(i1 * i2);
}

Value ddiv(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足", *brk);
	double d1 = env.getFloat(brk[0]);
	double d2 = env.getFloat(brk[1]);
	if (d2 == 0.0) throw CommandException("被除数为0", *brk);
	return Value(d1 / d2);
}

Value intDiv(Environment &env, char *brk[]) {
	if (brk[0] == nullptr || brk[1] == nullptr)throw CommandException("操作数不足", *brk);
	long i1 = env.getInt(brk[0]);
	long i2 = env.getInt(brk[1]);
	if (i2 == 0) throw CommandException("被除数为0", *brk);
	return Value(i1 / i2);
}

Value getCurrentTime(Environment &env, char *brk[]) {
	struct timeval tv{};
	gettimeofday(&tv, nullptr);
	return Value(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

Value getCurrentMicroTime(Environment &env, char *brk[]) {
	struct timeval tv{};
	gettimeofday(&tv, nullptr);
	return Value(tv.tv_sec * 1000 * 1000 + tv.tv_usec);
}

Value _sqrt(Environment &env, char *brk[]) {
	if (brk[0] == nullptr)throw CommandException("操作数不足", *brk);
	return Value(sqrt(env.getFloat(brk[0])));
}

CalcMap calcMap{
		{"add",                    add},
		{"+",                      add},
		{"intAdd",                 intAdd},
		{"i+",                     intAdd},
		{"sub",                    sub},
		{"-",                      sub},
		{"intSub",                 intSub},
		{"i-",                     intSub},
		{"mul",                    mul},
		{"*",                      mul},
		{"intMul",                 intMul},
		{"i*",                     intMul},
		{"mul",                    ddiv},
		{"*",                      ddiv},
		{"intDiv",                 intDiv},
		{"i/",                     intDiv},
		{"systemCurrentTime",      getCurrentTime},
		{"SystemCurrentTime",      getCurrentTime},
		{"SCT",                    getCurrentTime},
		{"systemCurrentMicroTime", getCurrentMicroTime},
		{"SystemCurrentMicroTime", getCurrentMicroTime},
		{"SCMT",                   getCurrentMicroTime},
		{"sqrt",                   _sqrt},
};

void stack_push(Environment &env, char *brk[], struct Stack &stack) {
	if (brk[0] == nullptr)throw CommandException("操作数不足", "push");
	for (int i = 0; brk[i] != nullptr; ++i) {
		Value value = env.getValue(brk[i]);
		stack.push(value);
	}
}

void stack_pop(Environment &env, char *brk[], struct Stack &stack) {
	if (brk[0] == nullptr)throw CommandException("操作数不足", "pop");
	Value target = env.getValue(brk[0]), *ans;
	string str;
	str = target.toString();
	ans = stack.pop();
	if (ans != nullptr)
		env[str] = *ans;
}

void stack_peek(Environment &env, char *brk[], struct Stack &stack) {
	if (brk[0] == nullptr)throw CommandException("操作数不足", "peek");
	static char buffer[20];
	Value target, *ans;
	string str;
	for (int i = 0; brk[i] != nullptr; ++i) {
		target = env.getValue(brk[0]);
		str = target.toString();
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

void echo(Environment &env, char *brk[]) {
	Value value;
	for (size_t index = 0; brk[index] != nullptr; index++) {
		value = env.getValue(brk[index]);
		if (value.getType() != Value::Stack) {
			printf("%s ", value.toCString());
		} else {
			auto stack = value.getStack();
			printf("[ ");
			for (int i = 0; i < stack.size(); ++i) {
				printf("%s ", stack[i].toCString());
			}
			printf("] ");
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
			env[target] = res;
		} else {
			value = env.getValue(arg0);
			env[target] = value;
		}
	}
}

void stack(Environment &env, char *brk[]) {
	Value target;
	string str;
	target = env.getValue(brk[0]);
	str = target.toString();
	auto stackPair = env.find(str);
	if (stackPair != env.end() && stackPair->second.getType() == Value::Stack) {
		auto func = stackFuncMap.find(brk[0]);
		if (func != stackFuncMap.end()) {
			auto stack = stackPair->second;
			func->second(env, brk + 1, stack.getStack());
		} else {
			throw CommandException("未定义操作", brk[1]);
		}
	} else {
		throw CommandException("无法找到栈", *brk);
	}
}

void stdStack(Environment &env, char *brk[]) {
	if (brk[0] == nullptr)throw CommandException("操作数不足");
	auto func = stackFuncMap.find(brk[0]);
	if (func != stackFuncMap.end()) {
		func->second(env, brk + 1, *env.stack);
	} else {
		throw CommandException("未定义操作", *brk);
	}
}

void import(Environment &env, char *brk[]) {
	static struct stat stat1{};
	static char *buffer;
	for (; *brk != nullptr; ++brk) {
		auto file = fopen(*brk, "r");
		if (file != nullptr) {
			if (::stat(*brk, &stat1) < 0) throw CommandException("无法打开文件", *brk);
			auto filesize = static_cast<size_t>(stat1.st_size);
			buffer = new char[filesize + 1];
			fread(buffer, 1, filesize, file);
			buffer[filesize] = 0;
			run(env, buffer);
			delete[] buffer;
			fclose(file);
		} else {
			throw CommandException("无法打开文件", *brk);
		}
	}
}

void exitProgram(Environment &env, char *brk[]) {
	if (brk[0] == nullptr)
		::exit(0);
	else {
		try {
			long i = env.getInt(*brk);
			exit(static_cast<int>(i));
		} catch (CommandException &e) {
			::exit(0);
		}
	}
}

void sleepThread(Environment &env, char *brk[]) {
	if (brk[0] == nullptr)
		return;
	else {
		try {
			long i = env.getInt(*brk);
			timeval tval{};
			tval.tv_sec = i / 1000;
			tval.tv_usec = (i * 1000) % 1000000;
			select(0, nullptr, nullptr, nullptr, &tval);
		} catch (CommandException &e) {
			::exit(0);
		}
	}
}

void usleep(Environment &env, char *brk[]) {
	if (brk[0] == nullptr)
		return;
	else {
		try {
			long i = env.getInt(*brk);
			timeval tval{};
			tval.tv_sec = i / 1000000;
			tval.tv_usec = i % 1000000;
			select(0, nullptr, nullptr, nullptr, &tval);
		} catch (CommandException &e) {
			::exit(0);
		}
	}
}

void func(Environment &env, char *brk[]) {
	if (definingUserFunc() != nullptr)throw CommandException("函数无法嵌套定义!");
	if (*brk == nullptr)throw CommandException("参数不足");
	Value value;
	string str;
	value = env.getValue(*brk);
	str = value.toString();
	setDefiningUserFunc(str.c_str());
}

void call(Environment &env, char *brk[]) {
	if (*brk == nullptr)throw CommandException("参数不足");
	Value value;
	string str;
	value = env.getValue(*brk);
	str = value.toString();
	auto func = userFunc.find(str);
	if (func != userFunc.end()) {
		Environment newEnv(env);
		for (int j = 1; brk[j] != nullptr; ++j) {
			switch (*(brk[j])) {
				case '@': {
					char *arg = brk[j] + 2;
					while (*arg != '=' && *++arg != 0);
					if (*arg == '=')*arg++ = 0;
					value = env.getValue(arg);
					newEnv[brk[j] + 1] = value;
				}
					break;
				case '$':
					newEnv[brk[j] + 1] = env.getValue(brk[j]);
					break;
				case '#':
					value = env.getValue(brk[j]);
					newEnv.stack->push(value);
					break;
				default:
					str = Value((long) j).toString();
					newEnv[str] = Value(brk[j]);
					break;
			}
		}
		for (const auto &i:func->second) {
			auto cmd = *i;
			runCommand(newEnv, cmd.command, cmd.args);
		}
	} else {
		throw CommandException("无法找到函数", *brk);
	}
}

void help(Environment &env, char *brk[]) {
	static const char helpStr[] =
			"欢迎使用Tursom Language (tlang) 系统\n"
			"直接输入命令以调用命令\n"
			"亦可以直接调用用户定义函数\n"
			"系统函数有:"
			" echo"
			" set"
			" stack"
			" @"
			" import"
			" exit"
			" sleep"
			" usleep"
			" func"
			" call\n"
			"输入 help (函数名) 获得帮助";
	static const map<const string, const char *> helpMap = {
			{"set", "给变量赋值\n"
			        "用法: set 变量名 操作数或操作符 (操作符参数)\n"
			        "操作符有"
			        " add"
			        " +"
			        " intAdd"
			        " i+"
			        " sub"
			        " -"
			        " intSub"
			        " i-"
			        " mul"
			        " *"
			        " intMul"
			        " i*"
			        " mul"
			        " *"
			        " intDiv"
			        " i/"
			        " systemCurrentTime"
			        " SystemCurrentTime"
			        " SCT"
			        " systemCurrentMicroTime"
			        " SystemCurrentMicroTime"
			        " SCMT"
			        " sqrt\n"
			        "所有非操作符的操作均视为操作数并转义\n"
			        "转义规则为:\n"
			        "以$开头的操作数视为对变量取值,变量名为$之后的字符串(不含$)\n"
			        "以@开头的操作数视为储存有变量名的变量,其值为其中存储的变量名所对应的变量的值\n"
			        "以\"开头的操作数不进行任何转义,只去掉开头的\"\n"
			        "以'开头的操作数将\\s转义为空格,\\\\转义为\\\n"
			        "以上转义如果失败将会直接返回原始字符串\n"
			        "输入 help set (操作符) 获取更多帮助"},
	};
	if (*brk == nullptr) {
		printf("%s\n", helpStr);
	} else {
		auto str = helpMap.find(*brk);
		if (str != helpMap.end())
			printf("%s\n", str->second);
		else
			throw CommandException("无法找到帮助", *brk);
	}
}

FuncMap funcMap{
		{"echo",   echo},
		{"set",    ::set},
		{"stack",  stack},
		{"@",      stdStack},
		{"import", import},
		{"exit",   exitProgram},
		{"sleep",  sleepThread},
		{"usleep", usleep},
		{"func",   func},
		{"call",   call},
		{"help",   help},
};


Value::ValueUnion::ValueUnion() {
	pVoid = nullptr;
}

Value::ValueUnion::~ValueUnion() {
}


Value &Value::operator=(const Value &value1) = default;

std::string Value::toString() const {
	return value->toString();
}

const char *Value::toCString() const {
	return value->toCString();
}

const std::string &Value::ValueClass::toString() const {
	if (*strFlag)return *str;
	static char buffer[20];
	static char True[] = "true", False[] = "false", space[] = "";
	switch (getType()) {
		case Value::Int:
			sprintf(buffer, "%li", getInt());
			*str = buffer;
			break;
		case Value::Double:
			sprintf(buffer, "%f", getDouble());
			*str = buffer;
			break;
		case Value::String:
			*str = getStr();
			break;
		case Value::Bool:
			*str = getBool() ? True : False;
			break;
		default:
			*str = space;
			break;
	}
	*strFlag = true;
	return *str;
}

const char *Value::ValueClass::toCString() const {
	return toString().c_str();
}

Value Environment::getValue(char *str) const {
	static char buffer[20];
	switch (str[0]) {
		case '$': {
			auto res = env.find(str + 1);
			if (res != env.end()) {
				return res->second;
			} else {
				return Value(str);
			}
		}
			break;
		case '@': {
			auto res1 = env.find(str + 1);
			if (res1 != env.end()) {
				auto res2 = env.find(res1->second.toString());
				if (res2 != env.end()) {
					return res2->second;
				} else {
					return Value(str);
				}
			} else {
				return Value(str);
			}
		}
			break;
		case '"':
			return Value(str + 1);
			break;
		case '\'': {
			int strIndex = 0;
			int index;
			for (index = 1; str[index] != 0; ++index) {
				str[strIndex] = str[index];
				if (str[index] == '\\') {
					switch (str[index + 1]) {
						case 's':
							str[strIndex] = ' ';
							++index;
							break;
						case '\\':
							str[strIndex] = '\\';
							++index;
							break;
						default:
							break;
					}
				}
				++strIndex;
			}
			str[strIndex] = str[index];
		}
			return Value(str);
			break;
		default:
			return Value(str);
			break;
	}
}


Value Environment::getValue(const std::string &valueName) const {
	static char buffer[20];
	switch (valueName[0]) {
		case '$': {
			auto res = env.find(valueName.substr(1));
			if (res != env.end()) {
				return res->second;
			} else {
				return Value(valueName);
			}
		}
			break;
		case '@': {
			auto res1 = env.find(valueName.substr(1));
			if (res1 != env.end()) {
				auto res2 = env.find(res1->second.toString());
				if (res2 != env.end()) {
					return res2->second;
				} else {
					return Value(valueName);
				}
			} else {
				return Value(valueName);
			}
		}
		case '"':
			return Value(valueName.substr(1));
		case '\'': {
			int strIndex = 0;
			int index;
			char *str = new char[valueName.size() + 1];
			strcpy(str, valueName.c_str());
			for (index = 1; str[index] != 0; ++index) {
				str[strIndex] = str[index];
				if (str[index] == '\\') {
					switch (str[index + 1]) {
						case 's':
							str[strIndex] = ' ';
							++index;
							break;
						case '\\':
							str[strIndex] = '\\';
							++index;
							break;
						default:
							break;
					}
				}
				++strIndex;
			}
			str[strIndex] = str[index];
			auto ret = Value(str);
			delete[] str;
			return ret;
		}
		default:
			return Value(valueName);
	}
}


Value::ValueClass::~ValueClass() {
	switch (type) {
		case Null:
			break;
		case Int:
			break;
		case Double:
			break;
		case String:
			value.str.~basic_string();
			break;
		case Bool:
			break;
		case Stack:
			value.stack.~shared_ptr();
			break;
	}
	delete str;
	delete strFlag;
}

struct Stack &Value::ValueClass::getStack() {
	if (type == Stack) {
		*strFlag = false;
		return *value.stack;
	} else throw CommandException("类型转换错误", "Stack");
}

void *Value::ValueClass::getP() const {
	if (type == Null)return value.pVoid;
	else throw CommandException("");
}

long Value::ValueClass::getInt() const {
	if (type == Int)return value.i;
	else throw CommandException("类型转换错误", "Int");
}

double Value::ValueClass::getDouble() const {
	if (type == Double)return value.d;
	else throw CommandException("类型转换错误", "Double");
}

const std::string &Value::ValueClass::getStr() const {
	if (type == String)return value.str;
	else throw CommandException("类型转换错误", "String");
}

bool Value::ValueClass::getBool() const {
	if (type == Bool)return value.b;
	else throw CommandException("类型转换错误", "String");
}

const struct Stack &Value::ValueClass::getStack() const {
	if (type == Stack)return *value.stack;
	else throw CommandException("类型转换错误", "Stack");
}

Value::Type Value::ValueClass::getType() const {
	return type;
}

Value::ValueClass::ValueClass() = default;

Value::ValueClass::ValueClass(std::nullptr_t value) : value(), type(Null) {}

Value::ValueClass::ValueClass(void *value) : value(value), type(Null) {}

Value::ValueClass::ValueClass(long value) : value(value), type(Int) {}

Value::ValueClass::ValueClass(double value) : value(value), type(Double) {}

Value::ValueClass::ValueClass(const std::string &value) : value(value), type(String) {}

Value::ValueClass::ValueClass(bool value) : value(value), type(Bool) {}

Value::ValueClass::ValueClass(const char *value) : value(value), type(String) {}

Value::ValueClass::ValueClass(const std::shared_ptr<struct Stack> &value) : value(value), type(Stack) {}

Value &Stack::operator[](int index) {
	return value[top - index - 1];
}

Value Stack::operator[](int index) const {
	return value[top - index - 1];
}

void Stack::push(Value &value1) {
	value[top++] = value1;
}

Value *Stack::pop() {
	if (top <= 0)return nullptr;
	return value + --top;
}

Value *Stack::peek() {
	if (top < 0)return nullptr;
	return value + top;
}

size_t Stack::size() {
	return top;
}

Environment::Environment() : stack(std::make_shared<struct Stack>()) { // NOLINT(modernize-use-equals-default)
	env["@"] = Value(stack);
}

Environment::Environment(const Environment &env) : stack(env.stack) { // NOLINT(modernize-use-equals-default)
	this->env["@"] = Value(stack);
}

std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>>, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value>>>::const_iterator
Environment::find(const std::string &key) const {
	return env.find(key);
}

std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>>, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value>>>::const_iterator
Environment::end() const {
	return env.end();
}

Value &Environment::operator[](const std::string &key) {
	return env[key];
}

const Value &Environment::operator[](const std::string &key) const {
	auto ret = env.find(key);
	if (ret != env.end())return ret->second;
	else throw CommandException("无法找到元素", key);
}

Value::Value() = default;

Value::Value(std::nullptr_t value) : value(std::make_shared<ValueClass>()) {}

Value::Value(void *value) : value(std::make_shared<ValueClass>(value)) {}

Value::Value(long value) : value(std::make_shared<ValueClass>(value)) {}

Value::Value(double value) : value(std::make_shared<ValueClass>(value)) {}

Value::Value(const std::string &value) : value(std::make_shared<ValueClass>(value)) {}

Value::Value(bool value) : value(std::make_shared<ValueClass>()) {}

Value::Value(const char *value) : value(std::make_shared<ValueClass>(value)) {}

Value::Value(const std::shared_ptr<struct Stack> &value)
		: value(std::make_shared<ValueClass>(value)) {}

void *Value::getP() const {
	return value->getP();
}

long Value::getInt() const {
	return value->getInt();
}

double Value::getDouble() const {
	return value->getDouble();
}

const std::string &Value::getStr() const {
	return value->getStr();
}

bool Value::getBool() const {
	return value->getBool();
}

const struct Stack &Value::getStack() const {
	return value->getStack();
}

struct Stack &Value::getStack() {
	return value->getStack();
}

Value::Type Value::getType() const {
	return value->getType();
}

Value &Value::operator=(const char *value1) {
	value = std::make_shared<ValueClass>(value1);
	return *this;
}


CommandException::CommandException() = default;

CommandException::CommandException(const std::string &message) : message(new std::string(message)) {}

CommandException::CommandException(const std::string &message, const std::string &where)
		: message(new std::string(message)),
		  where(new std::string(where)) {}

CommandException::CommandException(const char *message, const char *where) {
	if (message != nullptr)this->message = new std::string(message);
	if (where != nullptr)this->where = new std::string(where);
}

CommandException::~CommandException() {
	delete message;
	delete where;
}
