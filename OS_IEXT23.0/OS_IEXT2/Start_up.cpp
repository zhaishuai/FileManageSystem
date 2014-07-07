//
//  Start_up.cpp
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-26.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#include "Start_up.h"

void loading_slide_block(vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_link,unsigned int &group , const SuperBlock& super_block, const GroupDescriptor& gdt){

    BinaryBlock *block = new BinaryBlock(super_block.block_size);
    
    
    char state_map[2][2];
    for (int i=0,n=0; i<2; i++) //inital state map
        for(int j=0;j<2;j++)
            state_map[i][j] = n++;
    
    char last_state = 1;
    Slide_block slide_block;
    
    //char path[] = "/Users/jimzhai/Documents/project Place/OS_IEXT2/vitual_disk.iext2";
    fstream file(path,ios_base::in);
    
    
    //__get_block_bit_map(super_block, gdt, 0, block);
    unsigned int total_count = 0;
    for (unsigned short int group_number = group; group_number < super_block.block_groups; group_number++) {
        file.seekg(BOOT_BLOCK_BYTE+super_block.blocks_per_group*(super_block.block_size)*group_number+gdt.block_bitmap_position,ios_base::beg);
        block->read_block(file);
        
        slide_block.end_groupth = group_number ;
        //cout<<"---------------->>"<<group_number<<endl;
        for(unsigned short int i = 0 ; i < super_block.blocks_per_group ; i++)
        {
            switch (state_map[last_state][block->test(i)]) {
                case 0: slide_block.count++;    //counting
                    total_count++;
                    break;
                    
                case 1: slide_block_queue.push_back(slide_block);   //end counting
                    //cout<<"push_back start groupth:"<<slide_block.start_groupth<<"   "<<"end groupth:"<<slide_block.end_groupth<<endl;
                    break;
                    
                case 2:     //start count
                    slide_block.start = i;
                    slide_block.start_groupth = group_number;
                    slide_block.count = 1;
                    total_count++;
                    break;
                    
                case 3:
                    break;
                    
                default:
                    break;
                    
            }//switch
            last_state = block->test(i);

            
        }//for
        if (total_count>=super_block.blocks_per_group) {
            break;
        }//size of slider block is equal to the block group size;
        

    }
    group = ++slide_block.end_groupth;
    if (last_state == 0)
        slide_block_queue.push_back(slide_block);
    file.close();
    for(int i = 0 ; i < big_block_link.size(); i++)
    {
        //cout<<"--->>>>"<<big_block_link[i].start_groupth<<endl;
        if (slide_block.end_groupth >= big_block_link[i].start_groupth) {
            big_block_link.erase(big_block_link.begin() + i--);
        }else{
            break;
        }
    }

    delete block;
}//loading_slide_block


void loading_big_block_link(vector<BigBlockLink>& big_block_link,const unsigned short int group, const SuperBlock& super_block, const GroupDescriptor& gdt){
    
    BinaryBlock *block = new BinaryBlock(super_block.block_size);
    char state_map[2][2];
    for (int i=0,n=0; i<2; i++) //inital state map
        for(int j=0;j<2;j++)
            state_map[i][j] = n++;
    
    char last_state = 1;
    BigBlockLink big_block;
    
    //char path[] = "/Users/jimzhai/Documents/project Place/OS_IEXT2/vitual_disk.iext2";
    fstream file(path,ios_base::in);

    for (unsigned short int group_number = group; group_number < super_block.block_groups; group_number++) {
        file.seekg(BOOT_BLOCK_BYTE+super_block.blocks_per_group*(super_block.block_size)*group_number+gdt.block_bitmap_position,ios_base::beg);
        block->read_block(file);

        big_block.end_groupth = group_number;
        for(unsigned short int i = 0 ; i < super_block.blocks_per_group ; i++)
        {
            switch (state_map[last_state][block->test(i)]) {
                case 0: big_block.count++;    //counting
                    break;
                    
                case 1:
                    if(big_block.count>=super_block.blocks_per_group/2)
                    {
                        big_block_link.push_back(big_block);   //end counting
                        //cout<<"push_back start groupth:"<<big_block.start_groupth<<"   "<<"end groupth:"<<big_block.end_groupth<<endl;
                    }
                    break;
                    
                case 2:     //start count
                    big_block.start = i;
                    big_block.start_groupth = group_number;
                    //cout<<"here-->"<<group_number<<endl;
                    big_block.count = 1;
                    break;
                    
                case 3:
                    break;
                    
                default:
                    break;
                    
            }//switch
            last_state = block->test(i);
            
        }//for

    }
    if (last_state == 0&&big_block.count>=super_block.blocks_per_group/2){
        big_block.end_groupth++;
        big_block_link.push_back(big_block);
    }
    file.close();
    delete block;
}//loading_big_block_link


void loading_inode_slide_block(vector<InodeSlideBlock>& inode_slide_queue,unsigned int& inode_group, const SuperBlock& super_block, const GroupDescriptor& gdt){

    BinaryBlock *block = new BinaryBlock(gdt.inode_table_position-gdt.inode_bitmap_position);
    char state_map[2][2];
    for (int i=0,n=0; i<2; i++) //inital state map
        for(int j=0;j<2;j++)
            state_map[i][j] = n++;
    char last_state = 1;
    InodeSlideBlock inode_slide;
    //char path[] = "/Users/jimzhai/Documents/project Place/OS_IEXT2/vitual_disk.iext2";
    fstream file(path,ios_base::in);

    unsigned int total_count = 0;
    for (unsigned short int group_number = inode_group; group_number < gdt.inode_table_position-gdt.inode_bitmap_position; group_number++) {
        file.seekg(BOOT_BLOCK_BYTE+super_block.blocks_per_group*(super_block.block_size)*group_number+gdt.inode_bitmap_position,ios_base::beg);
        block->read_block(file);
        
        inode_slide.end_groupth = group_number;
        //cout<<"---------------->>"<<group_number<<endl;
        for(unsigned short int i = 0 ; i < super_block.blocks_per_group ; i++)
        {
            switch (state_map[last_state][block->test(i)]) {
                case 0: inode_slide.count++;    //counting
                    total_count++;
                    break;
                    
                case 1: inode_slide_queue.push_back(inode_slide);   //end counting
                    //cout<<"push_back start groupth:"<<inode_slide.start_groupth<<"   "<<"end groupth:"<<inode_slide.end_groupth<<endl;
                    break;
                    
                case 2:     //start count
                    inode_slide.start = i;
                    inode_slide.start_groupth = group_number;
                    inode_slide.count = 1;
                    total_count++;
                    break;
                    
                case 3:
                    break;
                    
                default:
                    break;
                    
            }//switch
            last_state = block->test(i);
            
            
            
        }//for
        if (total_count>=gdt.inode_table_position-gdt.inode_bitmap_position) {
            break;
        }//size of slider block is equal to the block group size;
        
        
    }
    inode_group = ++inode_slide.end_groupth;
    if (last_state == 0)
        inode_slide_queue.push_back(inode_slide);
    file.close();
    delete block;


}//loading_inode_slide_block

void loading_root_inode(Inode& inode,const SuperBlock& super_block,const GroupDescriptor& gdt){
    fstream file(path,ios_base::in);
    file.seekg(BOOT_BLOCK_BYTE+gdt.inode_table_position,ios_base::beg);
    file.read((char *)(&inode), sizeof(Inode));
    file.close();
    
}//loading_root_inode

