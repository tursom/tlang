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
	Command();
	
	Command(const char *cmd, char *arg[128]);
	
	Command(const Command &cmd);
	
	~Command();
	
	Command &operator=(const Command &cmd);
	
	char *command = nullptr;
	char *args[128] = {nullptr};
};

#endif //UNTITLED1_COMMANDREADER_H
