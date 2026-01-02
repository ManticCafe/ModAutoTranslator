# Mod Auto Translator

# 索引
1.[简介]
2.[配置文件]
3.[使用方法]
4.[如何查看modId与pack_format?]

---

## 简介
  Mod Auto Translator 是一款AI模组汉化工具,支持修改模组或导出材质包。支持全版本模组(版本低于1.12需要从配置文件ModAutoTranslator-config.json中手动开启lowVersionMode)

---
## 配置文件
```
{

    "model" : null, // 使用的AI模型

    "API" : null, // API url链接

    "KEY" : null, // 你的个人秘钥

    "temperature" : 1.3, // temperature 参数,参数越低AI越严谨,参数越高AI越有创造力

    "max_tokens" : 500, // 最大tokens

    "parallel" : 1, // 翻译并行数

    "lowVersionMode" : false, // 兼容模式,翻译版本小于1.12需要把此项设置为true

    "outputPath" : "./output/" // 输出目录

}
```


---
## 使用方法
  使用前请先配置ModAutoTranslator-config.json
### 版本大于1.12
#### 材质包模式
```
ModAutoTranslator.exe 你的源文件路径 -r
```
#### Jar包模式
```
ModAutoTranslator.exe 你的源文件路径 -d
```
### 版本小于等于1.12
  版本小于等于1.12请先前往ModAutoTranslator-config.json将lowVersionMode项调整为 true。
#### 材质包模式
```
ModAutoTranslator.exe 你的源文件路径 -r
```
  输入你的modId与pack_format。
#### Jar包模式
```
ModAutoTranslator.exe 你的源文件路径 -d
```
  输入你的modId。

---

## 如何查看modId与pack_format?
### modId
  在主界面中,点击mod选项,选中mod，即可看见modId
### pack_format

| 版本              | pack_format |
| --------------- | ----------- |
| 1.6.1 – 1.8.9   | 1           |
| 1.9 – 1.10.2    | 2           |
| 1.11 – 1.12.2   | 3           |
| 1.13 – 1.14.4   | 4           |
| 1.15 – 1.16.1   | 5           |
| 1.16.2 – 1.16.5 | 6           |
| 1.17 – 1.17.1   | 7           |
| 1.18 – 1.18.2   | 8           |
| 1.19 – 1.19.2   | 9           |
| 1.19.3          | 12          |
| 1.19.4          | 13          |
| 1.20 – 1.20.1   | 15          |
| 1.20.2          | 18          |
| 1.20.3 – 1.20.4 | 22          |
| **1.21**        | 50          |

---

