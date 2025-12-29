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