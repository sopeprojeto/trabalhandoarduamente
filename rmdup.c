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
#include <time.h>
#define BUF_LENGTH  256


typedef int bool;
#define true 1
#define false 0

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

int compare_time_last_modification(char* path_1, char* path_2){

    path_1 = strtok(path_1, " ");
    path_2 = strtok(path_2, " ");

    struct stat buf;
    time_t path_1time,path_2time;
    double seconds;

    lstat(path_1,&buf);
    path_1time=buf.st_mtime;

    lstat(path_2,&buf);
    path_2time=buf.st_mtime;
    seconds=difftime(path_1time,path_2time);

//if the diff between the two dates are bigger than 0 it means that file1 date is bigger->1_file is more recent
    if(seconds>0){
//  File 1 is more recent than file 2 
        return 1;

    }

//if the diff between the two dates are lesser than 0 it means that file1 date is lesser-> 1_file is older
    else if(seconds<0){
// File 1 is more older than file 2
        return 2;
    }

    else{
//Same date
        return 0;
    }
    return 0;

}

int compare_file_content(char *path_1, char *path_2){

//reseting blank spaces on the strings
    path_1 = strtok(path_1, " ");
    path_2 = strtok(path_2, " ");

//opening the files, with the respective path
    FILE* file_1= fopen(path_1, "r");
    FILE* file_2= fopen(path_2, "r");

    bool eof=false;
    int ch_file_1;
    int ch_file_2;

    if(file_1 == NULL || file_2 == NULL){
        perror("Error on opening files to compare content" );
        exit(1);
    }

    while(!eof){

//getting the respective characters to compare
        ch_file_1=getc(file_1);
        ch_file_2=getc(file_2);

//if they are different the files aren
        if(ch_file_1 != ch_file_2){
          return 1;
      }

//if the files over
      if(ch_file_1 == EOF || ch_file_2 == EOF){
          eof=true;
      }
  }
  return 0;
}

int createHardLink(char *path1,char *path2){
    
    int link_anterior = unlink(path2);
    if (link_anterior) {
        perror("Error unlinking file.\n");
        return link_anterior;
    }
 int link_final = link(path1,path2);
    if (link_final) {
        perror("Error linking files.\n");
        return link_final;
    }
return link_final;
}

void fileHardLink(int i,int j,char *arg,struct Filethings *f){
    DIR *d;
    FILE *novofile;
  //Criar ficheito txt se nao existir "a"
    d=opendir(arg);
   novofile = fopen("hlinks.txt","a");
   fprintf(novofile,"Criado Hardlink do ficheiro numero %d para o numero %d com o inode %d\n",i,j,f[j].inode);
    fclose(novofile);
    closedir(d);

}
void compareFile(struct Filethings *f, int n, char *nomedaraiz)
{   
    int aux;
    int i=0;
    int j=0;
    int a=0;
    int b=0;

    for(i=0;i<=n;i++){
        for(j=1;j<=n;j++){
            if(strcmp(f[i].name,f[j].name)==0){
                printf("o nome ficheiro %d é igual ao %d",f[i].number, f[j].number);
                a=1;}
                if(strcmp(f[i].permissions,f[j].permissions)==0){
                    printf("permissoes %d iguais ao %d",f[i].number, f[j].number);
                    b=1;}
                    if(compare_file_content(f[i].path,f[j].path)==0 && a==1 && b==1){
                        aux=compare_time_last_modification(f[i].path,f[j].path);
                        if(aux==2){
            //Criar hardlink em f[j] para f[i]
                            fileHardLink(i,j,nomedaraiz,f);
                        }
                        else{
            //Criar hardlink em f[i] para f[j]}
                            fileHardLink(i,j,nomedaraiz,f);
                        }
                    }
                }
 }
}

int getFileInfo(struct Filethings *f) {
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
            if(count==10){
                f[i].name=str;
                }
                    count ++;
                    if(count>10){
                        count=0;
                        f[i].number=i;
                        i++;
                    }
                }
                fclose(file);
            //Atenção! Não passa nada para a struct daqui para fora enquanto la dentro está ok o resto do programa é continuado como se estivesse a passar mas está comentado
            }
            return i;
        }

int main(int argc, char *argv[]){
    int n;
    struct Filethings f[BUF_LENGTH];
    calling(argv[1]);
    n=getFileInfo(f);
    //compareFile(f,n,argv[1]);
        }
