//hmcompressor.cpp: Huffman树压缩器的高级封装
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#include <hmcompressor.hpp>
#include <genbitstream.hpp>
#include <custream.hpp>
#include <huffman_tree.hpp>
#include <char_frequency.hpp>
#include <fstream>
#include <numeric>

namespace fs=std::filesystem;
namespace hmcmpsr
{
struct single_cmpsr_impl
{
    std::ifstream ifs;
    fs::path src_path;
    char_frequency_t cfre;
    std::unique_ptr<huffman_tree> tree;
    fs::file_time_type src_cfre_create_time;
    bool cfre_done=false,tree_done=false;//已经计算好cfre/tree
    bool is_opened=false;
    unsigned nbranches,culen;
    uint64_t dbsize;
};

single_cmpsr::single_cmpsr():m(std::make_unique<single_cmpsr_impl>())
{

}

single_cmpsr::~single_cmpsr(){}

void single_cmpsr::open(const std::filesystem::path &raw_file_path,
    unsigned n_branches,unsigned code_unit_length,uint64_t data_block_size)
{
    if(m->is_opened)
        throw std::runtime_error("single_cmpsr::open: 文件已打开");
    m->ifs.open(raw_file_path,std::ios::binary);
    if(!m->ifs)
        throw std::runtime_error("single_cmpsr::open: 打开失败");
    m->nbranches=n_branches,m->culen=code_unit_length,m->dbsize=data_block_size;
    m->tree=huffman_tree::construct(n_branches);
    m->src_path=raw_file_path;
    m->is_opened=true,m->cfre_done=false,m->tree_done=false;
}

void single_cmpsr::close()
{
    m->ifs.close();
    m->is_opened=false;
    m->tree.reset();
}

const char_frequency_t& single_cmpsr::char_frequency()
{
    if(!m->is_opened)
        throw std::runtime_error("single_cmpsr::char_frequency: 文件尚未打开");
    if(!m->cfre_done||fs::last_write_time(m->src_path)>m->src_cfre_create_time)
    {
        auto cuin=icustream::construct(m->culen,m->ifs);
        m->cfre.staticize(*cuin);
        m->ifs.clear();
        m->ifs.seekg(0);
        m->cfre_done=true;
        m->src_cfre_create_time=fs::last_write_time(m->src_path);
        m->tree_done=false;
    }
    return m->cfre;
}

const huffman_tree& single_cmpsr::huffman_tree()
{
    if(!m->is_opened)
        throw std::runtime_error("single_cmpsr::huffman_tree: 文件尚未打开");
    char_frequency();
    if(!m->tree_done)
    {
        m->tree->build_tree(m->cfre,m->nbranches);
        m->tree_done=true;
    }
    return *m->tree;
}

void single_cmpsr::compress(const std::filesystem::path &compressed_file_path)
{
    std::ofstream ofs(compressed_file_path,std::ios::binary);
    if(!ofs)
        throw std::runtime_error("single_cmpsr::compress: 无法打开文件");

    uint64_t raw_file_length=fs::file_size(m->src_path),n_data_blocks,data_block_length;
    {
        uint64_t tmp=m->culen/std::gcd(m->culen,8);
        data_block_length=m->dbsize/tmp*tmp;
    }
    huffman_tree();

    //第一步: 保存文件头
    n_data_blocks=(raw_file_length+data_block_length-1)/data_block_length;
    ofs.write(reinterpret_cast<char*>(&raw_file_length),8);
    ofs.write(reinterpret_cast<char*>(&n_data_blocks),8);
    
    //第二步：保存Huffman树
    m->tree->save_tree(ofs);

    //第三步：压缩并保存压缩后的文件
    auto icus=icustream::construct(m->culen,m->ifs);
    for(uint64_t i=0;i<n_data_blocks;++i)
    {
        auto ogb=genbitsaver::construct(m->nbranches);
        m->tree->encode(*ogb,*icus,data_block_length);
        ogb->save(ofs);
    }
}

struct single_dcmpsr_impl
{
    unsigned nbranches,culen;
    uint64_t raw_file_length,n_data_blocks;
    std::unique_ptr<huffman_tree> tree;
    std::streampos data_start;
    fs::path src_path;
    std::ifstream ifs;
    bool tree_done=false,is_opened=false;
};

single_dcmpsr::single_dcmpsr():m(std::make_unique<single_dcmpsr_impl>()){}
single_dcmpsr::~single_dcmpsr(){}

void single_dcmpsr::open(const std::filesystem::path &compressed_file_path)
{
    if(m->is_opened)
        throw std::runtime_error("single_dcmpsr::open: 文件已打开");
    m->ifs.open(compressed_file_path,std::ios::binary);
    if(!m->ifs)
        throw std::runtime_error("single_dcmpsr::open: 无法打开文件");
    m->ifs.read(reinterpret_cast<char*>(&m->raw_file_length),8);
    m->ifs.read(reinterpret_cast<char*>(&m->n_data_blocks),8);
    m->nbranches=m->ifs.get();
    m->culen=m->ifs.get();
    if(!m->ifs)
    {
        m->ifs.close();
        throw std::runtime_error("single_dcmpsr::open: 文件头不完整");
    }

    m->tree=huffman_tree::construct(m->nbranches);
    m->tree_done=false;
    m->src_path=compressed_file_path;
    m->is_opened=true;
}

void single_dcmpsr::close()
{
    m->is_opened=false;
    m->tree.reset();
}

uint64_t single_dcmpsr::raw_file_size()
{
    if(!m->is_opened)
        throw std::runtime_error("single_dcmpsr::raw_file_size: 文件尚未打开");
    return m->raw_file_length;
}

uint64_t single_dcmpsr::data_blocks_number()
{
    if(!m->is_opened)
        throw std::runtime_error("single_dcmpsr::data_blocks_number: 文件尚未打开");
    return m->n_data_blocks;
}

unsigned single_dcmpsr::huffman_tree_branches()
{
    if(!m->is_opened)
        throw std::runtime_error("single_dcmpsr::huffman_tree_branches: 文件尚未打开");
    return m->nbranches;
}

unsigned single_dcmpsr::code_unit_length()
{
    if(!m->is_opened)
        throw std::runtime_error("single_dcmpsr::code_unit_length: 文件尚未打开");
    return m->culen;
}

const huffman_tree& single_dcmpsr::huffman_tree()
{
    if(!m->is_opened)
        throw std::runtime_error("single_dcmpsr::huffman_tree: 文件尚未打开");
    if(!m->tree_done)
    {
        //让读入指针位于文件头后
        m->ifs.seekg(16);
        m->tree->load_tree(m->ifs);
        m->data_start=m->ifs.tellg();
        m->tree_done=true;
    }
    return *m->tree;
}

void single_dcmpsr::decompress(const std::filesystem::path &uncompressed_file_path)
{
    std::ofstream ofs(uncompressed_file_path,std::ios::binary);
    if(!ofs)
        throw std::runtime_error("single_dcmpsr::decompress: 无法打开文件");
    
    huffman_tree();
    m->ifs.seekg(m->data_start);

    auto ocus=ocustream::construct(m->culen,ofs);
    for(uint64_t i=0;i<m->n_data_blocks;++i)
    {
        auto igb=genbitloader::construct(m->nbranches);
        igb->load(m->ifs);
        m->tree->decode(*ocus,*igb);
    }
    ocus->sync();

    //校验文件大小
    ofs.close();
    auto filesize=fs::file_size(uncompressed_file_path);
    if(filesize<m->raw_file_length)
        throw std::runtime_error("single_dcmpsr::decompress: 压缩文件的数据不完整 (filesize<m->raw_file_length)");
    if(filesize>m->raw_file_length)
        fs::resize_file(uncompressed_file_path,m->raw_file_length);
}
}
