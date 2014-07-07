//
//  SuperBlock.h
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-25.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_SuperBlock_h
#define OS_IEXT2_SuperBlock_h

struct SuperBlock{
    unsigned int inodes_count;//inode total quantity（max value:2^32）
    unsigned int blocks_count;//block total quantity（max value:2^32）
    unsigned int block_groups;//block groups total quantity (max value:2^32)
    unsigned short int blocks_per_group;//the quantity of blocks in each bloks group (max value:2^16)
    unsigned int inodes_per_group;//the quantity of inodes in each bloks group (max value:2^32)
    unsigned short int block_size;//the size of block
    unsigned int free_inodes_count;
    unsigned int free_blocks_count;
};

#endif
