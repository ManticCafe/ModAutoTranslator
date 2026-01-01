// Translator.cpp
#include "Translator.h"
#include <codecvt>
#include <locale>

using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// 移除Minecraft格式代码（§符号及其后面的字符）
string cleanMinecraftFormatting(const string& str) {
    string result;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '§' || str[i] == 0xA7) { // 处理§符号的不同编码
            // 跳过格式代码字符（§及其后面的一个字符）
            if (i + 1 < str.size()) {
                i++; // 跳过§后面的字符
                continue;
            }
        }
        result += str[i];
    }
    return result;
}

// 确保字符串是有效的UTF-8
string ensureUtf8(const string& str) {
    string result;
    for (size_t i = 0; i < str.size(); i++) {
        unsigned char c = static_cast<unsigned char>(str[i]);

        // 检查UTF-8字符的有效性
        if (c <= 0x7F) {
            // ASCII字符
            result += str[i];
        } else if (c >= 0xC2 && c <= 0xDF) {
            // 2字节UTF-8字符
            if (i + 1 < str.size()) {
                result += str.substr(i, 2);
                i++;
            } else {
                result += '?';
            }
        } else if (c >= 0xE0 && c <= 0xEF) {
            // 3字节UTF-8字符
            if (i + 2 < str.size()) {
                result += str.substr(i, 3);
                i += 2;
            } else {
                result += '?';
            }
        } else if (c >= 0xF0 && c <= 0xF4) {
            // 4字节UTF-8字符
            if (i + 3 < str.size()) {
                result += str.substr(i, 4);
                i += 3;
            } else {
                result += '?';
            }
        } else {
            // 无效的UTF-8字节
            result += '?';
        }
    }
    return result;
}

// 使用nlohmann/json解析JSON文件
bool parseJsonFile(const string& filePath,
    vector<pair<string, string>>& keyValuePairs) {

    try {
        ifstream file(filePath, ios::binary);
        if (!file.is_open()) {
            cerr << "无法打开文件: " << filePath << endl;
            return false;
        }

        // 读取文件内容
        string content((istreambuf_iterator<char>(file)),
            istreambuf_iterator<char>());
        file.close();

        // 确保内容为有效UTF-8
        content = ensureUtf8(content);

        // 解析JSON
        json j = json::parse(content, nullptr, false);

        if (j.is_discarded()) {
            cerr << "JSON解析失败: 无效的JSON格式" << endl;
            return false;
        }

        for (auto it = j.begin(); it != j.end(); ++it) {
            if (it.value().is_string()) {
                string value = it.value().get<string>();
                // 确保值为有效UTF-8
                value = ensureUtf8(value);
                keyValuePairs.push_back({ it.key(), value });
            } else {
                // 如果不是字符串，转换为字符串
                string value = it.value().dump();
                keyValuePairs.push_back({ it.key(), value });
            }
        }

        return true;
    } catch (const exception& e) {
        cerr << "解析JSON文件失败: " << e.what() << endl;
        return false;
    }
}

// 生成JSON文件
bool generateJsonFile(const string& filePath,
    const vector<pair<string, string>>& keyValuePairs) {

    try {
        // 确保目录存在
        size_t lastSlash = filePath.find_last_of("/\\");
        if (lastSlash != string::npos) {
            string dirPath = filePath.substr(0, lastSlash);
#ifdef _WIN32
            _mkdir(dirPath.c_str());
#else
            mkdir(dirPath.c_str(), 0755);
#endif
        }

        // 构建JSON对象
        json j;
        for (const auto& [key, value] : keyValuePairs) {
            j[key] = value;
        }

        // 写入文件
        ofstream file(filePath, ios::binary);
        if (!file.is_open()) {
            cerr << "无法创建输出文件: " << filePath << endl;
            return false;
        }

        // UTF-8 BOM
        const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
        file.write(reinterpret_cast<const char*>(bom), sizeof(bom));

        // 写入JSON，使用缩进格式
        string json_str = j.dump(4);
        file.write(json_str.c_str(), json_str.size());
        file.close();

        return true;
    } catch (const exception& e) {
        cerr << "生成JSON文件失败: " << e.what() << endl;
        return false;
    }
}

