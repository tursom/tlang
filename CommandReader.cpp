//
// Created by tursom on 18-10-11.
//

#include "CommandReader.h"

const VoidChar voidChar;

void runLine(Environment &env, char *command) {
	if (command[0] == '#')
		return;;
	size_t brk[128];
	size_t brkIndex = 0;
	if (!voidChar.voidChar[command[0]]) {
		brk[brkIndex] = 0;
		brkIndex++;
	}
	for (size_t i = 0; command[i] != 0; ++i) {
		if (command[i] == '\\' && command[i + 1] == '\n') {
			command[i] = 0;
			command[i + 1] = ' ';
		} else {
			if (voidChar.voidChar[command[i]]) {
				if (!voidChar.voidChar[command[i + 1]] && command[i + 1] != '\\' && command[i + 2] != '\n') {
					brk[brkIndex] = i + 1;
					brkIndex++;
				}
				command[i] = 0;
			}
		}
	}
	auto i = funcMap.find(command + *brk);
	if (i != funcMap.end())
		try {
			i->second(env, command, brk + 1, brkIndex - 1);
		} catch (CommandException &e) {
			if (e.message != nullptr)
				printf("%s: %s\n", command + *brk, e.message->c_str());
		}
}

void run(Environment &env, char *command) {
	size_t brk[128] = {0};
	size_t brkIndex = 1;
	for (size_t i = 1; command[i] != 0; ++i) {
		if (command[i] == ';') {
			command[i] = 0;
			brk[brkIndex] = i + 1;
			++brkIndex;
		}
	}
	for (int j = 0; j < brkIndex; ++j) {
		runLine(env, command + brk[j]);
	}
}
