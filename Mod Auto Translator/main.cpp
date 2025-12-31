#include <stdio.h>
#include <windows.h>
#include "include/FileFunction.h"
#include "include/Parameters.h"

int main(int argc, char* argv[]) {

	// 设置控制台编码
	SetConsoleOutputCP(936);
	SetConsoleCP(936);

	// =================配置文件所需的变量声明===============

	std::string model; // 模型
	std::string API; // api
	std::string KEY; // key
	float temperature = 0.0F;
	int max_tokens = 0;
	std::string outputPath; // 输出路径

	//==================逻辑部分==========================

	// 读取配置文件，如果配置文件不存在则创建配置文件
	readConfigFile(model,API,KEY, temperature, max_tokens,outputPath, "ModAutoTranslator-config.json");

	// 创建输出文件夹
	createOutputDir(outputPath);

	// 结构体接收参数: mode-模式、path-jar文件路径
	Message message = executionParameters(argc, argv);

	// 接收结构体错误处理
	if (strcmp(message.path.c_str(), "null") == 0) {
		return 1;
	}

	// 模式流程判断
	if (strcmp(message.mode.data(), "-r") == 0) { // 资源包模式

		std::string mod_id; // 模组id
		int pack_format; // 数据包版本编号
		
		if (!rMode(message,outputPath,mod_id,pack_format,model,API,KEY,temperature,max_tokens)) {
			return 1;
		}

	}

	if (strcmp(message.mode.data(), "-d") == 0) { // 修改jar包模式

	}

	return 0;
}