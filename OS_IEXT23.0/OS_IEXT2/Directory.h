//
//  Directory.h
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-25.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_Directory_h
#define OS_IEXT2_Directory_h

#define IEXT2_NAME_LEN 32 //Byte

#define IEXT2_FT_UNKNOW 0
#define IEXT2_FT_REG_FILE 1
#define IEXT2_FT_DIR 2
#define IEXT2_FT_USER 3
#define IEXT2_FT_VOID 4
#define END 2
#define NO 1



#include <string>
using namespace std;
struct Directory_position{
    unsigned int inode;
    unsigned int group;
};

struct Directory{

    Directory_position position;
    char file_type;
    string name;
    unsigned int file_size;
    bool next;

};



#endif
