#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>
#define MAX_LENGTH 512


int main(int argc, char *argv[]) {

DIR *d;
struct dirent *dir;
struct stat buf;
char name[MAX_LENGTH];
pid_t pid;
int status;
FILE *newfile;

  //Erros
  if (argc!=2){ 
  fprintf(stderr, "You need to put the directory name next to %s \n", argv[0]);
  exit(1);}

  if ((d=opendir(argv[1]))==NULL){
   fprintf(stderr,"Erros: Directory %s not found\n", argv[1]);
   exit(1);}
  //Creation of the file if it doesnt exist

   newfile = fopen("info.txt", "a"); 

  //Ciclo para leitura do directorio

   while ((dir=readdir(d))!=NULL){
    if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue; //"Ignorar" . e ..
    sprintf(name, "%s/%s", argv[1], dir->d_name);
          if (lstat(name, &buf) == -1) {
          perror("lstat error");
          int erro=errno;
          if (erro==EACCES) //if one of the directories composing the path is not readable
              fprintf(stderr, "in %s\n\n", name);
      }
    if (S_ISREG(buf.st_mode)){ 
      if (strcmp(dir->d_name,"files.txt") != 0){
       fprintf(newfile,"%-8ld ",(unsigned long) buf.st_ino);
       fprintf(newfile,(buf.st_mode & S_IRUSR) ? "r" : "-");
       fprintf(newfile,(buf.st_mode & S_IWUSR) ? "w" : "-");
       fprintf(newfile,(buf.st_mode & S_IXUSR) ? "x" : "-");
       fprintf(newfile,(buf.st_mode & S_IRGRP) ? "r" : "-");
       fprintf(newfile,(buf.st_mode & S_IWGRP) ? "w" : "-");
       fprintf(newfile,(buf.st_mode & S_IXGRP) ? "x" : "-");
       fprintf(newfile,(buf.st_mode & S_IROTH) ? "r" : "-");
       fprintf(newfile,(buf.st_mode & S_IWOTH) ? "w" : "-");
       fprintf(newfile,(buf.st_mode & S_IXOTH) ? "x" : "-");
       char *date= ctime(&buf.st_mtime);
       date[24] = 0;
       fprintf(newfile, " %s", date);
       fprintf(newfile, " %-10lld", (unsigned long long)(buf.st_mtime));
       fprintf(newfile, " %-9lld", (unsigned long long) buf.st_size);
          //pwd
       char curdir[4096];
       if (getcwd(curdir, sizeof(curdir))!=NULL)
        fprintf(newfile, " %s/%s %s\n", curdir, argv[1], dir->d_name);
    }
  }  
      //New lsdir if a directory is found
  else if (S_ISDIR(buf.st_mode)){
    if ((pid=fork())==-1){
      perror("Lsdir fork error \n");
      exit(1);
    }
    if (pid==0)
      execlp("./lsdir", "./lsdir", name, NULL);
    else wait(&status);
  }
}
fclose(newfile);
closedir(d);
exit(0);
}