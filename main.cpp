#include <sys/time.h>
#include <cstring>
#include "Commands.h"
#include "CommandReader.h"

long getCurrentTime() {
	struct timeval tv{};
	gettimeofday(&tv, nullptr);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main() {
	Environment env;
	char command[] = "@ push a 10.0 +20.2; echo $@";
	long t1 = getCurrentTime();
	run(env, command);
	long t2 = getCurrentTime();
	printf("%li\n", t2 - t1);
	return 0;
}