// AI翻译函数 - 修改为OpenAI格式
string translateTextWithOpenAi(const string& text,
    const string& api_url,
    const string& api_key,
    const string& model,
    const float temperature,
    const int max_tokens) {

    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "CURL初始化失败" << endl;
        return text;
    }

    string response_string;

    try {
        // 构建请求JSON - OpenAI格式
        json request_json;
        request_json["model"] = model;

        // 构建消息数组
        json messages = json::array();

        // 系统消息 - 使用英文以避免编码问题
        json system_message;
        system_message["role"] = "system";
        system_message["content"] = "You are a professional game localization expert specializing in translating Minecraft mod content. Please translate the following text from English to Simplified Chinese. Requirements: 1. Maintain game terminology consistency 2. Fit the game context 3. Use natural and fluent Chinese 4. Keep brackets and formatting from the original text 5. For item names, be concise and game-like 6. Do not add extra explanations 7. Just give the translation result without prefix.";

        // 用户消息
        json user_message;
        user_message["role"] = "user";
        user_message["content"] = text;

        messages.push_back(system_message);
        messages.push_back(user_message);

        request_json["messages"] = messages;
        request_json["max_tokens"] = max_tokens;
        request_json["temperature"] = temperature;

        string request_data = request_json.dump();

        // 设置CURL选项
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: application/json");
        string auth_header = "Authorization: Bearer " + api_key;
        headers = curl_slist_append(headers, auth_header.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request_data.length());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "MinecraftModTranslator/1.0");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

        // 禁用SSL验证
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // 执行请求
        CURLcode res = curl_easy_perform(curl);

        // 获取HTTP状态码
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        // 清理
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            cerr << "API请求失败: " << curl_easy_strerror(res) << endl;
            return text;
        }

        if (http_code != 200) {
            cerr << "HTTP错误: " << http_code << endl;
            if (response_string.length() > 200) {
                cerr << "响应前200字符: " << response_string.substr(0, 200) << endl;
            }
            return text;
        }

        // 确保响应为有效UTF-8
        response_string = ensureUtf8(response_string);

        // 解析响应JSON
        json response_json = json::parse(response_string, nullptr, false);

        if (response_json.is_discarded()) {
            cerr << "响应JSON解析失败" << endl;
            return text;
        }

        // 检查是否有错误
        if (response_json.contains("error")) {
            cerr << "API错误: " << response_json["error"]["message"] << endl;
            return text;
        }

        // 获取翻译结果 - OpenAI格式
        string translated = response_json["choices"][0]["message"]["content"];

        // 确保翻译结果为有效UTF-8
        translated = ensureUtf8(translated);

        // 清理翻译结果
        // 移除可能的翻译前缀
        const char* prefixes[] = {
            "翻译：", "翻译:", "Translation: ", "Translated: ", "翻译结果：", "译：", "译文："
        };

        for (const char* prefix : prefixes) {
            if (translated.find(prefix) == 0) {
                translated.erase(0, strlen(prefix));
                break;
            }
        }

        // 移除首尾空格
        if (!translated.empty()) {
            size_t first = translated.find_first_not_of(" \t\n\r");
            size_t last = translated.find_last_not_of(" \t\n\r");
            if (first != string::npos && last != string::npos) {
                translated = translated.substr(first, last - first + 1);
            }
        }

        return translated;
    } catch (const exception& e) {
        cerr << "处理翻译请求时出错: " << e.what() << endl;
        if (curl) {
            curl_easy_cleanup(curl);
        }
        return text;
    }
}

// 批量翻译函数（供线程使用）
void translateBatch(const vector<pair<string, string>*>& batchItems,
    const string& api_url,
    const string& api_key,
    const string& model,
    float temperature,
    int max_tokens,
    atomic<int>& translatedItems,
    atomic<int>& failedItems,
    mutex& coutMutex,
    int batchIndex) {

    for (size_t i = 0; i < batchItems.size(); i++) {
        auto& [key, value] = *batchItems[i];

        // 跳过注释行
        if (key.find("//") == 0) {
            continue;
        }

        // 跳过空值
        if (value.empty()) {
            continue;
        }

        // 清理Minecraft格式代码
        string original_value = value;
        value = cleanMinecraftFormatting(value);

        // 显示当前翻译进度（使用锁保护输出）
        {
            lock_guard<mutex> lock(coutMutex);
            cout << "批次[" << batchIndex << "] 项目[" << i + 1 << "/" << batchItems.size() << "] 翻译: ";
            if (value.length() > 50) {
                cout << value.substr(0, 47) << "..." << endl;
            } else {
                cout << value << endl;
            }
        }

        // 翻译文本
        string translatedText = translateTextWithOpenAi(value, api_url, api_key, model, temperature, max_tokens);

        // 延迟避免API限制（每个线程独立延迟）
        this_thread::sleep_for(chrono::milliseconds(1000));

        if (!translatedText.empty() && translatedText != value) {
            translatedItems++;
            {
                lock_guard<mutex> lock(coutMutex);
                cout << "    -> 成功" << endl;
            }
            value = translatedText;
        } else {
            failedItems++;
            {
                lock_guard<mutex> lock(coutMutex);
                cout << "    -> 失败: 保留原文" << endl;
            }
            // 恢复原始值（包含格式代码）
            value = original_value;
        }
    }
}

