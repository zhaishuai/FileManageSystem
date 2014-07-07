//
//  Command.cpp
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-25.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#include "Command.h"


void execute_commands(){
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
    
    map<string,unsigned int> command_map;
    
    init_command_hash_map(command_map);
    
    cin_command(command_map, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt, root_inode);

}//execute_commands



void __get_SuperBlock_GDT_Inode(SuperBlock& super_block,GroupDescriptor& gdt,Inode& dir_inode){
    //char path[] = "/Users/jimzhai/Documents/project Place/OS_IEXT2/vitual_disk.iext2";

    fstream file(path,ios_base::in);
    file.seekg(BOOT_BLOCK_BYTE,ios_base::beg);
    file.read((char *)(&super_block), sizeof(SuperBlock));
    file.read((char *)(&gdt), sizeof(GroupDescriptor));
    file.seekg(gdt.inode_table_position+BOOT_BLOCK_BYTE,ios_base::beg);
    file.read((char *)(&dir_inode), sizeof(Inode));
    file.close();

}//__get_super_block_and_GDT

void __list_all_files_of_the_folder(const string& param,const Inode& inode, const SuperBlock &super_block,const GroupDescriptor& gdt){
    
    fstream file(path,ios_base::in);
    int count = 0;
    for(int i = 0; i < inode.i_used_blocks;i++)
    {
        __addressing(file, i, inode, super_block, gdt);
        Directory dir;
        for(int j = 0 ; j < super_block.block_size/sizeof(Directory);j++){
            //file.seekg((dir.next-1)*sizeof(Directory),ios_base::cur);
            file.read((char *)(&dir), sizeof(Directory));
            if(dir.next){
                Inode temp_inode = inode;
                get_inode(dir, temp_inode, super_block, gdt);
                if(param=="none")
                    cout<<setiosflags(ios::left)<<setw(20)<<dir.name;
                else if(param == "-a")
                {
                    cout<<setiosflags(ios::left)<<setw(6)<<temp_inode.i_position<<setw(4)<<temp_inode.access_table.user<<setw(4)<<temp_inode.access_table.group<<setw(4)<<temp_inode.access_table.other<<" "<<setw(10)<<temp_inode.user_name<<setw(10)<<temp_inode.user_group<<setw(10)<<dir.name;
                }else if(param == "-i")
                {
                    cout<<setiosflags(ios::left)<<setw(6)<<temp_inode.i_position<<setw(20)<<dir.name;
                }else if(param == "-l")
                {
                    cout<<setiosflags(ios::left)<<temp_inode.access_table.user<<" "<<setw(4)<<temp_inode.access_table.group<<setw(4)<<temp_inode.access_table.other<<" "<<setw(7)<<temp_inode.user_name<<setw(7)<<temp_inode.user_group<<setw(10)<<dir.name;
                }
                count++;
                if((count)%3==0)
                    cout<<endl;
            }
        }
    }
    if(count%3)
        cout<<endl;
    file.close();

}//__list_all_files_of_the_folder



