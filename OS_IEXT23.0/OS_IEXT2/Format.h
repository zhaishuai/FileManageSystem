//
//  Format.h
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-25.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_Format_h
#define OS_IEXT2_Format_h

#include "ConstantList.h"
#include <iostream>
#include <fstream>
#include "SuperBlock.h"
#include "Inode.h"
#include "GroupDescriptor.h"
#include "Binary_operation.h"
#include <string>
#include "Directory.h"
#include <vector>
#include "Slide_block.h"
#include "Start_up.h"
#include "Command.h"
#include "ACL.h"
using namespace std;



//check the virtal_disk.dat file exists
bool __check_virtual_disk_exists();

//format the virtal_disk
//return true mean format success ,false means false
bool format_virual_disk();

//format code is here
bool __format_iext2(s_int block_size,un_int average_file_size,l_int disk_size);

void __super_block_register(const s_int block_size,const un_int average_file_size,const l_int disk_size ,const GroupDescriptor *group_descriptor_groups, SuperBlock& super_block);

void __group_descriptor_register(const s_int block_size,const un_int average_file_size,const l_int disk_size ,GroupDescriptor *group_descriptor_groups);

void __create_default_folde();

#endif
