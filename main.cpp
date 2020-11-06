/*
 * @Author: lionel
 * @Date: 2020-07-22 17:33:50
 * @LastEditors: lionelzhang
 * @LastEditTime: 2020-10-30 21:05:07
 * @Description: main
 */ 

#include "base.h"
#include "rank_test.h"
int main(int argc, char** argv) {
    int ret = 0;
    //UNUSED(argc);
    //UNUSED(argv);
    //UNUSED(ret);

    int test_count = 1000000;
    if(argc > 1){
        test_count = atoi(argv[1]);
    }
    nm_rank_test::test(test_count);
    return 0;
}


