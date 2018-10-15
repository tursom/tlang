#include <sys/time.h>
#include "Commands.h"
#include "CommandReader.h"
#include <sys/stat.h>

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
	printf("import using %li microseconds\n", (t2.tv_sec - t1.tv_sec) * 1000 * 1000 + t2.tv_usec - t1.tv_usec);
	run(env);
	return 0;
}