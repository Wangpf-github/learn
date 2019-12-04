#include <stdio.h>
#include <string.h>
char *strRev(char *s)
{
	char *p = s;
    char temp, *end = s + strlen(s) - 1;
    while( end > s)
    {
        temp = *s;
        *s = *end;
        *end = temp;
        --end;
        ++s;
    }
    return p;
}
int main()
{
	char buf[20]="hello world";
	char *rev;
	rev = strRev(buf);
	printf("%s\n",rev);
}
