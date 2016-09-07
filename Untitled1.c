#include <stdio.h>
#include <string.h>

int main(void) {
    char tmpa[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    tmpa[10] = '\0';
    tmpa[11] = 'b';
    char tmpb[1000] = "abab";
    char *tmpc = tmpb;
    //tmpc += 4;
    strncpy(tmpb + 4, tmpa, 15);
    //tmpb
    fprintf(stdout, "%s\n", tmpb);
    int tmpj;
    for (tmpj = 0; tmpj < 20; tmpj++) fprintf(stdout, "%d|", tmpb[tmpj]);
}
