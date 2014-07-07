//
//  Command.h
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-25.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_Command_h
#define OS_IEXT2_Command_h

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

struct User;

void __get_SuperBlock_GDT_Inode(SuperBlock& super_block,GroupDescriptor& gdt,Inode& inode);

void __list_all_files_of_the_folder(const string& param,const Inode& inode, const SuperBlock& super_block,const GroupDescriptor& gdt);

void __create_file(const string& user_name,const string&user_group,Inode &parent_directory_inode,Directory& dir, vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block ,const GroupDescriptor& gdt ,string data="");

void __rename(User& user_info,Inode &parent_directory_inode,string& old_name, vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block,const GroupDescriptor& gdt,string& new_name);

bool __open_folder(Inode &parent_directory_inode,const string &folder_name, vector<Slide_block> &slide_block_queue,vector<BigBlockLink> &big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block,const GroupDescriptor& gdt);

bool __check_name_exist(Inode &parent_directory_inode,string &dir_name, vector<Slide_block> &slide_block_queue,vector<BigBlockLink> &big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block,const GroupDescriptor& gdt);

bool __open_file(Inode &parent_inode,const string &folder_name,const SuperBlock& super_block,const GroupDescriptor& gdt,string &data);

bool delete_file(User& user_info,const string& file_name,const Inode &parent_inode,Inode& cur_inode,vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block,const GroupDescriptor& gdt);

void copy_file(const string& user_name,const string&user_group,const string& file_name,const string& new_file_name,const string&destination_file_name,const Inode &parent_inode,const Inode& new_folder_inode,vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block,const GroupDescriptor& gdt);

void move_file(User&user_info,const string& user_name,const string&user_group,const Inode& from_inode,const Inode& to_inode,const string &from_file_name,const string&destination_file_name,vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block,const GroupDescriptor& gdt);

void execute_commands();

#endif
