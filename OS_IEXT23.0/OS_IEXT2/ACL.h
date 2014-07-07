//
//  ACL.h
//  OS_IEXT2
//
//  Created by jimzhai on 14-1-8.
//  Copyright (c) 2014年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_ACL_h
#define OS_IEXT2_ACL_h

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
#include "CommandTool.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
using namespace std;



struct User{
    string user_name;
    vector<string> group;
    string password;
};




void __create_user(string& user_name,const string &user_group,Inode &parent_directory_inode, vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block ,const GroupDescriptor& gdt);

bool __get_user_info(Inode &parent_inode,const string &user_name,const SuperBlock& super_block,const GroupDescriptor& gdt,User &data);

void write_user(vector<Slide_block>& slide_block_queue,vector<BigBlockLink>& big_block_queue,Inode& inode,unsigned int &slide_group,const Directory& dir,const SuperBlock& super_block,const GroupDescriptor& gdt,const User &data);

bool check_password(const string& file_name);

bool check_name(const string& file_name);

bool check_write_permission(Inode& temp_inode,User& User_info,string& file_name);

bool check_read_permission(Inode& temp_inode,User& User_info,string& file_name);

#endif
