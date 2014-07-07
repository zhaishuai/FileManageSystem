//
//  AllocBlockAndInode.cpp
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-29.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#include "AllocBlockAndInode.h"

Directory_position __alloc_inode(vector<InodeSlideBlock>& inode_slide_queue,unsigned int &inode_group,const SuperBlock& super_block,const GroupDescriptor&gdt){

    Directory_position result;
    if(inode_slide_queue.size()!=0)
    {
        result.inode = inode_slide_queue[0].start;
        //cout<<"inode_slide_queue[0].start:"<<inode_slide_queue[0].start<<endl;
        result.group = inode_slide_queue[0].start_groupth;
        inode_slide_queue.at(0).start ++;
        inode_slide_queue.at(0).count --;
        if(inode_slide_queue[0].count <= 0)
            inode_slide_queue.erase(inode_slide_queue.begin() + 0);
        
    }else{
        loading_inode_slide_block(inode_slide_queue,inode_group, super_block, gdt);
        if(inode_slide_queue.size() == 0){
            cout<<"inode is not enough!"<<endl;
            result.inode = -1;
            return result;
        }
        result = __alloc_inode(inode_slide_queue, inode_group, super_block, gdt);
    }
    
    
    
    
    BinaryBlock *inode_bit_map = new BinaryBlock(gdt.inode_table_position - gdt.inode_bitmap_position);
    fstream file(path,ios_base::in|ios_base::out);
    file.seekg(BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*result.group+gdt.inode_bitmap_position,ios_base::beg);
    inode_bit_map->read_a_piece_of_block(file, result.inode);
    inode_bit_map->set(result.inode);
    inode_bit_map->write_a_piece_of_block(file, result.inode);
    file.close();
    delete inode_bit_map;
    //cout<<"alloc_inode inode --->>"<<result.inode<<"   groupth-->"<<result.group<<endl;
    return result;
}//__alloc_inode

void __alloc_blocks_and_regist_inode(const string& user_name,const string&user_group,vector<Slide_block>& slide_block_queue,vector<BigBlockLink>& big_block_queue,Inode& inode,unsigned int slide_group,const Directory& dir,const SuperBlock& super_block,const GroupDescriptor& gdt,bool extends){
    unsigned int used_blocks=0;
    if (dir.file_size < (super_block.block_size*super_block.blocks_per_group)/2) {  //small file
        
        unsigned int file_len = dir.file_size/super_block.block_size;//file using the quantity of blocks
        if(dir.file_size%super_block.block_size)
            file_len++;
        
        if(file_len < 12){
            
        }else if(file_len < 12 + super_block.block_size/sizeof(Pointer)){
            file_len += 1;
        }else if(file_len<12 + super_block.block_size/sizeof(Pointer) + (super_block.block_size/sizeof(Pointer))*(super_block.block_size/sizeof(Pointer))){
            file_len += 2;
        }else if(file_len<12 + super_block.block_size/sizeof(Pointer) + (super_block.block_size/sizeof(Pointer))*(super_block.block_size/sizeof(Pointer))+(super_block.block_size/sizeof(Pointer))*(super_block.block_size/sizeof(Pointer))*(super_block.block_size/sizeof(Pointer))){
            file_len += 3;
        }
        //cout<<"extends: "<<extends<<endl;
        if(extends){
            used_blocks = inode.i_used_blocks;
            file_len -= used_blocks;
            inode.i_used_blocks++;
        }
        else
            inode.i_used_blocks += file_len;
        
        if (slide_block_queue.size()) { //disk not full
            
            unsigned int total_blocks = 0;
            for(int i = 0 ; i < slide_block_queue.size();i++){
                total_blocks += slide_block_queue[i].count;
            }
            if (total_blocks<(super_block.blocks_per_group)/2) {
                loading_slide_block(slide_block_queue,big_block_queue, slide_group, super_block, gdt);
            }
            /*Pre-alloc blocks*/
            
            unsigned int best_start = 0;
            unsigned int best_actial_len = -1;
            unsigned int now_start_position;
            double actial_len = 0;
            for(unsigned int i= 0 ,len = 0; i < slide_block_queue.size();i++){
                now_start_position = slide_block_queue[i].start;
                for (unsigned int j = i ; j < slide_block_queue.size();j++){
                    len += slide_block_queue[j].count;
                    actial_len += slide_block_queue[j].start - now_start_position + slide_block_queue[j].count;
                    now_start_position = slide_block_queue[j].start + slide_block_queue[j].count;
                    if(len >= file_len){
                        actial_len -= len - file_len;
                        if(best_actial_len == -1){
                            best_actial_len = actial_len;
                            best_start = i;
                        }
                        else if(actial_len < best_actial_len){
                            best_actial_len = actial_len;
                            best_start = i;
                        }
                        len = 0;
                        actial_len = 0;
                        break;
                    }else{
                        if(slide_block_queue[j].end_groupth - slide_block_queue[j].start_groupth != 0){
                            if(j+1<slide_block_queue.size()){
                                now_start_position = slide_block_queue[j+1].start;
                                actial_len += (slide_block_queue[j].end_groupth - slide_block_queue[j].start_groupth)*(gdt.first_data_block_position/super_block.block_size);//plus used blocks in group
                            }
                        }
                    }
                }
            }
            /*alloc blocks*/
            __alloc_blocks(slide_block_queue, big_block_queue, inode, slide_group, dir, super_block, gdt,best_start,used_blocks);
            Access_table default_access_table{"rwx","r-x","r--"};
            inode.access_table = inode.access_table;
            inode.user_name = user_name;
            inode.user_group = user_group;
            if(!extends)
                write_inode(dir, inode, super_block, gdt);
            else{
                Directory tem_dir;
                tem_dir.position.inode=inode.i_position;
                tem_dir.position.group=inode.i_group;
                write_inode(tem_dir, inode, super_block, gdt);
            }
            __wirte_file_data(slide_block_queue, big_block_queue, inode, slide_group, dir, super_block, gdt, file_len);
        }else{
            cout<<"disk is full!"<<endl;
        }
    }else{  //large file
    
    }
    
}//__alloc_blocks_and_regist_inode


