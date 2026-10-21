#define _CRT_SECURE_NO_WARNINGS  1
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
void caser(char * p,size_t len )
{
    int i = 0;
    for (i = 0; i < len; i++)
    {
        if (p[i] <= 'z' && p[i] >= 'a')
        {
            p[i] = (char)('a' + (p[i] - 'a' + 3) % 26);             /*关键点：
                                                                      若是小写字母，那就直接计算它与‘a’的差值，得到差值在 % 26，得到代换后理论上对应的数字（字母）；
                                                                      但此时我还要再把它转换成对应的ascii码字符，所以再加上 'a',之后就可以正确打印对应字符了。   
                                                                        */
        }
        else if (p[i] <= 'Z' && p[i] >= 'A')                       
        {
                                                                      //关键点： 若是大写字母，那就直接计算它与‘A’的差值，得到差值在 % 26，得到代换后理论的对应的数字（字母）；
             p[i] = (char)('A' + (p[i] - 'A' + 3) % 26);                //但此时我还要再把它转换成对应的ascii码字符，所以再加上 'A',之后就可以正确打印对应字符了。           
                                                                               
        }
    }

}
int main() {
    char a[100] = {0};
    fgets(a,100,stdin);
    int len = strlen(a);
    if (a[len - 1] == '\n')
    {
        a[len - 1] == '\0';
        len--;
    }
    caser(a,len);
    printf("%s", a);

    return 0;
}