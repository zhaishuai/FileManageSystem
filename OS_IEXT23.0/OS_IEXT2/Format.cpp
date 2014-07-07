//
//  Format.cpp
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-25.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#include "Format.h"


bool format_virual_disk(){

    s_int block_size = BLOCKSIZE;
    un_int average_file_size = AVERAGE_FILE_SIZE;
    l_int disk_size = DISC_SIZE;

    
    cout<<"use the default mode?[y/n]";
    char mode;
    cin>>mode;
    if (mode == 'n') {
        cout<<"enter the disk size?[y/n]";
        cin>>mode;
        if (mode == 'y') {
            cout<<"please enter the disk size(KB):";
            cin>>disk_size;
        }
        cout<<"enter the block size?[y/n]";
        cin>>mode;
        if (mode == 'y') {
            cout<<"please enter the block size(KB):";
            cin>>block_size;
        }
        cout<<"enter the average file size?[y/n]";
        cin>>mode;
        if (mode == 'y') {
            cout<<"please enter the average_file_size size(KB):";
            cin>>average_file_size;
        }

    }
    __format_iext2(block_size, average_file_size, disk_size);

    return true;//__format(block_size,average_file_size,disk_size);

}//format_virual_disk

bool __format_iext2(s_int block_size,un_int average_file_size,l_int disk_size){
    //char path[] = "/Users/jimzhai/Documents/project Place/MyFileManageSystem/vitual_disk.iext2";

    GroupDescriptor *group_descriptor_groups = new GroupDescriptor[(unsigned int)(disk_size/(8192*block_size*block_size))];
    __group_descriptor_register(block_size, average_file_size, disk_size, group_descriptor_groups);

    SuperBlock super_block;
    __super_block_register(block_size, average_file_size, disk_size,group_descriptor_groups, super_block);

    //inital defualt directorys
    super_block.inodes_count -= 10;
    super_block.blocks_count --;
//    group_descriptor_groups[0].free_inodes_in_group -= 10;
//    group_descriptor_groups[0].free_blocks_in_group --;


    //write boot_block
    //char path[] = "/Users/jimzhai/Documents/project Place/OS_IEXT2/vitual_disk.iext2";

    fstream file(path,ios_base::in|ios_base::out|ios::trunc);
    file.seekg(HEAH_POSITION,ios_base::beg);
    BinaryBlock *boot_block = new BinaryBlock(BOOT_BLOCK_BYTE);
    //file.write((char *)(boot_block),sizeof(*boot_block));
    boot_block->write_block(file);
    delete boot_block;
    BinaryBlock *block_bit_map = new BinaryBlock(super_block.block_size);
    BinaryBlock *inode_bit_map = new BinaryBlock(group_descriptor_groups[0].inode_table_position - group_descriptor_groups[0].inode_bitmap_position);
    BinaryBlock *block = new BinaryBlock(super_block.block_size);
    for(int i = 0,used_blocks = group_descriptor_groups[0].first_data_block_position/super_block.block_size ; i < super_block.block_groups; i++)
    {
        //write super_block
        file.write((char *)(&super_block),sizeof(super_block));

        //write group_descriptor_groups
        file.write((char *)(group_descriptor_groups), sizeof(*group_descriptor_groups)*super_block.block_groups);

        //write block_bit_map
        
        block_bit_map->clear();
        for(int j = 0 ; j < used_blocks;j++)
            block_bit_map->set(j);
        if (i == 0) {
            block_bit_map->set(used_blocks);    //used by defualt directory
            
        }

        //file.write((char *)(block_bit_map),super_block.block_size);
        block_bit_map->write_block(file);
        
        

        //write inode_bit_map

        inode_bit_map->clear();
        if(i == 0){
            inode_bit_map->set(0);  //used by default directory
        }
        //file.write((char *)(inode_bit_map), super_block.inodes_per_group/8);
        inode_bit_map->write_block(file);
        

        //write inode_table
        Inode *inode_table = new Inode[super_block.inodes_per_group];
        if (i == 0) {
            inode_table[0].i_group = 0;
            inode_table[0].i_pointer[0].i_pointer = group_descriptor_groups[0].first_data_block_position;
            inode_table[0].i_used_blocks = 1;
            inode_table[0].user_group = "root";
            inode_table[0].user_name = "root";
            Access_table default_access_table{"rwx","r-x","r--"};
            inode_table[0].access_table = default_access_table;

        }
        file.write((char *)(inode_table), sizeof(*inode_table)*super_block.inodes_per_group);
//        cout<<"----------->:"<<inode_table[i].i_pointer[0].i_pointer<<endl;
//        cout<<"sizeof(inode_table):"<<sizeof(*inode_table)*super_block.inodes_per_group<<endl;
//        cout<<"sizeof(Inode):"<<sizeof(Inode)<<endl;

        //wirte data_block
        file.seekg(group_descriptor_groups[0].first_data_block_position - group_descriptor_groups[0].inode_table_position - sizeof(*inode_table)*super_block.inodes_per_group,ios_base::cur);
        
        block->clear();
        for (int j = 0 ; j <  super_block.blocks_per_group - used_blocks;j++) {
            if (i==0&&j==0) {   //create default folders
                Directory directory;
                directory.position.group = -1;
                directory.position.inode = -1;
                directory.file_size = -1;
                directory.file_type = IEXT2_FT_VOID;
                directory.name = "";
                directory.next = false;
                for(int i = 0; i<super_block.block_size/sizeof(Directory);i++){
                    
                    file.write((char *)(&directory), sizeof(directory));
                }
                
                file.seekg(super_block.block_size%sizeof(Directory),ios_base::cur);
                
            }else {
                //file.write((char *)(block), super_block.block_size);
                block->write_block(file);
            }
        }
        

    }
    delete block_bit_map;
    delete inode_bit_map;
    delete block;
    file.close();
    
    
    __create_default_folde();
    
    
    return true;

}//__format_iext2

