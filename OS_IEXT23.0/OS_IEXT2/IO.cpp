//
//  IO.cpp
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-30.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#include "IO.h"

void write_file_to_folde(const string& user_name,const string&user_group,Inode &parent_inode,Directory& dir, vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block ,const GroupDescriptor& gdt){
//注意 文件名存满一个块再存时会有bug
    Directory cur_dir;
    cur_dir.name = "";
    cur_dir.next = false;
    
    fstream file(path,ios_base::in|ios_base::out);
    bool find = false;
    for (int i = 0 ;  i < parent_inode.i_used_blocks; i++ ) {
        __addressing(file, i, parent_inode, super_block, gdt);
        for(int j=0;j<super_block.block_size/sizeof(Directory);j++){
            file.read((char *)(&cur_dir), sizeof(Directory));
            if(!cur_dir.next)
            {
                find = true;
                break;
            }
            if (i==parent_inode.i_used_blocks-1 && j==(super_block.block_size/sizeof(Directory))-1) {
                
                __alloc_blocks_and_regist_inode(user_name,user_group,slide_block_queue, big_block_queue, parent_inode, slide_group, dir, super_block, gdt,true);
                break;
            }

        }
        if (find) {
            break;
        }

    }
    file.seekg(-sizeof(Directory),ios_base::cur);
    //cout<<"write "<<dir.name<<"    "<<file.tellg()<<endl;

    file.write((char *)(&dir), sizeof(Directory));
    file.close();
    
}//write_file_to_folde


void __addressing(fstream& file,unsigned int blockth,const Inode& inode,const SuperBlock& super_block,const GroupDescriptor& gdt){

    if (blockth < 12) {
        file.seekg(BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*inode.i_pointer[blockth].groupth+inode.i_pointer[blockth].i_pointer,ios_base::beg);
        //cout<<"move len -->"<<BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*inode.i_pointer[blockth].groupth+inode.i_pointer[blockth].i_pointer<<endl;
        
    }else if(blockth<12 + super_block.block_size/sizeof(Pointer))
    {
        file.seekg(BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*inode.i_pointer[12].groupth+inode.i_pointer[12].i_pointer,ios_base::beg);
        file.seekg((blockth - 12)*sizeof(Pointer),ios_base::cur);
        Pointer pointer;
        file.read((char *)(&pointer), sizeof(Pointer));
        file.seekg(BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*pointer.groupth+pointer.i_pointer,ios_base::beg);

    }else if(blockth<12 + super_block.block_size/sizeof(Pointer) + (super_block.block_size/sizeof(Pointer))*(super_block.block_size/sizeof(Pointer))){
        
        file.seekg(BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*inode.i_pointer[13].groupth+inode.i_pointer[13].i_pointer,ios_base::beg);
        file.seekg((((blockth - 12 - super_block.block_size/sizeof(Pointer))/(super_block.block_size/sizeof(Pointer)))*sizeof(Pointer)), ios_base::cur);
        Pointer pointer;
        file.read((char *)(&pointer), sizeof(Pointer));
        file.seekg(BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*pointer.groupth+pointer.i_pointer,ios_base::beg);
        file.seekg((((blockth - 12 - super_block.block_size/sizeof(Pointer))%(super_block.block_size/sizeof(Pointer)))*sizeof(Pointer)), ios_base::cur);
        file.read((char *)(&pointer), sizeof(Pointer));
        file.seekg(BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*pointer.groupth+pointer.i_pointer,ios_base::beg);
        
    }else if(blockth<12 + super_block.block_size/sizeof(Pointer) + (super_block.block_size/sizeof(Pointer))*(super_block.block_size/sizeof(Pointer))+(super_block.block_size/sizeof(Pointer))*(super_block.block_size/sizeof(Pointer))*(super_block.block_size/sizeof(Pointer))){
        
        file.seekg(BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*inode.i_pointer[14].groupth+inode.i_pointer[14].i_pointer,ios_base::beg);
        long long int temp = blockth - 12 - super_block.block_size/sizeof(Pointer) - (super_block.block_size/sizeof(Pointer))*(super_block.block_size/sizeof(Pointer));
        file.seekg((temp/((super_block.block_size/sizeof(Pointer))*(super_block.block_size/sizeof(Pointer))))*sizeof(Pointer),ios_base::cur);
        Pointer pointer;
        file.read((char *)(&pointer), sizeof(Pointer));
        file.seekg(BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*pointer.groupth+pointer.i_pointer,ios_base::beg);
        temp = temp%((super_block.block_size/sizeof(Pointer))*(super_block.block_size/sizeof(Pointer)));
        file.seekg(temp/(super_block.block_size/sizeof(Pointer))*sizeof(Pointer),ios_base::cur);
        file.read((char *)(&pointer), sizeof(Pointer));
        file.seekg(BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*pointer.groupth+pointer.i_pointer,ios_base::beg);
        file.seekg(temp%((super_block.block_size/sizeof(Pointer))*sizeof(Pointer)), ios_base::cur);
        file.read((char *)(&pointer), sizeof(Pointer));
        file.seekg(BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*pointer.groupth+pointer.i_pointer,ios_base::beg);
        
    }
    
}//addressing

void write_inode(const Directory& dir,const Inode& inode,const SuperBlock& super_block,const GroupDescriptor& gdt){

    fstream file(path,ios_base::in|ios_base::out);
    file.seekg(BOOT_BLOCK_BYTE+super_block.blocks_per_group*(super_block.block_size)*dir.position.group+gdt.inode_table_position+dir.position.inode*sizeof(Inode),ios_base::beg);
    file.write((char *)(&inode), sizeof(Inode));
    //cout<<"write inode---:"<<BOOT_BLOCK_BYTE+super_block.blocks_per_group*(super_block.block_size)*dir.position.group+gdt.inode_table_position+dir.position.inode*sizeof(Inode)<<endl;
    //cout<<"inode.i_used_blocks:"<<inode.i_used_blocks<<endl;
    for(int i=0;i<inode.i_used_blocks;i++){
        //cout<<"i:"<<BOOT_BLOCK_BYTE+(super_block.blocks_per_group*super_block.block_size)*inode.i_pointer[i].groupth+inode.i_pointer[i].i_pointer<<endl;
    }
    file.close();
    
}//write_inode

void get_inode(const Directory& dir,Inode& inode,const SuperBlock& super_block,const GroupDescriptor& gdt){

    fstream file(path,ios_base::in);
    file.seekg(BOOT_BLOCK_BYTE+super_block.blocks_per_group*(super_block.block_size)*dir.position.group+gdt.inode_table_position+dir.position.inode*sizeof(Inode),ios_base::beg);
    //cout<<"[-----]"<<BOOT_BLOCK_BYTE+super_block.blocks_per_group*(super_block.block_size)*dir.position.group+gdt.inode_table_position+dir.position.inode*sizeof(Inode)<<endl;
    file.read((char *)(&inode), sizeof(Inode));
    file.close();
    
}//get_inode