// 主翻译函数 - 添加parallel参数
bool translateJsonFile(const string& inputPath,
    const string& outputPath,
    const string& model,
    const string& api_url,
    const string& api_key,
    float temperature,
    int max_tokens,
    int parallel) {  // 添加parallel参数

    // 检查API密钥
    if (api_key.empty()) {
        cerr << "错误: 请提供有效的API密钥" << endl;
        return false;
    }

    // 修正输出路径
    string outputFilePath = outputPath;
    if (outputFilePath.empty()) {
        cerr << "错误: 输出路径不能为空" << endl;
        return false;
    }

    // 如果是目录，添加文件名
    if (outputFilePath.back() == '/' || outputFilePath.back() == '\\') {
        outputFilePath += "zh_cn.json";
    } else if (outputFilePath.find(".json") == string::npos) {
        // 确保路径以斜杠结尾
        if (outputFilePath.find_last_of("/\\") != outputFilePath.length() - 1) {
            outputFilePath += "/";
        }
        outputFilePath += "zh_cn.json";
    }

    try {
        // 解析JSON文件
        vector<pair<string, string>> keyValuePairs;
        if (!parseJsonFile(inputPath, keyValuePairs)) {
            cerr << "错误: 解析JSON文件失败" << endl;
            return false;
        }

        cout << "========================================" << endl;
        cout << "Minecraft模组自动翻译工具" << endl;
        cout << "输入文件: " << inputPath << endl;
        cout << "输出文件: " << outputFilePath << endl;
        cout << "API: " << api_url << endl;
        cout << "模型: " << model << endl;
        cout << "并行数: " << parallel << endl;
        cout << "max_tokens: " << max_tokens << endl;
        cout << "temperature: " << temperature << endl;
        cout << "========================================" << endl;
        cout << "开始翻译..." << endl;

        int totalItems = 0;
        atomic<int> translatedItems(0);
        atomic<int> failedItems(0);

        // 统计总项目数（跳过注释和空值）
        for (const auto& [key, value] : keyValuePairs) {
            totalItems++;
        }

        // 准备并行处理
        vector<vector<pair<string, string>*>> batches(parallel);
        vector<thread> threads;
        mutex coutMutex;

        // 创建批处理
        int batchSize = (keyValuePairs.size() + parallel - 1) / parallel;
        for (int i = 0; i < parallel; i++) {
            for (int j = i * batchSize; j < min((i + 1) * batchSize, (int)keyValuePairs.size()); j++) {
                batches[i].push_back(&keyValuePairs[j]);
            }
        }

        cout << "并行处理: 将" << keyValuePairs.size() << "个项目分成" << parallel << "个批次" << endl;
        cout << "开始并行翻译..." << endl;

        // 启动线程
        for (int i = 0; i < parallel; i++) {
            if (!batches[i].empty()) {
                threads.emplace_back(translateBatch,
                    ref(batches[i]),
                    ref(api_url),
                    ref(api_key),
                    ref(model),
                    temperature,
                    max_tokens,
                    ref(translatedItems),
                    ref(failedItems),
                    ref(coutMutex),
                    i + 1);
            }
        }

        // 等待所有线程完成
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        // 生成输出文件
        if (!generateJsonFile(outputFilePath, keyValuePairs)) {
            cerr << "错误: 无法生成输出文件" << endl;
            return false;
        }

        // 显示统计信息
        cout << "\n========================================" << endl;
        cout << "翻译完成!" << endl;
        cout << "总项目数: " << totalItems << endl;
        cout << "成功翻译: " << translatedItems << endl;
        cout << "翻译失败: " << failedItems << endl;
        cout << "输出文件: " << outputFilePath << endl;
        cout << "========================================" << endl;

        return true;

    } catch (const exception& e) {
        cerr << "严重错误: " << e.what() << endl;
        return false;
    }
}