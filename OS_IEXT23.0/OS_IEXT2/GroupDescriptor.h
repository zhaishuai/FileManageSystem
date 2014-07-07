//
//  GroupDescriptor.h
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-25.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_GroupDescriptor_h
#define OS_IEXT2_GroupDescriptor_h

struct GroupDescriptor{
    unsigned short int block_bitmap_position; //Relative to the current cursor position
    unsigned short int inode_bitmap_position;   //Relative to the current cursor position
    unsigned short int inode_table_position;
    //unsigned short int free_blocks_in_group;
    //unsigned int free_inodes_in_group;
    unsigned int first_data_block_position;//the position of first data block.

};
#endif
