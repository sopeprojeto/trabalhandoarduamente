#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <fcntl.h>
#define BUF_LENGTH  256


struct Filethings {
    int number;
    char *permissions;
    time_t mtime;
    int inode;
    int size;
    char *path;
    char *name;
};



int calling(char *directory) {
    pid_t pid;
    int status;
    FILE *file = fopen("files.txt", "w");
    if (file == NULL) {
        perror("Error opening files.txt.\n");
        return -1;
    }
    fclose(file);
    if ((pid = fork()) == -1) {
        perror("Error on fork \n");
        return -1;
    }
    if (pid == 0)
        execlp("./lsdir", "./lsdir", directory, NULL);
    else wait(&status);
    if ((pid = fork()) == -1) {
        perror("Error on fork \n");
        return -1;
    }
    if (pid == 0)
        execlp("sort", "sort", "files.txt", "-k", "11,11", "-k", "8,8", "-o", "files_sorted.txt", NULL);
    else wait(&status);
    return 0;
}



void getFileInfo(struct Filethings *f) {
    char str[888];

    int i=0;
    int count = 0;
    FILE *file = fopen("files_sorted.txt", "r");
    if (file) {
        while (fscanf(file, "%s", str)!=EOF){

          printf("%s\n",str);  
          if(count ==0)
            f[i].inode = atoi(str);

        if(count ==1)
            f[i].permissions=str;
        if(count ==8)
            f[i].size=atoi(str);
        if(count==9)
            f[i].path=str;
        if(count==10)
            f[i].name=str;
        count ++;
        if(count>10){
            count=0;
            f[i].number=i;
            i++;
        }
    }
    fclose(file);
}

}

int main(int argc, char *argv[]){
 struct Filethings f[BUF_LENGTH];
 calling(argv[1]);
 getFileInfo(f);
}
