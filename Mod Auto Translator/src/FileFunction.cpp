#include "FileFunction.h"

bool isJar(char* file) {
	std::string ext = getFileExtension(file);
	std::string extLower = ext;
	std::transform(extLower.begin(), extLower.end(), extLower.begin(), ::tolower);
	return extLower == ".jar";
}

std::string getFileExtension(char* file) {
	char dot = '.';
	const char* lastDot = strrchr(file, (int)dot);
	if (!lastDot || lastDot == file) {
		return "";
	}
	return std::string(lastDot);
}

std::string getFileName(char* file) {
	const char* lastSeparator = strrchr(file, '\\');
	if (!lastSeparator) {
		lastSeparator = strrchr(file, '/');
		if (!lastSeparator) {
			lastSeparator = file - 1;
		}
	}
	const char* nameStart = lastSeparator + 1;
	const char* lastDot = strrchr(nameStart, '.');
	if (!lastDot || lastDot == nameStart) {
		return std::string(nameStart);
	}
	return std::string(nameStart, lastDot);
}

bool creatConfigFile() {
	printf("创建配置文件中...\n");
	std::ofstream configFile;
	configFile.open("ModAutoTranslator-config.json");
	if (configFile.is_open()) {

		configFile << "{" << std::endl;
		configFile << "    \"model\" : null," << std::endl;
		configFile << "    \"API\" : null," << std::endl;
		configFile << "    \"KEY\" : null," << std::endl;
		configFile << "    \"temperature\" : 1.3," << std::endl;
		configFile << "    \"max_tokens\" : 500," << std::endl;
        configFile << "    \"parallel\" : 1," << std::endl;
        configFile << "    \"lowVersionMode\" : false," << std::endl;
		configFile << "    \"outputPath\" : \"./output/\"" << std::endl;
		configFile << "}" << std::endl;

		configFile.close();
		printf("配置文件创建成功\n");
		return true;
	} else {
		printf("配置文件创建失败\n");
		return false;
	}
}

bool readConfigFile(std::string& model, std::string& API, std::string& KEY, float& temperature, int& max_tokens, std::string& outputPath, std::string filepath, int& parallel, bool& lowVersionMode) {
	std::string filename = filepath;
	if (std::filesystem::exists(filename)) {
		
		std::ifstream configFile(filename);

		if (!configFile.is_open()) {
			printf("配置文件打开失败\n");
			return false;
		}

		nlohmann::json data = nlohmann::json::parse(configFile);
		configFile.close();

		if (data["model"].is_null()) {
			printf("请填写model\n");
			return false;
		} else {
			model = data["model"].get<std::string>();
		}

		if (data["API"].is_null()) {
			printf("请填写API\n");
			return false;
		} else {
			API = data["API"].get<std::string>();
		}

		if (data["KEY"].is_null()) {
			printf("请填写KEY\n");
			return false;
		} else {
			KEY = data["KEY"].get<std::string>();
		}

		if (data["temperature"].is_null()) {
			printf("请填写temperature\n");
			return false;
		} else {
			temperature = data["temperature"];
		}

		if (data["max_tokens"].is_null()) {
			printf("请填写max_tokens\n");
			return false;
		} else {
			max_tokens = data["max_tokens"];
		}

        if (data["parallel"].is_null()) {
            printf("请填写parallel\n");
            return false;
        } else if (data["parallel"] <= 0) {
            printf("parallel !< 1\n");
            return false;
        } else {
            parallel = data["parallel"];
        }

        if (data["lowVersionMode"].is_null()) {
            printf("请填写lowVersionMode\n");
            return false;
        } else {
            lowVersionMode = data["lowVersionMode"];
        }

		if (data["outputPath"].is_null()) {
			printf("请填写有效的输出地址\n");
			return false;
		} else {
			outputPath = data["outputPath"].get<std::string>();
		}

		return true;

	} else {
		printf("配置文件不存在，将创建配置文件...\n");
		creatConfigFile();
		return false;
	}
}

bool checkDir(char* dirPath) {
	if (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath)) {
		return true;
	} else {
		createDir(dirPath);
		return false;
	}
}

bool createDir(char* dirPath) {
	if (std::filesystem::create_directories(dirPath)) {
		return true;
	} else {
		return false;
	}
}

void createOutputDir(std::string outputPath) {
	if (!outputPath.empty()) {
		char* outputPathCStr = new char[outputPath.length() + 1];
		strcpy_s(outputPathCStr, outputPath.length() + 1, outputPath.c_str());
		if (!checkDir(outputPathCStr)) {
			printf("输出目录创建完成\n");
		}
		
		delete[] outputPathCStr;
	}
}

