//
//  Slider_block.h
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-26.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#ifndef OS_IEXT2_Slider_block_h
#define OS_IEXT2_Slider_block_h

struct Slide_block{
    unsigned short int start;
    unsigned short int count;
    unsigned short int start_groupth;
    unsigned short int end_groupth;
};
typedef Slide_block BigBlockLink;
typedef Slide_block InodeSlideBlock;

#endif
