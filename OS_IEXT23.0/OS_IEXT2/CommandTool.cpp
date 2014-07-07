//
//  CommandTool.cpp
//  OS_IEXT2
//
//  Created by jimzhai on 13-12-10.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#include "CommandTool.h"

void string_trim(string& str){

    for (int i = 0 ; i < str.length() ; i++ ){
        if(str.at(i) == ' '){
            str.replace(i, 1, "");
            i--;
        }else{
            break;
        }
    }//for
    
    for (int i = (int)str.length() - 1 ; i >= 0 ; i-- ){
        if(str.at(i) == ' '){
            str.replace(i, 1, "");
        }else{
            break;
        }
    }//for
    
}//string_trim

void init_command_hash_map(map<string, unsigned int> &command_map){

    command_map["mkdir"] = MKDIR;
    command_map["vim"] = VIM;
    command_map["mv"] = MV;
    command_map["cd"] = CD;
    command_map["ls"] = LS;
    command_map["shutdown"] = SHUTDOWN;
    command_map["rm"] = DELETE;
    command_map["cp"] = COPY;
    command_map["rename"] = RENAME;
    command_map["bindex"] = SHOW_SLIDE_INDEX;
    command_map["iindex"] = SHOW_INODE_INDEX;
    command_map["pwd"] = PWD;
    command_map["useradd"] = CREATEUSER;
    command_map["adminadd"] = ADMINADD;
    command_map["rootadd"] = ROOTADD;
    command_map["chmod"] = CHANGEPERMISSION;
    command_map["rmuser"] = REMOVEUSER;
    command_map["format"] = FORMAT;
}//init_command_hash_map

