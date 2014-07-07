//
//  Binary_operation.cpp
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-27.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#include "Binary_operation.h"

void BinaryBlock:: write_block(fstream& file){
    
    for (int i=0; i<block_size/sizeof(unsigned int); i++) {
        unsigned int temp = (unsigned int)block[i].to_ulong();
        file.write((char *)(&temp), sizeof(unsigned int));
    }
    
}//write_block

void BinaryBlock:: write_a_piece_of_block(fstream& file,unsigned int flag){
    unsigned int temp = (unsigned int)block[flag/(sizeof(unsigned int)*8)].to_ulong();
    file.seekg(((flag)/(sizeof(unsigned int)*8))*sizeof(unsigned int),ios_base::cur);
    file.write((char *)(&temp), sizeof(unsigned int));
    //cout<<"write_a_piece_of_block:"<<block[flag/(sizeof(unsigned int)*8)]<<endl;
    file.seekg(-((flag)/(sizeof(unsigned int)*8))*sizeof(unsigned int)-sizeof(unsigned int),ios_base::cur);
}//write_a_piece_of_block

void BinaryBlock:: read_a_piece_of_block(fstream& file,unsigned int flag){
    unsigned int temp = 0;

    file.seekg(((flag)/(sizeof(unsigned int)*8))*sizeof(unsigned int),ios_base::cur);
    //cout<<"(flag)/(sizeof(unsigned int)*8 "<<(flag)/(sizeof(unsigned int)*8)<<endl<<"file.tellg():"<<file.tellg()<<endl;
    file.read((char *)(&temp), sizeof(unsigned int));
    block[flag/(sizeof(unsigned int)*8)] = temp;
    file.seekg(-(((flag)/(sizeof(unsigned int)*8))*sizeof(unsigned int)+sizeof(unsigned int)),ios_base::cur);
    
}//read_a_piece_of_block

void BinaryBlock:: set(unsigned int flag){
        block[flag/(sizeof(unsigned int)*8)].set(flag%(sizeof(unsigned int)*8));
    //cout<<block[flag/(sizeof(unsigned int)*8)].set(flag%(sizeof(unsigned int)*8))<<endl;
}//set

void BinaryBlock:: reset(unsigned int flag){
    block[flag/(sizeof(unsigned int)*8)].reset(flag%(sizeof(unsigned int)*8));
}//reset

int BinaryBlock:: test(unsigned int flag){
    return block[flag/(sizeof(unsigned int)*8)].test(flag%(sizeof(unsigned int)*8));
}//test

unsigned int BinaryBlock:: get_value_from_a_piece_of_block(unsigned int flag){
    return (unsigned int)block[flag/(sizeof(unsigned int)*8)].to_ulong();
}//get_value_from_a_piece_of_block

void BinaryBlock:: read_block(fstream& file){
    unsigned int temp = 0;
    for (int i=0; i<block_size/sizeof(unsigned int); i++) {
        file.read((char *)(&temp), sizeof(unsigned int));
        block[i] = temp;
    }
}//read_block

void BinaryBlock:: print_block(){

    for (int i= 0 ; i < block_size/sizeof(unsigned int); i++) {
        for (int j = 0; j < sizeof(unsigned int)*8; j++) {
            cout<<block[i].test(j);
        }
    }
    
}//BianryBlock

void BinaryBlock:: print_a_piece_of_block(unsigned int flag){

    for (int j = 0; j < sizeof(unsigned int)*8; j++) {
        cout<<block[flag/(sizeof(unsigned int)*8)].test(j);
    }

}//print_a_piece_of_block

void BinaryBlock:: clear()
{
    for(int i = 0 ; i < block_size/sizeof(unsigned int); i++)
        block[i].reset();
}//clear
