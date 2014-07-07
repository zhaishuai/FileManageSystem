//
//  ACL.cpp
//  OS_IEXT2
//
//  Created by jimzhai on 14-1-8.
//  Copyright (c) 2014年 com.jimzhai. All rights reserved.
//

#include "ACL.h"


void __create_user(string& user_name,const string &user_group,Inode &parent_directory_inode, vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block ,const GroupDescriptor& gdt ){
    
    Inode dir_inode = parent_directory_inode;
    __open_folder(dir_inode, "usr", slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
    Directory dir;
    dir.file_size = super_block.block_size;
    dir.file_type = IEXT2_FT_USER;
    dir.name = user_name;
    dir.next = true;
    if(__check_name_exist(dir_inode, dir.name, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt))
        return ;
    
    if(!check_name(user_name)){
        cout<<"-bash: illegal user name."<<endl;
        return;
    }
    if(!check_name(user_group)){
        cout<<"-bash: illegal group name."<<endl;
        return;
    }
    string password1;
    string password2;
    bool judger=false;
    cout<<"please enter password: ";
    cin>>password1;
    judger = check_password(password1);
    while (!judger) {
        cout<<"please enter password: ";
        cin>>password1;
        judger = check_password(password1);
    }
    cout<<"please enter password again: ";
    cin>>password2;
    while(password1!=password2){
        cout<<"Twice to enter the password does not match,please enter again."<<endl;
        cout<<"please enter password: ";
        cin>>password1;
        judger = check_password(password1);
        while (!judger) {
            cout<<"please enter password: ";
            cin>>password1;
            judger = check_password(password1);
        }
        cout<<"please enter password again: ";
        cin>>password2;
    }
    User data;
    data.user_name = user_name;
    data.password = password1;
    data.group.push_back(user_group);
    cout<<"create user: "<<user_name<<" success."<<endl;
    dir.position = __alloc_inode(inode_slide_queue,inode_group ,super_block,gdt);
    write_file_to_folde(user_name,user_group,dir_inode, dir, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
    Inode newFile;
    newFile.i_used_blocks = 0;
    newFile.i_position = dir.position.inode;
    newFile.i_group = dir.position.group;
    //newFile.i_atime
    
    __alloc_blocks_and_regist_inode(user_name,user_group,slide_block_queue, big_block_queue, newFile, slide_group, dir, super_block, gdt);
    if(dir.file_type == IEXT2_FT_USER)
        write_user(slide_block_queue, big_block_queue, newFile, slide_group, dir, super_block, gdt, data);

    dir_inode = parent_directory_inode;
    __open_folder(dir_inode, "User", slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
    Directory i_dir;
    i_dir.name = user_name;
    i_dir.file_type = IEXT2_FT_DIR;
    i_dir.file_size = super_block.block_size;
    i_dir.next = true;
    __create_file(user_name,user_group,dir_inode, i_dir, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
    
}//__create_file

bool __get_user_info(Inode &parent_inode,const string &user_name,const SuperBlock& super_block,const GroupDescriptor& gdt,User &data){
    
    Directory cur_dir;
    
    fstream file(path,ios_base::in|ios_base::out);
    bool find = false;
    for (int i = 0 ;  i < parent_inode.i_used_blocks; i++ ) {
        __addressing(file, i, parent_inode, super_block, gdt);
        for(int j=0;j<super_block.block_size/sizeof(Directory);j++){
            file.read((char *)(&cur_dir), sizeof(Directory));
            if(cur_dir.next &&cur_dir.name == user_name&&cur_dir.file_type == IEXT2_FT_USER)
            {
                find = true;
                break;
            }
        }
        if (find) {
            break;
        }
    }
    if(!find)
    {
        file.close();
        return find;
    }
    User user;
    get_inode(cur_dir, parent_inode, super_block, gdt);
    
    __addressing(file, 0, parent_inode, super_block, gdt);
    int size=0;
    file>>data.user_name>>data.password>>size;
    string temp;
    for(int i=0;i<size;i++)
    {
        
        file>>temp;
        data.group.push_back(temp);
    }
    //cout<<"temp: "<<temp<<endl;
    file.close();
    return find;
}//__open_file


void write_user(vector<Slide_block>& slide_block_queue,vector<BigBlockLink>& big_block_queue,Inode& inode,unsigned int &slide_group,const Directory& dir,const SuperBlock& super_block,const GroupDescriptor& gdt,const User &data){
    
    fstream file(path,ios_base::in|ios_base::out);
    
    __addressing(file,0, inode, super_block, gdt);
    
    file<<data.user_name<<endl<<data.password<<endl<<(int)data.group.size();
    for(int i=0;i<data.group.size();i++)
    {
        file<<data.group[i]<<endl;
    }

}//write_string_data

bool check_password(const string& file_name){
    if(file_name.length()>20||file_name.length()<6)
        return false;
    for(int i=0;i<file_name.length();i++)
    {
        if(!((file_name.at(i)>='A'&&file_name.at(i)<='Z')||(file_name.at(i)>='a'&&file_name.at(i)<='z')||file_name.at(i)=='_'||file_name.at(i)=='.'||(file_name.at(i)>='0'&&file_name.at(i)<='9'))){
            return false;
        }
    }
    if(file_name[0]=='.')
        return false;
    return true;
}//check_password

bool check_name(const string& file_name){
    if(file_name.length()>20||file_name.length()<1)
        return false;
    for(int i=0;i<file_name.length();i++)
    {
        if(!((file_name.at(i)>='A'&&file_name.at(i)<='Z')||(file_name.at(i)>='a'&&file_name.at(i)<='z')||file_name.at(i)=='_'||file_name.at(i)=='.'||(file_name.at(i)>='0'&&file_name.at(i)<='9'))){
            return false;
        }
    }
    if(file_name[0]=='.')
        return false;
    return true;
}//check_name


bool check_write_permission(Inode& temp_inode,User& user_info,string& file_name){
    
    
    bool can = false;
    if (user_info.user_name == temp_inode.user_name) {
        if(temp_inode.access_table.user[1]=='w'){
            return true;
        }
    }else{
        for(int i=0;i<user_info.group.size();i++)
        {
            if(user_info.group[i] == "root")
            {
                return  true;
            }
            if(user_info.group[i] == temp_inode.user_group)
            {
                if(temp_inode.access_table.group[1]=='w'){
                    return true;
                }
            }
        }
    }
    if(!can){
        if(temp_inode.access_table.other[1]=='w'){
            return true;
        }
    }
    cout<<"-bash: "<<file_name<<" write permission deny."<<endl;
    return can;
}//check_write_permission

bool check_read_permission(Inode& temp_inode,User& user_info,string& file_name){
    bool can = false;
    if (user_info.user_name == temp_inode.user_name) {
        if(temp_inode.access_table.user[0]=='r'){
            return true;
        }
    }else{
        for(int i=0;i<user_info.group.size();i++)
        {
            if(user_info.group[i] == temp_inode.user_group)
            {
                if(user_info.group[i] == "root")
                    return true;
                
                if(temp_inode.access_table.group[0]=='r'){
                    return true;
                }
            }
        }
    }
    if(!can){
        if(temp_inode.access_table.other[0]=='r'){
            return true;
        }
    }
    cout<<"-bash: "<<file_name<<" read permission deny."<<endl;
    return can;
}//check_read_permission

