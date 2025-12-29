#include "FileFunction.h"

bool isJar(char* file) {
	std::string ext = getFileExtension(file);
	std::string extLower = ext;
	std::transform(extLower.begin(), extLower.end(), extLower.begin(), ::tolower);
	return extLower == ".jar";
}

std::string getFileExtension(char* file) {
	char dot = '.';
	const char* lastDot = strrchr(fileName, (int)dot);
	if (!lastDot || lastDot == fileName) {
		return "";
	}
	return std::string(lastDot);
}

std::string getFileName(char* file) {
	const char* lastSeparator = strrchr(fileName, '\\');
	if (!lastSeparator) {
		lastSeparator = strrchr(fileName, '/');
		if (!lastSeparator) {
			lastSeparator = fileName - 1;
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

bool readConfigFile(std::string& model, std::string& API, std::string& KEY, float& temperature, int& max_tokens, std::string& outputPath) {
	std::string filename = "ModAutoTranslator-config.json";
	if (std::filesystem::exists(filename)) {
		
		std::ifstream configFile("ModAutoTranslator-config.json");

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