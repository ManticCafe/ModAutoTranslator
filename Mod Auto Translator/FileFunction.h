#pragma once

#include <string>
#include <zip.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

// 判断是否是jar文件
bool isJar(char* file);

// 获取拓展名
std::string getFileExtension(char* file);

// 获取文件名
std::string getFileName(char* file);

// 创建配置文件 成功返回true
bool creatConfigFile();

// 读取配置文件 如果配置文件不存在则创建
bool readConfigFile(std::string& model,std::string& API, std::string& KEY, float& temperature, int& max_tokens, std::string& outputPath);

// 检查文件夹是否存在 不存在则创建
bool checkDir(char* dirPath);

// 创建文件夹
bool createDir(char* dirPath);

// 创建输出文件夹
void createOutputDir(std::string outputPath);

// =============待实现=============

// 解压 Jar 文件
bool unZipJarFile(char* file);

// 压缩jar文件
bool ZipJarFile(char* file,char* zipFileType);