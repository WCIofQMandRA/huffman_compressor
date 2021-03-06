# Hmcmpsr

Version 1.0.0

## 文件格式

数字使用小端序

这里的`struct`只是为了说明数据在硬盘中的数据的顺序，不填补空间以满足对齐要求。

文件开头是文件头：
```c++
struct compressed_file_head_t
{
    //以下两个数据移至huffman_tree_t
    //uint8_t n_branches;          //Huffman树的叉数, 范围2~256，本节记为n
    //uint8_t code_unit_length;    //编码单元的长度, 范围1~64, 单位是bit，本节记为m
    uint32_t identifier=2494000689;//文件标识，表明这是一个hmz文件
    uint32_t version=0;            //文件版本，目前为0
    uint64_t raw_file_length;      //原始的文件的长度，单位byte
    uint64_t n_data_blocks;        //数据块的数量
};
```

之后是Huffman树。因为`n`-叉Huffman树是正则`n`-叉树，所以可以通过先序遍历唯一确定Huffman树的结构。首先保存Huffman树的先序遍历，一个结点使用2bit，虚拟结点（分支点）用00表示，表示字符的叶子结点用10表示，虚拟叶子结点用11表示，使用0填充至整数字节。之后按从左往右的顺序保存叶子结点对应的字母，一个字母的长度是`m` bit，使用0填充至整数字节。
```c++
struct huffman_tree_t
{
    uint8_t n_branches;
    uint8_t code_unit_length;
    bitstream dfs_order;          //树的先序遍历
    custream chars;               //按从左往右的顺序保存叶子结点对应的字母, 忽略虚拟字符
};
```
bitstream和custream由`icustream`读取，`ocustream`写入。

之后是若干数据块。一个数据块对应原始文件的1KiB~128MiB的数据。除最后一个数据块外，其他数据块的长度是code_unit_length的倍数（单位byte）。单个数据块格式：
```c++
struct data_block_t
{
    uint32_t compressed_genbits;  //压缩后的理论长度，单位genbit，也就是压缩后的文件使用n进制输出时的长度
    uint32_t compressed_bytes;    //压缩后的实际长度，单位byte
    char data[compressed_bytes];
};
```
数据块由`genbitloader::load`读取，`genbitsaver::save`写入。

总体格式
```c++
struct compressed_file_t
{
    compressed_file_head_t head;
    huffman_tree_t tree;
    data_block_t blocks[head.n_data_blocks];
};
```

## 许可证

BSD 3-Clause License

详见 `./LICENSE`。

本库可以选择性地使用 mini-gmp 构建，但如果这么做，您在发布本库的二进制文件时，必须提供 mini-gmp 的源代码。

