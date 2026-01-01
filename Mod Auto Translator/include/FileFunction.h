#pragma once

#include <string>
#include <zip.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <toml.hpp>

// 判断是否是jar文件
bool isJar(char* file);

// 获取拓展名
std::string getFileExtension(char* file);

// 获取文件名
std::string getFileName(char* file);

// 创建配置文件 成功返回true
bool creatConfigFile();

// 读取配置文件 如果配置文件不存在则创建
bool readConfigFile(std::string& model, std::string& API, std::string& KEY, float& temperature, int& max_tokens, std::string& outputPath, std::string filepath, int& parallel, bool& lowVersionMode);

// 读取json文件
bool readJSON(int& output, std::string filepath);

// 检查文件夹是否存在 不存在则创建
bool checkDir(char* dirPath);

// 创建文件夹
bool createDir(char* dirPath);

// 创建输出文件夹
void createOutputDir(std::string outputPath);

// 解压 Jar 文件 成功返回true 文件名，要单独解压的文件在压缩包内的路径
bool unZipFile(char* file, char* outputPath, char* filePath);

// 读取toml
bool readModIdToml(std::string& mod_id, char* tomlpath);

// =============待实现=============
// 
// 压缩文件
bool ZipFile(char* file, char* outputPath, char* zipFileType);