bool unZipFile(char* file, char* outputPath, char* filePath) {
    int err = 0;
    zip* archive = nullptr;
    zip_file* zip_file_ptr = nullptr;

    // 检查参数有效性
    if (!file || !outputPath) {
        std::cerr << "无效参数: file或outputPath为null" << std::endl;
        return false;
    }

    // 确保输出目录存在
    createDir(outputPath);

    // 打开ZIP文件
    archive = zip_open(file, 0, &err);
    if (!archive) {
        std::cerr << "无法打开ZIP文件: " << file << std::endl;
        return false;
    }

    // 获取ZIP文件中的条目数量
    zip_int64_t num_entries = zip_get_num_entries(archive, 0);
    if (num_entries < 0) {
        std::cerr << "无法获取ZIP文件条目数量" << std::endl;
        zip_close(archive);
        return false;
    }

    std::string target_file = "";
    if (filePath) {
        target_file = filePath;
    }

    std::string output_dir = outputPath;
    // 确保输出目录以路径分隔符结尾
    if (!output_dir.empty() && output_dir.back() != '/' && output_dir.back() != '\\') {
        output_dir += "/";
    }

    int extracted_count = 0;
    bool file_found = false;

    // 遍历ZIP文件中的所有条目
    for (zip_int64_t i = 0; i < num_entries; i++) {
        const char* name = zip_get_name(archive, i, 0);
        if (!name) {
            continue;
        }

        std::string entry_name = name;

        // 检查是否是目录（以'/'结尾）
        bool is_directory = !entry_name.empty() && entry_name.back() == '/';

        // 判断是否应该解压这个文件
        bool should_extract = false;

        if (target_file.empty()) {
            // 如果filePath为空，解压所有文件
            should_extract = true;
        } else if (target_file == "*") {
            // 如果filePath为"*"，解压所有文件
            should_extract = true;
        } else {
            // 检查是否精确匹配或部分匹配
            if (entry_name == target_file) {
                should_extract = true;
            } else if (entry_name.find(target_file) != std::string::npos) {
                should_extract = true;
            }
        }

        if (!should_extract) {
            continue;
        }

        file_found = true;

        // 构造完整的输出路径
        std::string extracted_path = output_dir + entry_name;

        // 创建必要的目录
        if (is_directory || entry_name.find('/') != std::string::npos) {
            std::filesystem::path file_path(extracted_path);
            if (file_path.has_parent_path()) {
                std::filesystem::create_directories(file_path.parent_path());
            }
        }

        // 如果是目录条目，只需创建目录
        if (is_directory) {
            std::filesystem::create_directories(extracted_path);
            continue;
        }

        // 打开ZIP中的文件
        zip_file_ptr = zip_fopen_index(archive, i, 0);
        if (!zip_file_ptr) {
            std::cerr << "无法打开ZIP中的文件: " << entry_name << std::endl;
            continue;
        }

        // 获取文件信息
        zip_stat_t stat;
        if (zip_stat_index(archive, i, 0, &stat) != 0) {
            std::cerr << "无法获取文件信息: " << entry_name << std::endl;
            zip_fclose(zip_file_ptr);
            continue;
        }

        // 创建输出文件
        std::ofstream out_file(extracted_path, std::ios::binary);
        if (!out_file.is_open()) {
            std::cerr << "无法创建输出文件: " << extracted_path << std::endl;
            zip_fclose(zip_file_ptr);
            continue;
        }

        // 读取并写入文件
        std::vector<char> buffer(8192);
        zip_int64_t bytes_read;
        zip_int64_t total_read = 0;

        while ((bytes_read = zip_fread(zip_file_ptr, buffer.data(), buffer.size())) > 0) {
            out_file.write(buffer.data(), bytes_read);
            total_read += bytes_read;
        }

        // 关闭文件
        out_file.close();
        zip_fclose(zip_file_ptr);

        extracted_count++;
        std::cout << "已解压: " << entry_name << " (" << total_read << " 字节)" << std::endl;

        // 如果是特定文件，且只需要解压这个文件，可以提前退出
        if (!target_file.empty() && target_file != "*" && entry_name == target_file) {
            break;
        }
    }

    // 关闭ZIP文件
    zip_close(archive);

    if (!file_found && !target_file.empty() && target_file != "*") {
        std::cerr << "未找到指定文件: " << target_file << std::endl;
        return false;
    }

    std::cout << "解压完成: " << extracted_count << " 个文件已解压到 " << output_dir << std::endl;
    return true;
}

bool readModIdToml(std::string& mod_id, char* tomlpath) {
    try {
        auto file = toml::parse_file(tomlpath);
        mod_id = file["mods"][0]["modId"].value_or<std::string>("");
        return true;
    } catch (...) {
        mod_id.clear();
        return false;
    }
}

bool readJSON(int& output, std::string filepath) {
	std::string filename = filepath;
    if (std::filesystem::exists(filename)) {

        std::ifstream configFile(filename);

        if (!configFile.is_open()) {
            printf("配置文件打开失败\n");
            return false;
        }

        nlohmann::json data = nlohmann::json::parse(configFile);
        configFile.close();

        if (data["pack"]["pack_format"].is_null()) {
            printf("读取pack_format失败\n");
            return false;
        } else {
            output = data["pack"]["pack_format"];
            return true;
        }
    } else {
        return false;
    }
}