//
//  Start_up.h
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-26.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_Start_up_h
#define OS_IEXT2_Start_up_h


#include "Binary_operation.h"
#include "Slide_block.h"
#include "SuperBlock.h"
#include "GroupDescriptor.h"
#include "ConstantList.h"
#include "Inode.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
using namespace std;
#include <boost/dynamic_bitset.hpp>
using namespace boost;
typedef Slide_block BigBlockLink;

typedef Slide_block InodeSlideBlock;


void loading_slide_block(vector<Slide_block> &slide_block_queue, vector<BigBlockLink>& big_block_link,unsigned int &group, const SuperBlock& super_block, const GroupDescriptor& gdt);

void loading_big_block_link(vector<BigBlockLink>& big_block_link,const unsigned short int group, const SuperBlock& super_block, const GroupDescriptor& gdt);

void loading_inode_slide_block(vector<InodeSlideBlock>& inode_slide_block,unsigned int& inode_group, const SuperBlock& super_block, const GroupDescriptor& gdt);

void loading_root_inode(Inode& inode,const SuperBlock& super_block,const GroupDescriptor& gdt);


#endif
