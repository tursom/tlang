//
// Created by tursom on 18-10-11.
//

#ifndef UNTITLED1_COMMANDREADER_H
#define UNTITLED1_COMMANDREADER_H

#include "Commands.h"

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

void runLine(Environment &env, char *command);

void run(Environment &env, char *command);


#endif //UNTITLED1_COMMANDREADER_H
