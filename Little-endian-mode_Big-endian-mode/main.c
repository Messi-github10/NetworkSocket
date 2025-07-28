#include <stdio.h>

/*
联合体的所有成员共享同一块内存空间，且内存起始地址相同。
num 和 bytes 共享同一块内存。
给 num 赋值后，可以通过 bytes 直接访问该整数的每一个字节。

大端模式：
地址增长方向 → 
+----+----+----+----+
| 00 | 00 | 00 | 01 |  （bytes[0]是最高位0x00）
+----+----+----+----+

小端模式：
地址增长方向 → 
+----+----+----+----+
| 01 | 00 | 00 | 00 |  （bytes[0]是最低位0x01）
+----+----+----+----+

*/

int is_little_endian(){
    union{
        int num;
        char bytes[sizeof(int)];
    } test;
    test.num = 1;
    return test.bytes[0];
}

int main(){
    if(is_little_endian()){
        printf("当前机器是小端存储.\n");
    }else{
        printf("当前机器是大端存储.\n");
    }
    return 0;
}