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

bool rMode(Message message, std::string outputPath, std::string& mod_id, int& pack_format, std::string model, std::string API, std::string KEY, float temperature, int max_tokens,int parallel, bool lowVersionMode) {
	// 创建子文件夹
	std::string zdir = outputPath + getFileName(message.path.data()).c_str();
	checkDir(zdir.data());
	checkDir((zdir + "/cache").data());

	// 解压mods.toml
	std::string tomlPath = "META-INF/mods.toml";
	std::string mcmeta = "pack.mcmeta";

	// 如果为兼容模式则不需要下面代码块 变成手动输入id与pack_format ============================
	if (!lowVersionMode) {
		if (!unZipFile(message.path.data(), (zdir + "/cache").data(), tomlPath.data())) {
			printf("mods.toml文件解压失败");
			return false;
		}

		if (!unZipFile(message.path.data(), (zdir + "/cache").data(), mcmeta.data())) {
			printf("pack.mcmeta文件解压失败");
			return false;
		}

		// 读取mod.toml获取mod_id
		if (readModIdToml(mod_id, (zdir + "/cache/" + tomlPath).data())) {
			std::cout << "读取到modID: " << mod_id << std::endl;
		} else {
			printf("modID读取失败");
			return false;
		}

		// 读取pack.mcmeta获取版本
		if (readJSON(pack_format, (zdir + "/cache/" + mcmeta).data())) {
			std::cout << "读取到pack_format: " << pack_format << std::endl;
		} else {
			printf("pack_format读取失败");
			return false;
		}
	} else {
		std::cout << "当前为兼容低版本模式,需要手动输入:" << std::endl << "modID: ";
		std::cin >> mod_id;
		std::cout << std::endl << "pack_format: ";
		std::cin >> pack_format;
		std::cout << std::endl;
	}
	//==================================================================================

	// 解压lang文件
	if (!unZipFile(message.path.data(), (zdir + "/cache").data(), ("assets/" + mod_id + "/lang/en_us.json").data())) {
		printf("lang文件解压失败");
		return false;
	}

	// ai翻译模块:遍历json文件并发送给ai处理
	std::string enLang = zdir + "/cache/assets/" + mod_id + "/lang/en_us.json";
	std::string chLangOut = zdir + "/cache/assets/" + mod_id + "/lang/";
	if (!translateJsonFile(enLang, chLangOut, model, API, KEY, temperature, max_tokens,parallel)) {
		return false;
	}

	// 判断是否是兼容模式 是则创建pack.mcmeta
	if (lowVersionMode) {
		printf("创建pack.mcmeta文件中...\n");
		std::ofstream configFile;
		configFile.open(zdir + "/cache/pack.mcmeta");
		if (configFile.is_open()) {

			{
				configFile << "{" << std::endl;
				configFile << "  \"pack\": {" << std::endl;
				configFile << "    \"description\": {" << std::endl;
				configFile << "      \"text\": \"" + mod_id + "Translate\"" << std::endl;
				configFile << "    }," << std::endl;
				configFile << "    \"pack_format\": " + std::to_string(pack_format) << std::endl;
				configFile << "  }" << std::endl;
				configFile << "}" << std::endl;
			}

			configFile.close();
			printf("配置文件创建成功\n");
		} else {
			printf("配置文件创建失败\n");
			return false;
		}
	}

	// 压缩包输出目录 zdir
	std::string type = ".zip";
	
	printf("正在打包资源包");
	if (!zipFile((zdir + "/cache").data(), (zdir + "/" + getFileName(message.path.data()) + "-Translate-resources" + type).data(), type.data())) {
		return false;
	}
	printf("资源包打包成功: %s", (zdir + "/" + getFileName(message.path.data()) + "-Translate-resources" + type).data());

	return true;
}

bool dMode(Message message, std::string outputPath, std::string& mod_id, std::string model, std::string API, std::string KEY, float temperature, int max_tokens, int parallel, bool lowVersionMode) {
	// 创建子文件夹
	std::string zdir = outputPath + getFileName(message.path.data()).c_str();
	checkDir(zdir.data());
	checkDir((zdir + "/cache").data());

	// 解压mods.toml
	std::string tomlPath = "META-INF/mods.toml";

	// 如果为兼容模式则不需要下面代码块 变成手动输入id与pack_format ============================
	if (!lowVersionMode) {
		if (!unZipFile(message.path.data(), (zdir + "/cache").data(), tomlPath.data())) {
			printf("mods.toml文件解压失败");
			return false;
		}

		// 读取mod.toml获取mod_id
		if (readModIdToml(mod_id, (zdir + "/cache/" + tomlPath).data())) {
			std::cout << "读取到modID: " << mod_id << std::endl;
		} else {
			printf("modID读取失败");
			return false;
		}

	} else {
		std::cout << "当前为兼容低版本模式,需要手动输入:" << std::endl << "modID: ";
		std::cin >> mod_id;
		std::cout << std::endl;
	}
	//==================================================================================

	// 解压lang文件
	if (!unZipFile(message.path.data(), (zdir + "/cache").data(), ("assets/" + mod_id + "/lang/en_us.json").data())) {
		return false;
	}

	// ai翻译模块:遍历json文件并发送给ai处理
	std::string enLang = zdir + "/cache/assets/" + mod_id + "/lang/en_us.json";
	std::string chLangOut = zdir + "/cache/assets/" + mod_id + "/lang/";
	if (!translateJsonFile(enLang, chLangOut, model, API, KEY, temperature, max_tokens, parallel)) {
		return false;
	}

	// 复制jar包到 zdir目录 将翻译出来的 zh_ch.json 写入到jar包内
	if (!copyFile(message.path.data(), (zdir + "/" + getFileName(message.path.data()) + "-Translate.jar").data())) {
		return false;
	}

	printf("正在修改 Jar 包");
	if (!addFileToZip((zdir + "/" + getFileName(message.path.data()) + "-Translate.jar").data(),(chLangOut + "zh_cn.json").data(),("assets/" + mod_id + "/lang/zh_cn.json").data())) {
		return false;
	}
	printf("Jar包修改成功: %s", (zdir + "/" + getFileName(message.path.data()) + "-Translate.jar").data());

	return true;
}