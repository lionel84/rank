/*
 * @Author: lionel
 * @Date: 2020-07-30 11:14:57
 * @LastEditors: lionelzhang
 * @LastEditTime: 2020-11-06 21:42:30
 * @Description: 
 */ 

#include <iostream>
#include <vector>
#include <sstream>
#include "base.h"
using namespace std;
#include  <sys/time.h>
void printnow(string desc){
    static unsigned long long time=0;
    timeval tm;
    gettimeofday(&tm,NULL);
    if(time !=0 && desc != "")
        printf("%s cost time = %llu us\n",desc.c_str(), (tm.tv_sec*1000000+tm.tv_usec - time));
    time = tm.tv_sec*1000000+tm.tv_usec;
}
