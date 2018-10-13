#include <sys/time.h>
#include <cstring>
#include "Commands.h"
#include "CommandReader.h"
#include <iostream>
#include <sys/stat.h>

using namespace std;

long getCurrentTime() {
	struct timeval tv{};
	gettimeofday(&tv, nullptr);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main(int argc, char *argv[]) {
	char *file[2] = {nullptr, nullptr};
	timeval t1{}, t2{};
	Environment env;
	gettimeofday(&t1, nullptr);
	for (int i = 1; i < argc; ++i) {
		file[0] = argv[i];
		runCommand(env, "import", file);
	}
	gettimeofday(&t2, nullptr);
	printf("improt using %li microsecons\n", (t2.tv_sec - t1.tv_sec) * 1000 * 1000 + t2.tv_usec - t1.tv_usec);
	run(env);
	return 0;
}