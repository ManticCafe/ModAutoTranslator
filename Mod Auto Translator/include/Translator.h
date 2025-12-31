#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <map>
#include <direct.h>
#include <sstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <algorithm>

using json = nlohmann::json;

// З­вы
bool translateJsonFile(const std::string& inputPath,const std::string& outputPath,const std::string& model,const std::string& api_url,const std::string& api_key,float temperature,int max_tokens);