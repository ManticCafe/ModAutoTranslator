#include <stdio.h>
#include "FileFunction.h"
#include "Parameters.h"

int main(int argc, char* argv[]) {

	// =================配置文件所需的变量声明===============

	std::string model; // 模型
	std::string API; // api
	std::string KEY; // key
	float temperature = 0.0F;
	int max_tokens = 0;
	std::string outputPath; // 输出路径

	//==================逻辑部分==========================

	// 读取配置文件，如果配置文件不存在则创建配置文件
	readConfigFile(model,API,KEY, temperature, max_tokens,outputPath);

	// 创建输出文件夹
	createOutputDir(outputPath);

	// 结构体接收参数: mode-模式、path-jar文件路径
	Message message = executionParameters(argc, argv);

	// 接收结构体错误处理
	if (strcmp(message.path.c_str(), "null") == 0) {
		return -1;
	}

	// 解压文件 unZipFile函数还未实现
	if (!unZipFile(message.path.data())) {
		printf("文件解压失败");
		return -1;
	}

	return 0;
}