void __create_file(const string& user_name,const string&user_group,Inode &parent_directory_inode,Directory& dir, vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block ,const GroupDescriptor& gdt ,string data){
    
    if(__check_name_exist(parent_directory_inode, dir.name, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt))
        return ;
    
    dir.position = __alloc_inode(inode_slide_queue,inode_group ,super_block,gdt);
    //cout<<"create "<<dir.name<<endl;
    //cout<<"dir.position.inode:"<<dir.position.inode<<endl;
    write_file_to_folde(user_name,user_group,parent_directory_inode, dir, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
    //cout<<"*********>"<<parent_directory_inode.i_used_blocks<<endl;
    Inode newFile;
    newFile.i_used_blocks = 0;
    newFile.i_position = dir.position.inode;
    newFile.i_group = dir.position.group;
    newFile.access_table = parent_directory_inode.access_table;
    //newFile.i_atime
    
    __alloc_blocks_and_regist_inode(user_name,user_group,slide_block_queue, big_block_queue, newFile, slide_group, dir, super_block, gdt);
    if(dir.file_type == IEXT2_FT_REG_FILE)
        write_string_data(user_name,user_group,slide_block_queue, big_block_queue, newFile, slide_group, dir, super_block, gdt, data);
    


}//__create_file

void __rename(User& user_info,Inode &parent_directory_inode,string& old_name, vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block,const GroupDescriptor& gdt,string& new_name){
    
    Directory cur_dir;
    cur_dir.name = new_name;
    bool find = false;
    
    fstream file(path,ios_base::in|ios_base::out);
    for (int i = 0 ;  i < parent_directory_inode.i_used_blocks; i++ ) {
        __addressing(file, i, parent_directory_inode, super_block, gdt);
        for(int j=0;j<super_block.block_size/sizeof(Directory);j++){
            file.read((char *)(&cur_dir), sizeof(Directory));
            if(cur_dir.next &&cur_dir.name == old_name)
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
        cout<<"can't find the file "<<old_name<<endl;
        file.close();
        return ;
    }
    Inode temp_inode = parent_directory_inode;
    get_inode(cur_dir, temp_inode, super_block, gdt);
    if(!check_write_permission(temp_inode, user_info, old_name))
        return ;
    
    if(__check_name_exist(parent_directory_inode, new_name, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt)){
        file.close();
        return;
    }
    
    

    file.seekg(-sizeof(Directory),ios_base::cur);
    cur_dir.name = new_name;
    //cout<<"--->"<<cur_dir.name<<endl;
    file.write((char *)(&cur_dir), sizeof(Directory));
    /* ! need to update inode information ! */
    file.close();
    
    
}//__rename

bool __check_name_exist(Inode &parent_directory_inode,string &dir_name, vector<Slide_block> &slide_block_queue,vector<BigBlockLink> &big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block,const GroupDescriptor& gdt){
    Directory cur_dir;
    fstream file_temp(path,ios_base::in);
    for (int i = 0 ;  i < parent_directory_inode.i_used_blocks; i++ ) {
        __addressing(file_temp, i, parent_directory_inode, super_block, gdt);
        for(int j=0;j<super_block.block_size/sizeof(Directory);j++){
            file_temp.read((char *)(&cur_dir), sizeof(Directory));
            if(cur_dir.next &&cur_dir.name == dir_name)
            {
                cout<<dir_name<<" is existing,please enter another one!"<<endl;
                file_temp.close();
                return true;
            }
            
        }
    }
    file_temp.close();
    return false;
}//__check_name_exist


bool __open_folder(Inode &parent_inode,const string &folder_name, vector<Slide_block> &slide_block_queue,vector<BigBlockLink> &big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block,const GroupDescriptor& gdt){

    Directory cur_dir;
    
    fstream file(path,ios_base::in|ios_base::out);
    bool find = false;
    for (int i = 0 ;  i < parent_inode.i_used_blocks; i++ ) {
        __addressing(file, i, parent_inode, super_block, gdt);
        for(int j=0;j<super_block.block_size/sizeof(Directory);j++){
            file.read((char *)(&cur_dir), sizeof(Directory));
            if(cur_dir.next &&cur_dir.name == folder_name&&cur_dir.file_type == IEXT2_FT_DIR)
            {
                find = true;
                break;
            }
            
        }
        if (find) {
            break;
        }
    }
    //cout<<"cd inode --->>"<<cur_dir.position.inode<<"   groupth-->"<<cur_dir.position.group<<endl;
    if (find) {
        get_inode(cur_dir, parent_inode, super_block, gdt);
    }
    file.close();
    return find;
}//__open_folder

bool __open_file(Inode &parent_inode,const string &file_name,const SuperBlock& super_block,const GroupDescriptor& gdt,string &data){

    Directory cur_dir;
    
    fstream file(path,ios_base::in|ios_base::out);
    bool find = false;
    for (int i = 0 ;  i < parent_inode.i_used_blocks; i++ ) {
        __addressing(file, i, parent_inode, super_block, gdt);
        for(int j=0;j<super_block.block_size/sizeof(Directory);j++){
            file.read((char *)(&cur_dir), sizeof(Directory));
            if(cur_dir.next &&cur_dir.name == file_name&&cur_dir.file_type == IEXT2_FT_REG_FILE)
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
        cout<<"can't find the file "<<file_name<<endl;
        file.close();
        return find;
    }
    string temp="";
    get_inode(cur_dir, parent_inode, super_block, gdt);
    
    for(int i = 0 ; i < parent_inode.i_used_blocks ; i++){
        __addressing(file, i, parent_inode, super_block, gdt);
        file>>temp;
        //cout<<"temp: "<<temp<<endl;
        data += temp;
    }
    file.close();
    return find;
}//__open_file

bool delete_file(User& user_info,const string& file_name,const Inode &parent_inode,Inode& cur_inode,vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block,const GroupDescriptor& gdt){

        
        
    bool can = true;
    Directory cur_dir;
    fstream file(path,ios_base::in|ios_base::out);
    bool find = false;
    for (int i = 0 ;  i < parent_inode.i_used_blocks; i++ ) {
        __addressing(file, i, parent_inode, super_block, gdt);
        for(int j=0;j<super_block.block_size/sizeof(Directory);j++){
            file.read((char *)(&cur_dir), sizeof(Directory));
            if(cur_dir.next &&cur_dir.name == file_name)
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
        cout<<"can't find the file "<<file_name<<endl;
        file.close();
        return find;
    }
    
    if(cur_dir.file_type == IEXT2_FT_DIR)
    {
        Inode temp_inode = parent_inode;
        get_inode(cur_dir, temp_inode, super_block, gdt);
        if(cur_inode.i_position == temp_inode.i_position){
            cout<<"-bash: "<<file_name<<" is using."<<endl;
            return false;
        }
        if(!check_write_permission(temp_inode, user_info, cur_dir.name))
            return false;
        
        fstream file(path,ios_base::in);
        for(int i = 0; i < temp_inode.i_used_blocks;i++)
        {
            __addressing(file, i, temp_inode, super_block, gdt);
            Directory dir;
            for(int i = 0 ; i < super_block.block_size/sizeof(Directory);i++){
                file.read((char *)(&dir), sizeof(Directory));
                if(dir.next){
                    can = delete_file(user_info,dir.name,temp_inode,cur_inode,slide_block_queue,big_block_queue ,inode_slide_queue,inode_group,slide_group, super_block,gdt);
                    if(!can){
                        return false;
                    }
                }
            }
        }
        file.close();
        
        
    }
    
    Inode inode;
    
    get_inode(cur_dir, inode, super_block, gdt);
    if(!check_write_permission(inode, user_info, cur_dir.name)){
        return false;
    }
    
    file.seekg(-sizeof(Directory),ios_base::cur);
    cur_dir.next=false;
    file.write((char *)(&cur_dir), sizeof(Directory));

    //cout<<"i am run"<<endl;
        for(int i=0 ; i < inode.i_used_blocks ; i++){
            //cout<<"blockth:----->>>"<<inode.i_pointer[i].i_pointer/super_block.block_size<<endl;
            for(int j=0 ; j < slide_block_queue.size() ; j++){
                
                if(inode.i_pointer[i].i_pointer/super_block.block_size<slide_block_queue[j].start&&inode.i_pointer[i].groupth<=slide_block_queue[j].start_groupth){
                    bool del = false;
                    if(inode.i_pointer[i].groupth<slide_block_queue[j].start_groupth){
                        vector<Slide_block>::iterator pos = slide_block_queue.begin()+j;
                        Slide_block sb;
                        sb.start = inode.i_pointer[i].i_pointer/super_block.block_size;
                        sb.count = 1;
                        sb.start_groupth = inode.i_pointer[i].groupth;
                        if(inode.i_pointer[i].i_pointer/super_block.block_size == super_block.blocks_per_group-1)
                            sb.end_groupth = inode.i_pointer[i].groupth+1;
                        else
                            sb.end_groupth = inode.i_pointer[i].groupth;
                        slide_block_queue.insert(pos, sb);
                        del = true;

                    }else if(inode.i_pointer[i].i_pointer/super_block.block_size+1<slide_block_queue[j].start&&inode.i_pointer[i].groupth == slide_block_queue[j].start_groupth){
                        vector<Slide_block>::iterator pos = slide_block_queue.begin()+j;
                        Slide_block sb;
                        sb.start = inode.i_pointer[i].i_pointer/super_block.block_size;
                        sb.count = 1;
                        sb.start_groupth = inode.i_pointer[i].groupth;
                        if(inode.i_pointer[i].i_pointer/super_block.block_size == super_block.blocks_per_group-1)
                            sb.end_groupth = inode.i_pointer[i].groupth+1;
                        else
                            sb.end_groupth = inode.i_pointer[i].groupth;
                        slide_block_queue.insert(pos, sb);
                        del = true;
                    }else if(inode.i_pointer[i].i_pointer/super_block.block_size+1 == slide_block_queue[j].start&&inode.i_pointer[i].groupth == slide_block_queue[j].start_groupth){
                        
                        slide_block_queue[j].start--;
                        slide_block_queue[j].count++;
                        del = true;
                    }
                    if(del){
                        file.seekg(BOOT_BLOCK_BYTE+super_block.blocks_per_group*(super_block.block_size)*inode.i_pointer[i].groupth+gdt.block_bitmap_position,ios_base::beg);
                        BinaryBlock *block_bit_map = new BinaryBlock(super_block.block_size);
                        block_bit_map->read_a_piece_of_block(file,inode.i_pointer[i].i_pointer/super_block.block_size);
                        block_bit_map->reset(inode.i_pointer[i].i_pointer/super_block.block_size);
                        block_bit_map->write_a_piece_of_block(file, inode.i_pointer[i].i_pointer/super_block.block_size);
                        delete block_bit_map;
                        //cout<<"size of slide_block:"<<slide_block_queue.size()<<endl;
                        break;
                    }
                }//if
            
            }//for
            
        }//for
        
        for(int i=0;i<inode_slide_queue.size();i++){
            if(inode.i_position<inode_slide_queue[i].start&&inode.i_group<=inode_slide_queue[i].start_groupth){
                bool del = false;
                if(inode.i_group<inode_slide_queue[i].start_groupth){
                    vector<InodeSlideBlock>::iterator pos = inode_slide_queue.begin()+i;
                    InodeSlideBlock isb;
                    isb.start = inode.i_position;
                    isb.count = 1;
                    isb.start_groupth = inode.i_group;
                    if(inode.i_group == gdt.inode_table_position-gdt.inode_bitmap_position-1)
                        isb.end_groupth = inode.i_group+1;
                    else
                        isb.end_groupth = inode.i_group;
                    inode_slide_queue.insert(pos, isb);
                    del = true;
                    
                }else if(inode.i_position+1 < inode_slide_queue[i].start&&inode.i_group == inode_slide_queue[i].start_groupth){
                    vector<InodeSlideBlock>::iterator pos = inode_slide_queue.begin()+i;
                    InodeSlideBlock isb;
                    isb.start = inode.i_position;
                    isb.count = 1;
                    isb.start_groupth = inode.i_group;
                    if(inode.i_group == gdt.inode_table_position-gdt.inode_bitmap_position-1)
                        isb.end_groupth = inode.i_group+1;
                    else
                        isb.end_groupth = inode.i_group;
                    inode_slide_queue.insert(pos, isb);
                    del = true;
                    
                }else if(inode.i_position+1 == inode_slide_queue[i].start&&inode.i_group == inode_slide_queue[i].start_groupth){
                    inode_slide_queue[i].start--;
                    inode_slide_queue[i].count++;
                    del = true;
                }//if
                if(del){
                file.seekg(BOOT_BLOCK_BYTE+super_block.blocks_per_group*(super_block.block_size)*inode.i_group+gdt.inode_bitmap_position,ios_base::beg);
                    BinaryBlock *inode_bit_map = new BinaryBlock((gdt.inode_table_position-gdt.inode_bitmap_position)/8);
                    inode_bit_map->read_a_piece_of_block(file,inode.i_position);
                    inode_bit_map->reset(inode.i_position);
                    inode_bit_map->write_a_piece_of_block(file,inode.i_position);
                    delete inode_bit_map;
                    break;
                }
            }//if
        }//for
        
        
   // }//if
    
    
    
    
    
    file.close();
    return find;
}//delete_file


void copy_file(const string& user_name,const string&user_group,const string& file_name,const string& new_file_name,const string&destination_file_name,const Inode &parent_inode,const Inode& new_folder_inode,vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block,const GroupDescriptor& gdt){

    Directory cur_dir;
    fstream file(path,ios_base::in|ios_base::out);
    bool find = false;
    for (int i = 0 ;  i < parent_inode.i_used_blocks; i++ ) {
        __addressing(file, i, parent_inode, super_block, gdt);
        for(int j=0;j<super_block.block_size/sizeof(Directory);j++){
            file.read((char *)(&cur_dir), sizeof(Directory));
            if(cur_dir.next &&cur_dir.name == file_name)
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
        cout<<"can't find the file "<<file_name<<endl;
        file.close();
        return ;
    }
    
    Inode temp_destination_inode = new_folder_inode;
    if(destination_file_name != "/")
    {
        bool temp = __open_folder(temp_destination_inode, destination_file_name, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
        if(!temp)
        {
            cout<<"-bash: cp: "<<destination_file_name<<": No such file or directory"<<endl;
            return;
        }
    }
    
    Inode temp_from_inode = parent_inode;
    get_inode(cur_dir,temp_from_inode, super_block, gdt);
    if(temp_from_inode.i_position == temp_destination_inode.i_position){
        cout<<"You can’t copy “"<<cur_dir.name<<"” at this location because you can’t copy an item into itself."<<endl;
        return;
    }
    
    if(cur_dir.file_type == IEXT2_FT_DIR)
    {
        Directory dir;
        dir.file_type = cur_dir.file_type;
        dir.file_size = cur_dir.file_size;
        dir.name = cur_dir.name;
        dir.next = true;
        temp_destination_inode.access_table = parent_inode.access_table;
        __create_file(parent_inode.user_name,parent_inode.user_group,temp_destination_inode, dir, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
        
        fstream file(path,ios_base::in);
        for(int i = 0; i < temp_from_inode.i_used_blocks;i++)
        {
            __addressing(file, i, temp_from_inode, super_block, gdt);
            Directory dir;
            for(int i = 0 ; i < super_block.block_size/sizeof(Directory);i++){
                file.read((char *)(&dir), sizeof(Directory));
                if(dir.next){
                    copy_file(user_name,user_group,dir.name, dir.name,cur_dir.name, temp_from_inode, temp_destination_inode, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                }
            }
        }
        file.close();
    }else if(cur_dir.file_type == IEXT2_FT_REG_FILE){
        string data=" ";
        Inode temp = parent_inode;
        __open_file(temp, cur_dir.name, super_block, gdt, data);
        Directory dir;
        dir.file_type = cur_dir.file_type;
        dir.file_size = cur_dir.file_size;
        dir.name = new_file_name;
        dir.next = true;
        temp_destination_inode.access_table = parent_inode.access_table;
        __create_file(parent_inode.user_name,parent_inode.user_group,temp_destination_inode, dir, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt,data);
    }
}//copy_file

void move_file(User& user_info,const string& user_name,const string&user_group,const Inode& from_inode,const Inode& to_inode,const string &from_file_name,const string&destination_file_name,vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block,const GroupDescriptor& gdt){
    string str_name = from_file_name;
    Inode ti = from_inode;
    if(!check_read_permission(ti, user_info, str_name))
        return ;
    if(!check_write_permission(ti, user_info, str_name))
        return;
    
    Directory cur_dir;
    fstream file(path,ios_base::in|ios_base::out);
    bool find = false;
    for (int i = 0 ;  i < from_inode.i_used_blocks; i++ ) {
        __addressing(file, i, from_inode, super_block, gdt);
        for(int j=0;j<super_block.block_size/sizeof(Directory);j++){
            file.read((char *)(&cur_dir), sizeof(Directory));
            if(cur_dir.next &&cur_dir.name == from_file_name)
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
        cout<<"can't find the file "<<from_file_name<<endl;
        file.close();
        return ;
    }
    cur_dir.next = false;
    file.seekg(-sizeof(Directory),ios_base::cur);
    file.write((char *)(&cur_dir), sizeof(Directory));
    cur_dir.next = true;
    Inode temp_destination_inode = to_inode;
//    if(destination_file_name != "/")
//    {
//        bool temp = __open_folder(temp_destination_inode, destination_file_name, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
//        if(!temp)
//        {
//            cout<<"-bash: mv: "<<destination_file_name<<": No such file or directory"<<endl;
//            return;
//        }
//    }
    string name = from_file_name;
    //cur_dir.name = name;
    if(!check_write_permission(temp_destination_inode, user_info,name))
        return ;

    if(__check_name_exist(temp_destination_inode, name, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt))
        return;
    write_file_to_folde(user_name,user_group,temp_destination_inode, cur_dir, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
    
    
}//move_file

