#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include "FileFunction.h"
#include "Parameters.h"

int main(int argc, char* argv[]) {

	std::string API; // api
	std::string KEY; // key
	std::string outputPath; // 输出路径

	// 读取配置文件，如果配置文件不存在则创建配置文件
	readConfigFile(API,KEY,outputPath);

	// 创建输出文件夹
	createOutputDir(outputPath);

	Message message = executionParameters(argc, argv);
	// 错误处理
	if (strcmp(message.path.c_str(), "null") == 0) {
		return -1;
	}
	return 0;
}