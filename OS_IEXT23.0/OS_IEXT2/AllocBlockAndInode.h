//
//  AllocBlockAndInode.h
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-29.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_AllocBlockAndInode_h
#define OS_IEXT2_AllocBlockAndInode_h

#include "SuperBlock.h"
#include "GroupDescriptor.h"
#include "Slide_block.h"
#include "Binary_operation.h"
#include "Directory.h"
#include "Inode.h"
#include "GroupDescriptor.h"
#include "Start_up.h"
#include "IO.h"
#include "ACL.h"
#include <vector>
#include <iostream>
using namespace std;



Directory_position __alloc_inode(vector<InodeSlideBlock>& inode_slide_queue,unsigned int &inode_group,const SuperBlock& super_block,const GroupDescriptor&gdt);

void __alloc_blocks_and_regist_inode(const string& user_name,const string&user_group,vector<Slide_block>& slide_block_queue,vector<BigBlockLink>& big_block_queue,Inode& inode,unsigned int slide_group,const Directory& dir,const SuperBlock& super_block,const GroupDescriptor& gdt,bool extends=false);//include alloc blocks for small file and big file

void __alloc_blocks(vector<Slide_block>& slide_block_queue,vector<BigBlockLink>& big_block_queue,Inode& inode,unsigned int slide_group,const Directory& dir,const SuperBlock& super_block,const GroupDescriptor& gdt,const unsigned int &best_start,unsigned int used_blocks);

void __wirte_file_data(vector<Slide_block>& slide_block_queue,vector<BigBlockLink>& big_block_queue,Inode& inode,unsigned int &slide_group,const Directory& dir,const SuperBlock& super_block,const GroupDescriptor& gdt,const unsigned int &file_len);

void write_string_data(const string& user_name,const string&user_group,vector<Slide_block>& slide_block_queue,vector<BigBlockLink>& big_block_queue,Inode& inode,unsigned int &slide_group,const Directory& dir,const SuperBlock& super_block,const GroupDescriptor& gdt,const string &data);


#endif