void __super_block_register(s_int block_size,un_int average_file_size,l_int disk_size ,const GroupDescriptor *group_descriptor_groups, SuperBlock& super_block){

    super_block.inodes_count = (unsigned int)(disk_size/average_file_size);
    super_block.blocks_count = (unsigned int)(disk_size/block_size);
    super_block.block_groups = (unsigned int)(disk_size/(8192*block_size*block_size));
    super_block.blocks_per_group = (unsigned short int)(8192*block_size);
    super_block.inodes_per_group = (unsigned int)((8192*block_size*block_size)/average_file_size);
    super_block.block_size = block_size*1024;
    super_block.free_blocks_count = super_block.blocks_count - (group_descriptor_groups[0].first_data_block_position/super_block.block_size)*2;
    super_block.free_inodes_count = super_block.inodes_count;

//    cout<<"super_block.inodes_count:"<<super_block.inodes_count<<endl;
//    cout<<"super_block.blocks_count:"<<super_block.blocks_count<<endl;
//    cout<<"super_block.block_groups:"<<super_block.block_groups<<endl;
//    cout<<"super_block.blocks_per_group:"<<super_block.blocks_per_group<<endl;
//    cout<<"super_block.inodes_per_group:"<<super_block.inodes_per_group<<endl;
//    cout<<"super_block.block_size:"<<super_block.block_size<<endl;
//    cout<<"super_block.free_blocks_count:"<<super_block.free_blocks_count<<endl;
//    cout<<"super_block.free_inodes_count:"<<super_block.free_inodes_count<<endl;

}//__super_block_register

void __group_descriptor_register(const s_int _block_size,const un_int average_file_size,const l_int disk_size ,GroupDescriptor *group_descriptor_groups){
    unsigned int block_groups = (unsigned int)(disk_size/(8192*_block_size*_block_size));
    unsigned int inodes_per_group = (unsigned int)((8192*_block_size*_block_size)/average_file_size);
    unsigned short int block_size = _block_size*1024;
    for(int i = 0 ; i < block_groups ; i++)
    {
        group_descriptor_groups[i].block_bitmap_position = sizeof(SuperBlock) + sizeof(*group_descriptor_groups)*block_groups;
        group_descriptor_groups[i].inode_bitmap_position = group_descriptor_groups[i].block_bitmap_position + block_size;
        group_descriptor_groups[i].inode_table_position = group_descriptor_groups[i].inode_bitmap_position + inodes_per_group/8;
        if (inodes_per_group%8) {
            group_descriptor_groups[i].inode_table_position++;
        }
        group_descriptor_groups[i].first_data_block_position = group_descriptor_groups[i].inode_table_position + inodes_per_group*sizeof(Inode);
        unsigned short int temp = group_descriptor_groups[i].first_data_block_position%block_size;
        if(temp)
            group_descriptor_groups[i].first_data_block_position += block_size - temp;
    }

//    cout<<"sizeof(GroupDescriptor):"<<sizeof(GroupDescriptor)<<endl<<
//    "sizeof(SuperBlock):"<<sizeof(SuperBlock)<<endl;
//    for (int i = 0 ; i < block_groups; i++) {
//        cout<<"group_descriptor["<<i<<"].block_bitmap_position:"<<group_descriptor_groups[i].block_bitmap_position<<endl<<
//        "group_descriptor["<<i<<"].inode_bitmap_position:"<<group_descriptor_groups[i].inode_bitmap_position<<endl
//        <<"group_descriptor["<<i<<"].inode_table_position:"<<group_descriptor_groups[i].inode_table_position<<endl
//        <<"group_descriptor["<<i<<"].first_data_block_position:"<<group_descriptor_groups[i].first_data_block_position<<endl;
//    }
    
}//__group_descriptor_register

void __create_default_folde()
{
    SuperBlock super_block;
    GroupDescriptor gdt;
    Inode dir_inode,root_inode;
    vector<Slide_block> slide_block_queue;
    vector<BigBlockLink> big_block_queue;
    vector<InodeSlideBlock>inode_slide_queue;
    __get_SuperBlock_GDT_Inode(super_block,gdt,dir_inode);
    unsigned int slide_group=0,inode_group=0;
    
    loading_slide_block(slide_block_queue,big_block_queue, slide_group,super_block, gdt);
    
    loading_big_block_link(big_block_queue, slide_group, super_block, gdt);
    
    loading_inode_slide_block(inode_slide_queue, inode_group, super_block, gdt);
    
    loading_root_inode(root_inode, super_block, gdt);
    string file_name[9] = {"System","Libary","User","usr","bin","core","ect","var","Network"};
    string user_name = "root";
    string user_group = "root";
    Access_table default_access_table{"rwx","r-x","r--"};
    root_inode.access_table = default_access_table;
    for (int i = 0 ; i < 9 ; i++){
        Directory i_dir;
        i_dir.name = file_name[i];
        i_dir.file_type = IEXT2_FT_DIR;
        i_dir.file_size = super_block.block_size;
        i_dir.next = true;
        __create_file(user_name,user_group,root_inode, i_dir, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
    }
    
    dir_inode = root_inode;
    

    
    __create_user(user_name, user_group, dir_inode, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
    cout<<"format finish."<<endl;
}//__create_default_folde