void __alloc_blocks(vector<Slide_block>& slide_block_queue,vector<BigBlockLink>& big_block_queue,Inode& inode,unsigned int slide_group,const Directory& dir,const SuperBlock& super_block,const GroupDescriptor& gdt,const unsigned int &best_start,unsigned int used_blocks){

    BinaryBlock *binary_block = new BinaryBlock(super_block.block_size);
    unsigned int i_seekg;
    fstream file(path,ios_base::in|ios_base::out);
    i_seekg = BOOT_BLOCK_BYTE+super_block.blocks_per_group*(super_block.block_size)*slide_block_queue[best_start].start_groupth+gdt.block_bitmap_position;
    file.seekg(i_seekg,ios_base::beg);
    binary_block->read_block(file);
    
    for(unsigned int i = used_blocks,flag = best_start  ;i < inode.i_used_blocks && flag < slide_block_queue.size(); i++){
        if(slide_block_queue[flag].count <= 0){
            file.seekg(i_seekg,ios_base::beg);
            binary_block->write_block(file);
            slide_block_queue.erase(slide_block_queue.begin() + flag);
            i_seekg = BOOT_BLOCK_BYTE+super_block.blocks_per_group*(super_block.block_size)*slide_block_queue[best_start].start_groupth+gdt.block_bitmap_position;
            file.seekg(i_seekg,ios_base::beg);
            binary_block->read_block(file);
        }
        if(i < 15){
            inode.i_pointer[i].i_pointer = slide_block_queue[flag].start*super_block.block_size;
            //cout<<dir.name<<"used_blocks: "<<i<<",    alloc block position:"<<inode.i_pointer[i].i_pointer+1024<<" the real blocks: "<<inode.i_pointer[i].i_pointer/super_block.block_size<<endl;
            //cout<<"test alloc_blocks---->>>i_pointer:"<<inode.i_pointer[i].i_pointer<<endl;
            binary_block->set(slide_block_queue[best_start].start);
            //cout<<"+_+_+_+_+_+_+_+_+_+_+_+slide_block_queue[flag].start:"<<slide_block_queue[flag].start<<endl;
            slide_block_queue[flag].count--;
            slide_block_queue[flag].start++;
            //cout<<"----------------------------------->"<<slide_block_queue[flag].start<<endl;
            inode.i_pointer[i].groupth = slide_block_queue[flag].start_groupth;
        }
        
    }
    file.seekg(i_seekg,ios_base::beg);
    binary_block->write_block(file);
    delete binary_block;
    
}//__alloc_blocks

void __wirte_file_data(vector<Slide_block>& slide_block_queue,vector<BigBlockLink>& big_block_queue,Inode& inode,unsigned int &slide_group,const Directory& dir,const SuperBlock& super_block,const GroupDescriptor& gdt,const unsigned int &file_len){
    
    if(dir.file_type == IEXT2_FT_DIR){
        fstream file(path,ios_base::in|ios_base::out);
        __addressing(file, inode.i_used_blocks-1, inode, super_block, gdt);
        Directory directory ;
        directory.position.group = -1;
        directory.position.inode = -1;
        directory.file_size = -1;
        directory.file_type = IEXT2_FT_VOID;
        directory.name = "";
        directory.next = false;
        for(int i = 0 ; i < super_block.block_size/sizeof(Directory) ; i++)
        {
            file.write((char *)(&directory), sizeof(Directory));
        }
        
        file.close();
    }
    
}//__wirte_data

void write_string_data(const string& user_name,const string&user_group,vector<Slide_block>& slide_block_queue,vector<BigBlockLink>& big_block_queue,Inode& inode,unsigned int &slide_group,const Directory& dir,const SuperBlock& super_block,const GroupDescriptor& gdt,const string &data){
    
    fstream file(path,ios_base::in|ios_base::out);
    unsigned int data_size = (unsigned int)(data.size()/super_block.block_size);
    if(data.size()%super_block.block_size)
        data_size++;
    if(data_size>inode.i_used_blocks)
        __alloc_blocks_and_regist_inode(user_name,user_group,slide_block_queue, big_block_queue, inode, slide_group, dir, super_block, gdt);
    for(int i = 0; i < data_size-1 ;i++){
        __addressing(file, i, inode, super_block, gdt);
        file<<(string)data.substr(i*super_block.block_size,i+super_block.block_size)<<endl;
    }
    __addressing(file, data_size-1, inode, super_block, gdt);
    
    file<<(string)data.substr((data_size-1)*super_block.block_size,data.size())<<endl;
    //cout<<file.tellg()<<" write_string_data:"<<(string)data.substr((data_size-1)*super_block.block_size,data.size())<<endl;
}//write_string_data


