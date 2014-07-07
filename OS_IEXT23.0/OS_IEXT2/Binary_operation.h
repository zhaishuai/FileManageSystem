//
//  Binary_operation.h
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-27.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_Binary_operation_h
#define OS_IEXT2_Binary_operation_h

#include <fstream>
#include <bitset>
#include <iostream>
using namespace std;


class BinaryBlock{
private:
    unsigned short int block_size;
    bitset<sizeof(unsigned int)*8> *block ;
    
public:
    BinaryBlock(unsigned short int block_len){
        block_size = block_len; //Byte
        block = new bitset<sizeof(unsigned int)*8>[block_size/sizeof(unsigned int)];
    }
    ~BinaryBlock(){
        delete[] block;
    }
    void write_block(fstream& file);//file.seekg is the block start position will change file.seekg
    void write_a_piece_of_block(fstream &file,unsigned int flag); //file.seekg is the block start position will not change file.seekg
    void read_a_piece_of_block(fstream &file,unsigned int flag);  //file.seekg is the block start position will not change file.seekg
    void set(unsigned int flag);//set 1
    void reset(unsigned int flag);//set 0
    int test(unsigned int flag);
    unsigned int get_value_from_a_piece_of_block(unsigned int flag);
    void read_block(fstream& file);//file.seekg is the block start position will change file.seekg
    void print_block(); //debug BinaryBlock
    void print_a_piece_of_block(unsigned int flag);  //debug
    void clear();//set all bit to 0
    
    
    
};


#endif
