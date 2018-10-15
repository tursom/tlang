//
// Created by tursom on 18-10-11.
//

#include "CommandReader.h"
#include <string>
#include <cstring>
#include <iostream>

using namespace std;

const VoidChar voidChar;
std::vector<std::shared_ptr<Command>> funcBody;

string toString(long value) {
	static char buffer[20];
	sprintf(buffer, "%li", value);
	return buffer;
}

void runCommand(Environment &env, const char *command, char *args[128]) {
	while (voidChar.voidChar[*command]);
	for (; *command != 0; ++command) {
		if (voidChar.voidChar[*command])continue;
		else if (*command == 0) return;
		else break;
	}
	if (definingUserFunc() != nullptr) {
		if (strcmp(command, "end") == 0) {
			userFuncDefineEnd(funcBody);
			funcBody = std::vector<std::shared_ptr<Command>>();
		} else if (strcmp(command, "func") == 0) {
			userFuncDefineEnd(funcBody);
			funcBody = std::vector<std::shared_ptr<Command>>();
			runCommand(env, command, args);
		} else {
			Command cmd(command, args);
			funcBody.push_back(make_shared<Command>(cmd));
		}
	} else {
		auto i = funcMap.find(string(command));
		
		if (i != funcMap.end()) {
			try {
				i->second(env, args);
			} catch (CommandException &e) {
				if (e.message != nullptr) {
					if (e.where != nullptr) {
						printf("\033[31m%s: %s\nwhere: %s\n\033[m", command, e.message->c_str(), e.where->c_str());
					} else {
						printf("\033[31m%s: %s\n\033[m", command, e.message->c_str());
					}
				}
			}
		} else {
			auto func = userFunc.find(string(command));
			if (func != userFunc.end()) {
				Value value;
				for (int j = 0; args[j] != nullptr; ++j) {
					switch (*(args[j])) {
						case '@': {
							char *arg = args[j] + 2;
							while (*arg != '=' && *++arg != 0);
							if (*arg == '=')*arg++ = 0;
							value = env.getValue(arg);
							env[args[j] + 1] = value;
						}
							break;
						case '#':
							value = env.getValue(args[j]);
							env.stack->push(value);
							break;
						case '$':
							break;
						default:
							env[toString(j + 1)] = Value(args[j]);
							break;
					}
				}
				for (const auto &cmmd:func->second) {
					auto cmd = *cmmd;
					runCommand(env, cmd.command, cmd.args);
				}
			} else {
				printf("\033[31m%s: 无法运行命令\n\033[m", command);
			}
		}
	}
}

void runLine(Environment &env, char *command) {
	static char *brk[128];
	static char *cmd;
	static char *nxtCmd;
	for (cmd = command; cmd != nullptr && *cmd != 0; cmd = nxtCmd) {
		size_t brkIndex = 0;
		if (!voidChar.voidChar[cmd[0]]) {
			brk[brkIndex++] = cmd;
		}
		for (nxtCmd = cmd; *nxtCmd != 0; ++nxtCmd) {
			if (*nxtCmd < 0) {
				nxtCmd += 2;
				continue;
			}
			if (*nxtCmd == '\\' && nxtCmd[+1] == '\n') {
				*nxtCmd = 0;
				*++nxtCmd = ' ';
			} else if (voidChar.voidChar[*nxtCmd]) {
				if (*nxtCmd == '\n') {
					*nxtCmd = 0;
					++nxtCmd;
					break;
				} else if (!voidChar.voidChar[nxtCmd[1]] &&
				           !(nxtCmd[1] == '\\' && nxtCmd[2] == '\n')
				           && nxtCmd[1] != 0) {
					brk[brkIndex] = nxtCmd + 1;
					brkIndex++;
				}
				*nxtCmd = 0;
			}
		}
		brk[brkIndex] = nullptr;
		for (; true; ++cmd) {
			if (voidChar.voidChar[*cmd])continue;
			else break;
		}
		if (brk[0] == nullptr || *brk[0] == '#' || *brk[0] == 0)continue;
		runCommand(env, *brk, brk + 1);
	}
}

void run(Environment &env, char *commandLines) {
	char *singleCommand = commandLines;
	for (; *commandLines != 0; ++commandLines) {
		if (*commandLines == ';') {
			*commandLines = 0;
			runLine(env, singleCommand);
			singleCommand = commandLines + 1;
		}
	}
	runLine(env, singleCommand);
}


void run(Environment &env) {
	string command;
	while (command != "exit") {
		if (definingUserFunc() != nullptr)printf("%s>>>", definingUserFunc());
		else printf(">>>");
		getline(cin, command);
		if (cin.eof()) {
			printf("\n");
			break;
		}
		char *cmd = (char *) malloc(command.size() + 1);
		strcpy(cmd, command.c_str());
		run(env, cmd);
		free(cmd);
	}
}


Command::Command() = default;

Command::Command(const char *cmd, char *arg[128]) {
	command = new char[strlen(cmd) + 1];
	strcpy(command, cmd);
	int i;
	for (i = 0; arg[i] != nullptr; ++i) {
		args[i] = new char[strlen(arg[i])];
		strcpy(args[i], arg[i]);
	}
	args[i] = nullptr;
}

Command::Command(const Command &cmd) {
	command = new char[strlen(cmd.command) + 1];
	strcpy(command, cmd.command);
	int i;
	for (i = 0; cmd.args[i] != nullptr; ++i) {
		args[i] = new char[strlen(cmd.args[i])];
		strcpy(args[i], cmd.args[i]);
	}
	args[i] = nullptr;
}

Command::~Command() {
	delete[] command;
	for (int i = 0; args[i] != nullptr; ++i) {
		delete[] args[i];
	}
	command = nullptr;
	*args = {nullptr};
}

Command &Command::operator=(const Command &cmd) {
	delete[] command;
	for (int i = 0; args[i] != nullptr; ++i) {
		delete[] args[i];
	}
	command = new char[strlen(cmd.command) + 1];
	strcpy(command, cmd.command);
	int i;
	for (i = 0; cmd.args[i] != nullptr; ++i) {
		args[i] = new char[strlen(cmd.args[i])];
		strcpy(args[i], cmd.args[i]);
	}
	args[i] = nullptr;
	return *this;
}
