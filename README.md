# Huffman 压缩器 Huffman Compressor

中国科大 2021 秋 数据结构（011127.01） 实验 02

Version 1.0.0

## 生成

编译本程序需要 CMake 和支持 C++ 17 的编译器。

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

编译的输出位于`build\output`。

### 库依赖

1. gmp (可选)，找不到时使用 mini-gmp 编译。

## 使用方法

本软件只提供命令行界面。完整的使用方法可以使用`huffman_compressor --help`查看。

### 基本用法

| 选项 | 功能 | 备注 |
| - | - | --- |
| `-c, --compress <原始文件>` | 压缩文件。           | 程序只支持压缩单个文件。`<原始文件>`可以是单个文件名，也可以是相对路径或绝对路径。 |
| `-x, --extract <压缩文件>`  | 提取文件。           | 除非查看帮助信息、版本信息等，运行程序时必须选择`-c`异或`-x`。 |
| `-o, --out <输出文件>`      | 指定输出文件的名称。 | 默认情况下，压缩时的输出文件名是`<原始文件>`+".hmz"，解压时的输出文件名是`<压缩文件>`-".hmz"。字符串的减运算`a`-`b`的含义是，如果`a`的后缀是`b`，则运算结果是删除后缀`b`后的`a`，否则，运算出错。 |
| `-–override` | 在输出文件已存在时，覆盖原来的文件。 |  |

### 示例

#### 压缩文件

```sh
huffman_compressor -c example.txt
huffman_compressor -c example.txt -o name.hmz
```

#### 提取文件

```sh
huffman_compressor -x example.txt.hmz
huffman_compressor -x name.hmz -o example.txt
```

#### 查看压缩包信息

```sh
huffman_compressor -iTx example.txt.hmz
```

### 查看处理进度

当程序收到 `SIGINT` 信号（一般是按`Ctrl+C`）时，会向标准错误流输出压缩/解压的处理进度。对于交互式的stdin，连续收到两次`SIGINT`信号会终止程序。

**注意：在Windows上，累积按两次`Ctrl+C`就会终止程序，并且程序不会收到第二次按`Ctrl+C`发出的`SIGINT`信号。这是Windows控制台的问题，暂时无法解决。** ~~这充分体现了 Linux 的优越性。~~

## 功能

实验要求中的功能：
- [x] 所有基本要求，包括但不限于：
  - 每次运行程序时，用户可指定 只压缩/只解压缩 指定路径的文件；
  - 压缩时不需要指定解压文件的目标路径，解压时不需要指定压缩前原文件的路径，压缩后的文件可以换到另一个位置再做解压缩。
  - 不使用 STL 容器。
- [x] 指定符号单元大小（0.5/1/1.5/2/2.5/3/3.5/4字节）。
- [x] 指定多元 Huffman 压缩（2~16叉树）。

拓展功能：
- [x] 可以任意组合符号单元大小和 Huffman 树的叉数。
- [x] 将符号单元的取值范围扩展到1~64比特。
- [x] 将Huffman树的叉树的取值范围扩展到2~256。
- [x] 可以在 压缩/解压 过程中查看处理进度。
- [x] 命令行界面。 

其他特色：
- [x] 完整地实现`std::map`的接口，实现`std::vector`、`std::queue`、`std::priority_queue`的大部分接口。
- [x] 只要文件系统支持并且储存空间足够，支持任意大小的文件。
- [x] 将 Huffman 压缩的功能封装在 hmcmpsr 库中。
- [x] 用高精度整数运算处理非 $2^n$-叉 Huffman 树。
- [x] `--help`选项输出的帮助信息整齐而完整。
- [x] 能够在 Windows 上正确地处理非 ASCII 字符（用 Boost.Nowide 和`std::filesystem`实现）。

## 许可证

BSD 3-Clause License

详见 `./LICENSE`。

### 与 hmcmpsr 库的关系

Hmcmpsr 库与本软件（Huffman 压缩器）同时编写，并遵循相同的许可证。Hmcmpsr 库既可以被视为本软件的一部分，也可以被分离出来单独使用和分发。目录`lib/hmcmpsr/`完整地包含了 hmcmpsr 库的组件。