void cin_command(map<string, unsigned int> &command_map,vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block ,const GroupDescriptor& gdt,Inode root_inode){
    
    Inode inode = root_inode;
    //vector<Inode> inode_path;
    vector<I_Path> ipath;
    I_Path i_path = {root_inode,"/"};
    ipath.push_back(i_path);
    
    
    if(!__open_folder(inode, "usr", slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt)){
        Directory i_dir;
        i_dir.name = "usr";
        i_dir.file_type = IEXT2_FT_DIR;
        i_dir.file_size = super_block.block_size;
        i_dir.next = true;
        __create_file("root","root",inode, i_dir, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
    }
    string user_name;
    User user_info;
    {
        bool judger_name = false;
        bool judger_password = false;
        while(!(judger_name&&judger_password)){
            cout<<"please enter user name: ";
            cin>>user_name;
            Inode t = inode;
            judger_name = __get_user_info(t,user_name,super_block,gdt,user_info);
            cout<<"please enter password: ";
            string password;
            cin>>password;
            
            if(password!=user_info.password)
                judger_password = false;
            else
                judger_password = true;
        }
    }
    inode = root_inode;
    while(!__open_folder(inode, "User", slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt)){
        Directory i_dir;
        i_dir.name = "User";
        i_dir.file_type = IEXT2_FT_DIR;
        i_dir.file_size = super_block.block_size;
        i_dir.next = true;
        __create_file("root","root",inode, i_dir, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
        I_Path i_path = {inode,"User"};
        ipath.push_back(i_path);
    }
    i_path.name = "User";
    i_path.inode = inode;
    ipath.push_back(i_path);
    while(!__open_folder(inode, user_name, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt)){
        Directory i_dir;
        i_dir.name = user_name;
        i_dir.file_type = IEXT2_FT_DIR;
        i_dir.file_size = super_block.block_size;
        i_dir.next = true;
        __create_file("root","root",inode, i_dir, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);

    }
    i_path.name = user_name;
    i_path.inode = inode;
    ipath.push_back(i_path);
    getchar();
    while (true) {
        string command;
        cout<<"Mac-mini:"<<ipath[ipath.size()-1].name<<" "<<user_name<<"$ ";
        getline(cin,command);
        string_trim(command);
        vector<string> command_vector;
        vector<string> param_vector;
        
        stringstream stream_command(command);
        string sub_str;
        
        while(getline(stream_command,sub_str,' ')){
            if(sub_str.find('-'))
                command_vector.push_back(sub_str);
            else
                param_vector.push_back(sub_str);
        }
        
        
        if(command_vector.size()==0)
            continue;
        
        if (command_map.find(command_vector[0]) == command_map.end()) {
            cout<<"-bash: "<<command_vector[0]<<": command not found"<<endl;
            continue;
        }
        
        switch (command_map[command_vector[0]]) {
            case MKDIR:{
                
                if(command_vector.size()<=1){
                    cout<<"usage: mkdir [-pv] [-m mode] directory ..."<<endl;
                }


                for(int i = 1; i < command_vector.size() ; i++){
                    Inode temp_inode = inode;
                    string temp_str = __temp_cd(temp_inode, root_inode, command_vector[i], ipath, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                    if(temp_str == "")
                    {
                        cout<<"-bash: mkdir: "<<command_vector[i]<<": No such file or directory"<<endl;
                        break;
                    }
                    if(!check_file_name(temp_str))
                    {
                        cout<<"-bash: mkdir: illegal file name: "<<temp_str<<endl;
                        break;
                    }
                    if(!check_write_permission(temp_inode, user_info, temp_str))
                        break;
                    command_vector[i]=temp_str;
                    Directory i_dir;
                    i_dir.name = command_vector[i];
                    i_dir.file_type = IEXT2_FT_DIR;
                    i_dir.file_size = super_block.block_size;
                    i_dir.next = true;
                    __create_file(user_info.user_name,user_info.group[0],temp_inode, i_dir, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                }
            }
                break;
            case VIM:
            {
                if(command_vector.size()<=1){
                    cout<<"usage: mkdir [-pv] [-m mode] directory ..."<<endl;
                    break;
                }
                
                Inode temp_inode = inode;
                string file_name = __temp_cd(temp_inode, root_inode, command_vector[1], ipath, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                if(file_name == "")
                {
                    cout<<"-bash: vim: "<<command_vector[1]<<": No such file or directory"<<endl;
                    break;
                }
                
                if(!check_file_name(file_name))
                {
                    cout<<"-bash: mkdir: illegal file name: "<<file_name<<endl;
                    break;
                }

                string data;
                Directory cur_dir;
                fstream file(path,ios_base::in|ios_base::out);
                bool find = false;
                for (int i = 0 ;  i < temp_inode.i_used_blocks; i++ ) {
                    __addressing(file, i, temp_inode, super_block, gdt);
                    for(int j=0;j<super_block.block_size/sizeof(Directory);j++){
                        file.read((char *)(&cur_dir), sizeof(Directory));
                        if(cur_dir.name == file_name&&cur_dir.next)
                        {
                            find = true;
                            break;
                        }
                        
                    }
                    if (find) {
                        break;
                    }
                }
                string result="";
                Inode t_inode = temp_inode;
                if(find){
                    if(!check_read_permission(t_inode, user_info, file_name))
                        break;
                    if(!__open_file(t_inode, file_name, super_block, gdt, result))
                        break;
                    cout<<result<<endl;
                    cin>>data;
                    getchar();
                }


                if(!find){
                    if(!check_write_permission(temp_inode,user_info, file_name))
                        break;
                    cin>>data;
                    getchar();
                    Directory i_dir4;
                    i_dir4.name = file_name;
                    i_dir4.file_type = IEXT2_FT_REG_FILE;
                    i_dir4.file_size = (unsigned int)data.length();
                    i_dir4.next = true;

                    __create_file(user_info.user_name,user_info.group[0],temp_inode, i_dir4, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt,data);
                }else{
                    if(!check_write_permission(t_inode, user_info, file_name))
                        break;
                    result+=data;
                    write_string_data(user_info.user_name,user_info.group[0],slide_block_queue, big_block_queue, t_inode, slide_group, cur_dir, super_block, gdt, result);
                }
            }
                break;
            case MV:
            {
                if(command_vector.size()<=2){
                    cout<<"usage: mv [-pv] [-m mode] directory ..."<<endl;
                    break;
                }
                if(command_vector[command_vector.size()-1]=="$"){
                    command_vector[command_vector.size()-1]="";
                    for(int i=0;i<ipath.size();i++)
                        command_vector[command_vector.size()-1] += ipath[i].name+"/";
                    if(ipath.size()>1){
                        command_vector[command_vector.size()-1]+="$";
                    }
                }
                else
                    command_vector[command_vector.size()-1] = command_vector[command_vector.size()-1]+"/$";
                Inode to_inode = inode;
                string to_file_name = __temp_cd(to_inode, root_inode, command_vector[command_vector.size()-1], ipath, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                
                if(to_file_name == "")
                {
                    cout<<"-bash: mv: "<<command_vector[command_vector.size()-1]<<": No such file or directory"<<endl;
                    break;
                }//if
                for(int i = 1 ; i < command_vector.size()-1 ; i++ ){
                    Inode from_inode = inode;
                    string from_file_name = __temp_cd(from_inode, root_inode, command_vector[i], ipath, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                    if(from_file_name == "")
                    {
                        cout<<"-bash: mv: "<<command_vector[i]<<": No such file or directory"<<endl;
                        break;
                    }//if
                    move_file(user_info,user_info.user_name,user_info.group[0],from_inode, to_inode, from_file_name, ipath[ipath.size()-1].name, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                }//for

                
            }
                
                break;
            case CD:
            {
                if(command_vector.size()<=1){
                    cout<<"usage: cd [-pv] [-m mode] directory ..."<<endl;
                    break;
                }
                Inode temp_inode = inode;
                vector<I_Path> temp_path;
                temp_path = ipath;
                if(command_vector[1][0] == '/'){
                    inode = root_inode;
                    ipath.clear();
                    I_Path i_path = {root_inode,"/"};
                    ipath.push_back(i_path);
                }
                    vector<string> path;
                    stringstream stream_command(command_vector[1]);
                    string sub_str;
                    while(getline(stream_command,sub_str,'/')){
                        if(sub_str.size()!=0)
                        {
                            path.push_back(sub_str);
                            //cout<<sub_str<<endl;
                        }
                    }
                    bool find = true;
                    for (int i=0; i<path.size()&&find; i++) {
                        if(path[i] == ".."){
                            if(ipath.size()>1)
                                ipath.pop_back();
                            inode = ipath[ipath.size()-1].inode;
                            continue;
                        }
                        find = __open_folder(inode, path[i], slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                        I_Path i_path = {inode,path[i]};
                        ipath.push_back(i_path);
                        //cout<<"size:"<<ipath.size()<<endl;
                    }
                    if(!find)
                    {
                        cout<<"-bash: cd: "<<command_vector[1]<<": No such file or directory"<<endl;
                        inode = temp_inode;
                        ipath = temp_path;
                    }
            
                break;
            }
            case LS:
            {
                string param="none";
                if(param_vector.size()!=0)
                    param = param_vector[0];
                if(!check_read_permission(inode, user_info,ipath[ipath.size()-1].name))
                    break;
                __list_all_files_of_the_folder(param,inode,super_block,gdt);
            }
                break;
            case SHUTDOWN:
                exit(0);
            
            case DELETE:
            {
                if(command_vector.size()<=1){
                    cout<<"usage: rm [-pv] [-m mode] directory ..."<<endl;
                    break;
                }
                for (int i = 1 ; i < command_vector.size() ; i++) {
                    Inode temp_inode = inode;
                    string file_name = __temp_cd(temp_inode, root_inode, command_vector[i], ipath, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                    if(file_name == "")
                    {
                        cout<<"-bash: rm: "<<command_vector[i]<<": No such file or directory"<<endl;
                        continue;
                    }
                    if(!check_write_permission(temp_inode, user_info, ipath[ipath.size()-1].name))
                        break;

                    delete_file(user_info,file_name,temp_inode,inode,slide_block_queue,big_block_queue ,inode_slide_queue,inode_group,slide_group, super_block,gdt);
                }

            }
                break;
                
            case COPY:
            {
                if(command_vector.size()<=2){
                    cout<<"useage: cp source_file target_file "<<endl<<"\t   cp  source_file ... target_directory"<<endl;
                    break;
                }
                if(command_vector[command_vector.size()-1]=="$"){
                    command_vector[command_vector.size()-1]="";
                    for(int i=0;i<ipath.size();i++)
                        command_vector[command_vector.size()-1] += ipath[i].name+"/";
                    if(ipath.size()>1){
                        command_vector[command_vector.size()-1];
                    }
                }
                Inode to_inode = inode;
                string to_file_name = ipath[ipath.size()-1].name;
                    to_file_name = __temp_cd(to_inode, root_inode, command_vector[command_vector.size()-1], ipath, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                    if(to_file_name == "")
                    {
                        cout<<"-bash: cp: "<<command_vector[command_vector.size()-1]<<": No such file or directory"<<endl;
                        break;
                    }//if

                for(int i = 1 ; i < command_vector.size()-1 ; i++ ){
                    Inode from_inode = inode;
                    string from_file_name = __temp_cd(from_inode, root_inode, command_vector[i], ipath, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                    if(from_file_name == "")
                    {
                        cout<<"-bash: cp: "<<command_vector[i]<<": No such file or directory"<<endl;
                        break;
                    }//if
                    copy_file(user_info.user_name,user_info.group[0],from_file_name, to_file_name,to_file_name, from_inode, to_inode, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                    
                }//for
            }
                break;
                
            case RENAME:
            {
                if(command_vector.size()<=2){
                    cout<<"usage: rename old_name new_name."<<endl;
                    break;
                }
                if(!check_file_name(command_vector[2]))
                {
                    cout<<"-bash: rename: illegal file name: "<<command_vector[1]<<endl;
                    break;
                }
                if(!check_write_permission(inode, user_info, ipath[ipath.size()-1].name))
                    break;
                __rename(user_info,inode, command_vector[1], slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt,command_vector[2]);
            }
                break;
                
            case SHOW_SLIDE_INDEX:
            {
                cout<<"blocks index:"<<endl;
                for(int i=0;i<slide_block_queue.size();i++)
                {
                    cout<<"start:"<<slide_block_queue[i].start<<" count:"<<slide_block_queue[i].count<<" start group:"<<slide_block_queue[i].start_groupth<<" end group:"<<slide_block_queue[i].end_groupth<<endl;
                }
            }
                break;
            case SHOW_INODE_INDEX:
            {
                cout<<"inode index:"<<endl;
                for(int i = 0 ; i < inode_slide_queue.size() ; i++)
                {
                    cout<<"start:"<<inode_slide_queue[i].start<<" count:"<<inode_slide_queue[i].count<<" start group:"<<inode_slide_queue.size()<<" end group:"<<inode_slide_queue[i].end_groupth<<endl;
                }
                
            }
                break;
            case PWD:
            {
                cout<<"/";
                for (int i = 1; i < ipath.size(); i++) {
                    cout<<ipath[i].name;
                    if(i!=ipath.size()-1)
                        cout<<"/";
                }
                cout<<endl;
            }
                break;
            case CREATEUSER:
            {
                if(command_vector.size()<=2){
                    cout<<"usage: useradd user_name user_group."<<endl;
                    break;
                }
                bool can = false;
                for(int i=0;i<user_info.group.size();i++)
                {
                    if(user_info.group[i]=="root"||user_info.group[i]=="admin"){
                        can = true;
                        break;
                    }
                }
                if(!can||command_vector[2]=="root"||command_vector[2]=="admin"){
                    cout<<"permission deny."<<endl;
                    break;
                }
                Inode temp = root_inode;
                __create_user(command_vector[1], command_vector[2], temp, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                
                
            }
                break;
                
            case ADMINADD:
            {
                if(command_vector.size()<=1){
                    cout<<"usage: adminadd user_name."<<endl;
                    break;
                }
                bool can = false;
                for(int i=0;i<user_info.group.size();i++)
                {
                    if(user_info.group[i]=="root"){
                        can = true;
                        break;
                    }
                }
                if(!can||command_vector[2]=="root"||command_vector[2]=="admin"){
                    cout<<"permission deny."<<endl;
                    break;
                }
                Inode temp = root_inode;
                __create_user(command_vector[1], command_vector[2], temp, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
            }
                break;
            case ROOTADD:
            {
                if(command_vector.size()<=2){
                    cout<<"usage: rootadd user_name."<<endl;
                    break;
                }
                bool can = false;
                for(int i=0;i<user_info.group.size();i++)
                {
                    if(user_info.group[i]=="root"){
                        can = true;
                        break;
                    }
                }
                if(!can){
                    cout<<"permission deny."<<endl;
                    break;
                }
                Inode temp = root_inode;
                __create_user(command_vector[1], command_vector[2], temp, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
            }
                break;
            case CHANGEPERMISSION:
            {
                if(command_vector.size()<=1){
                    cout<<"usage: chmod file -urwx -grwx -orwx."<<endl;
                    break;
                }
                Inode from_inode = inode;
                string file_name = __temp_cd(from_inode, root_inode, command_vector[1], ipath, slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                if(file_name == "")
                {
                    cout<<"-bash: mv: "<<command_vector[1]<<": No such file or directory"<<endl;
                    break;
                }//if
                Inode t_inode = from_inode;
                Directory cur_dir;
                fstream file(path,ios_base::in|ios_base::out);
                bool find = false;
                for (int i = 0 ;  i < from_inode.i_used_blocks; i++ ) {
                    __addressing(file, i, from_inode, super_block, gdt);
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
                get_inode(cur_dir, t_inode, super_block, gdt);
                if(!check_write_permission(t_inode, user_info, cur_dir.name))
                    break;
                for(int i=0;i<param_vector.size();i++)
                {
                    if(param_vector.size()>3)
                    {
                        cout<<"usage: chmod -urwx -grwx -orwx."<<endl;
                        break;
                    }
                    if(param_vector[i].length()!=5)
                    {
                        cout<<"usage: chmod -urwx -grwx -orwx."<<endl;
                        break;
                    }
                    if(param_vector[i][0]=='-'&&param_vector[i][1]=='u'){
                        t_inode.access_table.user[0]=param_vector[i][2];
                        t_inode.access_table.user[1]=param_vector[i][3];
                        t_inode.access_table.user[2]=param_vector[i][4];
                        
                    }else if(param_vector[i][0]=='-'&&param_vector[i][1]=='g'){
                        t_inode.access_table.group[0]=param_vector[i][2];
                        t_inode.access_table.group[1]=param_vector[i][3];
                        t_inode.access_table.group[2]=param_vector[i][4];
                    }else if(param_vector[i][0]=='-'&&param_vector[i][1]=='o'){
                        t_inode.access_table.other[0]=param_vector[i][2];
                        t_inode.access_table.other[1]=param_vector[i][3];
                        t_inode.access_table.other[2]=param_vector[i][4];
                    }
                }
                write_inode(cur_dir, t_inode, super_block, gdt);
                
            }
                break;
                
            case REMOVEUSER:
            {
                if(user_info.user_name != "root"){
                    cout<<"permission deny."<<endl;
                    break;
                }
                Inode temp_inode = root_inode;
                __open_folder(temp_inode, "usr", slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
                
                delete_file(user_info,command_vector[1],temp_inode,inode,slide_block_queue,big_block_queue ,inode_slide_queue,inode_group,slide_group, super_block,gdt);
            }
                break;
            case FORMAT:
            {
                if(user_info.user_name != "root"){
                    cout<<"permission deny."<<endl;
                    break;
                }
                format_virual_disk();
                exit(0);
            }
                break;
            
            default:
                break;
        }
        
    }
    
}//cin_command

string __temp_cd(Inode &temp_inode,const Inode &root_inode,const string &command,const vector<I_Path> &ipath,vector<Slide_block> &slide_block_queue,vector<BigBlockLink>& big_block_queue ,vector<InodeSlideBlock>&inode_slide_queue,unsigned int &inode_group,unsigned int &slide_group,const SuperBlock& super_block ,const GroupDescriptor& gdt){

    string result="";
    
    vector<I_Path> temp_path;
    temp_path = ipath;
    if(command[0] == '/'){
        temp_inode = root_inode;
        temp_path.clear();
        I_Path i_path = {root_inode,"/"};
        temp_path.push_back(i_path);
    }
    vector<string> path;
    stringstream stream_command(command);
    string sub_str;
    while(getline(stream_command,sub_str,'/')){
        if(sub_str.size()!=0)
        {
            path.push_back(sub_str);
            //cout<<sub_str<<endl;
        }
    }
    bool find = true;

    for (int i=0; i<((int)path.size()-1)&&find; i++) {
        if(path[i] == ".."){
            if(ipath.size()>1)
                temp_path.pop_back();
            temp_inode = temp_path[temp_path.size()-1].inode;
            continue;
        }
        find = __open_folder(temp_inode, path[i], slide_block_queue, big_block_queue, inode_slide_queue, inode_group, slide_group, super_block, gdt);
        I_Path i_path = {temp_inode,path[i]};
        temp_path.push_back(i_path);
    }
    if((int)path.size()==0)
    {
        temp_inode = root_inode;
        return "/";
        
    }else if(!find){
        return result="";
    }else
        return result=path[path.size()-1];
    
}//__temp_cd

bool check_file_name(const string& file_name){
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
}//check_file_name
