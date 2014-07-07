//
//  main.cpp
//  OS_IEXT2
//
//  Created by jimzhai on 13-11-25.
//  Copyright (c) 2013年 com.jimzhai. All rights reserved.
//

#include <iostream>
#include <bitset>
#include <fstream>
#include "Format.h"
#include "Command.h"
#include "Start_up.h"
#include "Binary_operation.h"
#include <iostream>
#include <sstream>
#include "CommandTool.h"
#include <list>
#include <map>
using namespace std;
#include <boost/regex.hpp>
using namespace boost;


int main(int argc, const char * argv[])
{
    
    ifstream File(path);
    if(!File)
    {
        format_virual_disk();
    }
    
    
    execute_commands();

    
    return 0;
}
