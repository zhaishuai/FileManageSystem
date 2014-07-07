//
//  IO.h
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-30.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_IO_h
#define OS_IEXT2_IO_h

#include <fstream>
#include "ConstantList.h"
#include "SuperBlock.h"
#include "GroupDescriptor.h"
#include "Directory.h"
#include "Inode.h"
#include <iostream>
#include <vector>
#include "Slide_block.h"
#include "AllocBlockAndInode.h"
using namespace std;


void write_file_to_folde(const string& user_name,const string&user_group,Inode &parent_directory_inode,Directory& dir, vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block ,const GroupDescriptor& gdt);

void __addressing(fstream& file,unsigned int blockth,const Inode& parent_inode,const SuperBlock& super_block,const GroupDescriptor& gdt);

void write_inode(const Directory& dir,const Inode& inode,const SuperBlock& super_block,const GroupDescriptor& gdt);

void get_inode(const Directory& dir,Inode& inode,const SuperBlock& super_block,const GroupDescriptor& gdt);

#endif
