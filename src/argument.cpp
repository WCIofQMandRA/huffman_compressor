//argument.cpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the Huffman压缩器.
#include "argument.hpp"
#include "exit.hpp"
#include <nowide/iostream.hpp>
#include <iostream>

namespace argu
{
std::string app_name;
unsigned branch=2;                                  //Huffman树的叉数
unsigned code_unit_length=8;                        //编码单元的长度
uint64_t block_size=1048576;                        //数据块的大小
std::optional <std::filesystem::path> output_path;  //输出文件的路径
std::optional <std::filesystem::path> raw_path;     //原始文件的路径
std::optional <std::filesystem::path> cmp_path;     //压缩文件的路径
bool show_tree=false;                               //输出Huffman树
bool exit_show_tree=false;                          //在输出Huffman树后退出
bool show_frequency=false;                          //输出各个字符的出现频率
bool exit_show_frequency=false;                     //在输出字符频率后退出
bool show_head=false,exit_show_head=false;
}

static void help()
{
    using namespace argu;
    nowide::cout<<"用法: "<<app_name<<" [选项...]\n不含参数的短选项可以连用。\n\n";
    nowide::cout<<"示例：\n  压缩文件\n     "<<app_name<<" -c example.txt\n"
        "     "<<app_name<<" -c example.txt -o name.hmz\n\n  提取文件\n"
        "     "<<app_name<<" -x example.txt.hmz\n"
        "     "<<app_name<<" -x name.hmz -o example.txt\n\n  查看压缩包信息\n"
        "     "<<app_name<<" -iTx example.txt.hmz\n\n";
    nowide::cout<<
R"(基本用法:
  -c, --compress <原始文件目录>     压缩文件。
  -x, --extract <压缩文件目录>      提取文件。
  -o, --out <输出文件目录>          指定输出文件目录。压缩时默认的输出目录是
                                    <原始文件目录>+".hmz"，解压时默认的输出
                                    目录是<压缩文件目录>-".hmz"。

压缩选项:
  -B, --branch <k> [=2]             使用k-叉Huffman树压缩。
  -U, --unit-length <n> [=8]        在统计字符频率时，将原始文件的n个bit视为
                                    一个“字符”。
  -S, --block-size <n> [=1MiB]      指定压缩文件的数据块大小。
                                    取值范围：1KiB~512MiB。
                                    默认的单位是byte，支持的单位：KB(1000
                                    byte), KiB(1024byte), MB(10⁶byte), MiB
                                    (2²⁰byte)。
                                    为了提高压缩速度，当k不是2的幂时，实际的
                                    数据块大小是输入值的立方根（向下取整），
                                    这在默认情况下是101。

查询选项:
  -T, --tree                        输出Huffman树，然后退出。
  -F, --frequency                   输出字符频率，然后退出，仅在压缩时可用。
  -I, --info                        输出压缩包的基本信息（文件头），然后退
                                    出。

输出选项:
  -t, --show-tree                   输出Huffman树。
  -f, --show-frequency              输出字符频率。
                                    由于压缩时先统计字符频率再建树，可用使用
                                    -fT输出字符频率和Huffman树然后退出。
  -i, --show-info                   输出压缩包的基本信息（文件头）。

其他选项:
  -h, --help                        输出本帮助信息。
  -v, --version                     输出版本信息。
      --copying                     输出版权信息。)"<<std::endl;
}

static void version()
{
    //TODO: 从config文件读入版本
    nowide::cout<<"Huffman树压缩器   Version 1.0.0.0\n";
    nowide::cout<<"Copyright (C) 2021-2022 张子辰\n";
    nowide::cout<<"本程序是中国科学技术大学215院011系的马建辉副教授在2021年秋季学期"
        "开始的数据结构课程(011127.01)的“Huffman编码压缩/解压器实验”的一部分。"
        "该课程的老师和助教拥有使用、修改、分发本程序及其衍生作品的权利。\n详细的版权信息"
        "可以使用 “"<<argu::app_name<<" --copying” 查看"<<std::endl;
}

static void copying()
{
    //TODO
    nowide::cout<<"暂无"<<std::endl;
}

static uint64_t block_size_to_int(const std::string &s)
{
    size_t i=0,rate=1;
    while(i<s.length()&&'0'<=s[i]&&s[i]<='9')++i;
    if(auto unit=s.substr(i);unit=="KB")rate=1000;
    else if(unit=="KiB")rate=1024;
    else if(unit=="MB")rate=1000000;
    else if(unit=="MiB")rate=1048576;
    else throw std::runtime_error("block_size_to_int: 未知的文件大小单位 "+unit);
    return std::stoull(s.substr(0,i))*rate;
}

