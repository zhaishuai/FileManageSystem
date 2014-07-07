//
//  Inode.h
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-25.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_Inode_h
#define OS_IEXT2_Inode_h
#include <iostream>
using namespace std;

struct Pointer{
    unsigned int i_pointer;
    unsigned short int groupth;
};

struct Access_table{
    char user[4];
    char group[4];
    char other[4];
};

struct Inode{
    unsigned int i_position;
    unsigned int i_group;
    string user_name;   //Owner uid
    string user_group;
    Access_table access_table;
    unsigned int i_size;    //File size
    unsigned int i_ctime;   //Creation time
    unsigned int i_used_blocks;  //used blocks
    Pointer i_pointer[15]; //a group pointer to blocks
    
};

#endif
