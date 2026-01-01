#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <string>
#include <cstring>
#include <cstdio>
#include "FileFunction.h"
#include "Translator.h"

struct Message {
	std::string mode;
	std::string path;
};

// 执行参数:源文件、生成模式(修改jar或生成材质包)
Message executionParameters(int argc, char* argv[]);

// -r 模式
bool rMode(Message message, std::string outputPath, std::string& mod_id, int& pack_format, std::string model, std::string API, std::string KEY, float temperature, int max_tokens, int parallel, bool lowVersionMode);

// -d 模式
bool dMode(Message message, std::string outputPath, std::string& mod_id, std::string model, std::string API, std::string KEY, float temperature, int max_tokens, int parallel, bool lowVersionMode);