void parse_argument(int argc,char **argv)
{
    using namespace argu;
    using namespace std::string_literals;
    app_name=argv[0];
    for(int i=1;i<argc;++i)
    {
        if(argv[i][0]=='-')
        {
            //长参数名
            if(argv[i][1]=='-')
            {
                if(argv[i]=="--help"s)
                {
                    help();
                    throw exit_type{0};
                }
                else if(argv[i]=="--version"s)
                {
                    version();
                    throw exit_type{0};
                }
                else if(argv[i]=="--copying"s)
                {
                    copying();
                    throw exit_type{0};
                }
                else if(argv[i]=="--compress"s)
                {
                    if(i==argc-1)
                        throw std::runtime_error("parse_argument: --compress的参数缺失");
                    raw_path=std::filesystem::u8path(argv[++i]);
                }
                else if(argv[i]=="--extract"s)
                {
                    if(i==argc-1)
                        throw std::runtime_error("parse_argument: --extract的参数缺失");
                    cmp_path=std::filesystem::u8path(argv[++i]);
                }
                else if(argv[i]=="--out"s)
                {
                    if(i==argc-1)
                        throw std::runtime_error("parse_argument: --out的参数缺失");
                    output_path=std::filesystem::u8path(argv[++i]);
                }
                else if(argv[i]=="--branch"s)
                {
                    if(i==argc-1)
                        throw std::runtime_error("parse_argument: --branch的参数缺失");
                    branch=std::stoul(argv[++i]);
                }
                else if(argv[i]=="--unit-length"s)
                {
                    if(i==argc-1)
                        throw std::runtime_error("parse_argument: --unit-length的参数缺失");
                    code_unit_length=std::stoul(argv[++i]);
                }
                else if(argv[i]=="--block-size"s)
                {
                    if(i==argc-1)
                        throw std::runtime_error("parse_argument: --block-size的参数缺失");
                    block_size=block_size_to_int(argv[++i]);
                }
                else if(argv[i]=="--tree"s)
                {
                    show_tree=exit_show_tree=true;
                }
                else if(argv[i]=="--frequency"s)
                {
                    show_frequency=exit_show_frequency=true;
                }
                else if(argv[i]=="--info"s)
                {
                    show_head=exit_show_head=true;
                }
                else if(argv[i]=="--show-tree"s)
                {
                    show_tree=true;
                }
                else if(argv[i]=="--show-frequency"s)
                {
                    show_frequency=true;
                }
                else if(argv[i]=="--show-info"s)
                {
                    show_head=true;
                }
                else
                {
                    throw std::runtime_error("parse_argument: 未知的选项 "s+argv[i]);
                }
            }
            //短参数名
            else
            {
                for(int j=1;argv[i][j];++j)
                {
                    if(argv[i][j]=='h')
                    {
                        help();
                        throw exit_type{0};
                    }
                    else if(argv[i][j]=='v')
                    {
                        version();
                        throw exit_type{0};
                    }
                    else if(argv[i][j]=='c')
                    {
                        if(argv[i][j+1]==0)
                        {
                            if(i==argc-1)
                                throw std::runtime_error("parse_argument: -c的参数缺失");
                            raw_path=std::filesystem::u8path(argv[++i]);
                        }
                        else
                            raw_path=std::filesystem::u8path(std::string(argv[i]).substr(j+1));
                        break;
                    }
                    else if(argv[i][j]=='x')
                    {
                        if(argv[i][j+1]==0)
                        {
                            if(i==argc-1)
                                throw std::runtime_error("parse_argument: -x的参数缺失");
                            cmp_path=std::filesystem::u8path(argv[++i]);
                        }
                        else
                            cmp_path=std::filesystem::u8path(std::string(argv[i]).substr(j+1));
                        break;
                    }
                    else if(argv[i][j]=='o')
                    {
                        if(argv[i][j+1]==0)
                        {
                            if(i==argc-1)
                                throw std::runtime_error("parse_argument: -o的参数缺失");
                            output_path=std::filesystem::u8path(argv[++i]);
                        }
                        else
                            output_path=std::filesystem::u8path(std::string(argv[i]).substr(j+1));
                        break;
                    }
                    else if(argv[i][j]=='B')
                    {
                        if(argv[i][j+1]==0)
                        {
                            if(i==argc-1)
                                throw std::runtime_error("parse_argument: -B的参数缺失");
                            branch=std::stoul(argv[++i]);
                        }
                        else
                            branch=std::stoul(std::string(argv[i]).substr(j+1));
                        break;
                    }
                    else if(argv[i][j]=='U')
                    {
                        if(argv[i][j+1]==0)
                        {
                            if(i==argc-1)
                                throw std::runtime_error("parse_argument: -U的参数缺失");
                            code_unit_length=std::stoul(argv[++i]);
                        }
                        else
                            code_unit_length=std::stoul(std::string(argv[i]).substr(j+1));
                        break;
                    }
                    else if(argv[i][j]=='S')
                    {
                        if(argv[i][j+1]==0)
                        {
                            if(i==argc-1)
                                throw std::runtime_error("parse_argument: -S的参数缺失");
                            block_size=block_size_to_int(argv[++i]);
                        }
                        else
                            block_size=block_size_to_int(std::string(argv[i]).substr(j+1));
                        break;
                    }
                    else if(argv[i][j]=='T')
                    {
                        show_tree=exit_show_tree=true;
                    }
                    else if(argv[i][j]=='F')
                    {
                        show_frequency=exit_show_frequency=true;
                    }
                    else if(argv[i][j]=='I')
                    {
                        show_head=exit_show_head=true;
                    }
                    else if(argv[i][j]=='t')
                    {
                        show_tree=true;
                    }
                    else if(argv[i][j]=='f')
                    {
                        show_frequency=true;
                    }
                    else if(argv[i][j]=='i')
                    {
                        show_head=true;
                    }
                    else
                    {
                        throw std::runtime_error("parse_argument: 未知的选项 "s+argv[i]);
                    }
                }
            }
        }
        
    }
}
