//
//  CommandTool.h
//  OS_IEXT2
//
//  Created by jimzhai on 13-12-10.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_CommandTool_h
#define OS_IEXT2_CommandTool_h

#define MKDIR 0
#define VIM 1
#define MV 2
#define CD 3
#define LS 4
#define SHUTDOWN 5
#define DELETE 6
#define COPY 7
#define RENAME 8
#define SHOW_SLIDE_INDEX 9
#define SHOW_INODE_INDEX 10
#define PWD 11
#define CREATEUSER 12
#define ADMINADD 13
#define ROOTADD 14
#define CHANGEPERMISSION 15
#define REMOVEUSER 16
#define FORMAT 17

#include "Directory.h"
#include "ConstantList.h"
#include "GroupDescriptor.h"
#include "Inode.h"
#include "SuperBlock.h"
#include "Start_up.h"
#include "Directory.h"
#include "AllocBlockAndInode.h"
#include "IO.h"
#include "Binary_operation.h"
#include "Command.h"
#include "Format.h"
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

struct I_Path{
    Inode inode;
    string name;
};

void string_trim(string& str);

void init_command_hash_map(map<string, unsigned int> &command_map);

string __temp_cd(Inode &temp_inode,const Inode &root_inode,const string &command,const vector<I_Path> &ipath,vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block ,const GroupDescriptor& gdt);

void cin_command(map<string, unsigned int> &command_map,vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block ,const GroupDescriptor& gdt,Inode inode);

bool check_file_name(const string& file_name);

#endif
