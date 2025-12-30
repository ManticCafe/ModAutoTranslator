#include <stdio.h>
#include "include/FileFunction.h"
#include "include/Parameters.h"

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
		
		{
			// 创建子文件夹
			std::string zdir = outputPath + getFileName(message.path.data()).c_str();
			checkDir(zdir.data());

			// 解压mods.toml
			std::string tomlPath = "META-INF/mods.toml";
			std::string mcmeta = "pack.mcmeta";

			if (!unZipFile(message.path.data(), zdir.data(), tomlPath.data())) {
				printf("mods.toml文件解压失败");
				return 1;
			}

			if (!unZipFile(message.path.data(), zdir.data(), mcmeta.data())) {
				printf("pack.mcmeta文件解压失败");
				return 1;
			}

			// 读取mod.toml获取mod_id
			if (readModIdToml(mod_id, (zdir + "/" + tomlPath).data())) {
				std::cout << "读取到modID: " << mod_id << std::endl;
			} else {
				printf("modID读取失败");
				return 1;
			}

			// 读取pack.mcmeta获取版本
			if (readJSON(pack_format, (zdir + "/" + mcmeta).data())) {
				std::cout << "读取到pack_format: " << pack_format << std::endl;
			} else {
				printf("pack_format读取失败");
				return 1;
			}

			// 解压lang文件
			if (!unZipFile(message.path.data(), zdir.data(), ("assets/" + mod_id + "/lang/en_us.json").data())) {
				printf("pack.mcmeta文件解压失败");
				return 1;
			}

			// 遍历json文件并发送给ai处理

		}
	}

	if (strcmp(message.mode.data(), "-d") == 0) { // 修改jar包模式

	}

	return 0;
}