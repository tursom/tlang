//
// Created by tursom on 18-10-11.
//

#ifndef UNTITLED1_COMMANDREADER_H
#define UNTITLED1_COMMANDREADER_H

#include "Commands.h"
#include <cstring>

struct VoidChar {
	VoidChar() {
//		voidChar[0] = true;
		voidChar[' '] = true;
		voidChar['\t'] = true;
		voidChar['\n'] = true;
	}
	
	bool voidChar[256]{};
};

extern const VoidChar voidChar;

void runLine(Environment &env, char *nxtCmd);

void run(Environment &env, char *commandLines);

void runCommand(Environment &env, const char *command, char *args[]);

void run(Environment &env);

struct Command {
	Command() = default;
	
	Command(const char *cmd, char *arg[128]) {
		command = new char[strlen(cmd) + 1];
		strcpy(command, cmd);
		int i;
		for (i = 0; arg[i] != nullptr; ++i) {
			args[i] = new char[strlen(arg[i])];
			strcpy(args[i], arg[i]);
		}
		args[i] = nullptr;
	}
	
	Command(const Command &cmd) {
		command = new char[strlen(cmd.command) + 1];
		strcpy(command, cmd.command);
		int i;
		for (i = 0; cmd.args[i] != nullptr; ++i) {
			args[i] = new char[strlen(cmd.args[i])];
			strcpy(args[i], cmd.args[i]);
		}
		args[i] = nullptr;
	}
	
	~Command() {
		delete[] command;
		for (int i = 0; args[i] != nullptr; ++i) {
			delete[] args[i];
		}
		command = nullptr;
		*args = {nullptr};
	}
	
	Command &operator=(const Command &cmd) {
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
	
	char *command = nullptr;
	char *args[128] = {nullptr};
};

#endif //UNTITLED1_COMMANDREADER_H
