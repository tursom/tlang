//
// Created by tursom on 18-10-11.
//

#include "CommandReader.h"

const VoidChar voidChar;

void runLine(Environment &env, char *command) {
	if (command != nullptr) {
		for (int j = 0; true; ++j) {
			if (voidChar.voidChar[command[j]])continue;
			else if (command[j] == '#' || command[j] == 0)return;
			else break;
		}
		char *brk[128];
		size_t brkIndex = 0;
		if (!voidChar.voidChar[command[0]]) {
			brk[brkIndex] = command;
			brkIndex++;
		}
		for (size_t i = 0; command[i] != 0; ++i) {
			if (command[i] == '\\' && command[i + 1] == '\n') {
				command[i] = 0;
				command[i + 1] = ' ';
			} else {
				if (voidChar.voidChar[command[i]]) {
					if (!voidChar.voidChar[command[i + 1]] && command[i + 1] != '\\'
					    && command[i + 2] != '\n' && command[i + 1] != 0) {
						brk[brkIndex] = command + i + 1;
						brkIndex++;
					}
					command[i] = 0;
				}
			}
		}
		brk[brkIndex] = nullptr;
		auto i = funcMap.find(brk[0]);
		if (i != funcMap.end())
			try {
				i->second(env, brk + 1);
			} catch (CommandException &e) {
				if (e.message != nullptr)
					printf("%s: %s\n", brk[0], e.message->c_str());
			}
	}
}

void run(Environment &env, char *command) {
	char *brk[10240];
	size_t brkIndex = 1;
	brk[0] = command;
	for (size_t i = 1; command[i] != 0; ++i) {
		if (command[i] == ';') {
			command[i] = 0;
			brk[brkIndex] = command + i + 1;
			++brkIndex;
		}
	}
	brk[brkIndex] = nullptr;
	for (int j = 0; j < brkIndex; ++j) {
		runLine(env, brk[j]);
	}
}
