#include "Parameters.h"

Message executionParameters(int argc, char* argv[]) {

	struct Message message = {"-d","null"};

	bool jarFound = false;

	// 校验参数数量
	if (argc <= 1) {
		printf("请提供参数\n");
		return message;
	}

	// 遍历源文件参数
    for (int i = 1; i < argc; i++) {

        // 判断模式
        if (strcmp(argv[i], "-r") == 0) {
            message.mode = "-r";
        } else if (strcmp(argv[i], "-d") == 0) {
            message.mode = "-d";
        } else {
            
            // 判断文件
            if (jarFound) {
                printf("未知参数: %s\n", argv[i]);
                message.path = "null";
                return message;
            }

            if (isJar(argv[i])) {
                message.path = argv[i];
                jarFound = true;
            } else {
                printf("未知参数: %s\n", argv[i]);
                message.path = "null";
                return message;
            }
        }
    }

	if (!jarFound) {
		printf("未指定 jar 文件\n");
		message.path = "null";
	}

	return message;
}

bool rMode(Message message, std::string outputPath,std::string& mod_id, int& pack_format) {
	// 创建子文件夹
	std::string zdir = outputPath + getFileName(message.path.data()).c_str();
	checkDir(zdir.data());

	// 解压mods.toml
	std::string tomlPath = "META-INF/mods.toml";
	std::string mcmeta = "pack.mcmeta";

	if (!unZipFile(message.path.data(), zdir.data(), tomlPath.data())) {
		printf("mods.toml文件解压失败");
		return false;
	}

	if (!unZipFile(message.path.data(), zdir.data(), mcmeta.data())) {
		printf("pack.mcmeta文件解压失败");
		return false;
	}

	// 读取mod.toml获取mod_id
	if (readModIdToml(mod_id, (zdir + "/" + tomlPath).data())) {
		std::cout << "读取到modID: " << mod_id << std::endl;
	} else {
		printf("modID读取失败");
		return false;
	}

	// 读取pack.mcmeta获取版本
	if (readJSON(pack_format, (zdir + "/" + mcmeta).data())) {
		std::cout << "读取到pack_format: " << pack_format << std::endl;
	} else {
		printf("pack_format读取失败");
		return false;
	}

	// 解压lang文件
	if (!unZipFile(message.path.data(), zdir.data(), ("assets/" + mod_id + "/lang/en_us.json").data())) {
		printf("pack.mcmeta文件解压失败");
		return false;
	}

	// 遍历json文件并发送给ai处理

}