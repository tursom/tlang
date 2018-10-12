#include <sys/time.h>
#include <cstring>
#include "Commands.h"
#include "CommandReader.h"
#include <iostream>

using namespace std;

long getCurrentTime() {
	struct timeval tv{};
	gettimeofday(&tv, nullptr);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main() {
	timeval t1{}, t2{};
	Environment env;
	char command[] = "@ push 1 2\\\n   ;@ pop a; @ pop b; set c add $a $b; echo $c;";
	gettimeofday(&t1, nullptr);
	run(env, command);
	gettimeofday(&t2, nullptr);
	printf("%li\n", (t2.tv_sec - t1.tv_sec) * 1000 * 1000 + t2.tv_usec - t1.tv_usec);
	cout << sizeof(Val) << endl;
	return 0;
}