#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define LTBLUE "\x1B[38;5;33m"
#define LTGREEN "\x1B[38;5;48m"
#define BOLDRED "\e[1;31m"
#define WHITE   "\x1B[37m"
#define RESETCOLOR "\x1B[0m"

//run: ./detector /ilab/users/wa125/cs214/hw3/  
// ./detector ./

void* readFile(void* ptr){
puts("here");
printf("thread is here, pathname: %s\n", *(char**)ptr);

return ptr;
}




char* appendString(char* string1, char* string2){
int space_needed = strlen(string1) + strlen(string2) + 1;

int string1_space = strlen(string1);
int string2_space = strlen(string2);
//printf("here, space needed: %i\n", space_needed);
char* newstring = malloc(space_needed);


for(int i=0; i<string1_space; i++){
newstring[i]=string1[i];
//printf("i loop, newstring[i] %c\n", newstring[i]);
}

for(int j=0; j<string2_space; j++){
newstring[string1_space+j]=string2[j];
//printf("j loop, newstring[j] %c\n", newstring[string1_space+j]);
}
newstring[space_needed-1]='\0';
//printf("end of function\n");

return newstring;
}



void printDirectoryContents(char* directory_path){

DIR *dirptr = opendir(directory_path);

if(dirptr==NULL){
printf("Could not open directory");
return;
}
//pointer for each entry in directory
struct dirent *direntptr;

//iterate through each entry in directory
while ((direntptr = readdir(dirptr))){

//If the entry is a sub directory, print name in blue
if(direntptr->d_type==DT_DIR){
printf("Directory Name: " LTBLUE "./%s\n"  RESETCOLOR, direntptr->d_name);
continue;
}
//check if the file is executeable, if so print in red
if(access(direntptr->d_name, X_OK)==0){
printf("Filename: " BOLDRED "%s" RESETCOLOR, direntptr->d_name);
}
 else if (direntptr->d_type==DT_REG) {
//if regular file, print filename in light green
 printf("Filename: " LTGREEN "%s" RESETCOLOR, direntptr->d_name);
 }
   else{ //else print white
   printf("Filename: " WHITE "%s" RESETCOLOR, direntptr->d_name);
   } 

//will probably need to append full string here
char* pathname = appendString(directory_path, direntptr->d_name);
printf("\tpathname: %s\t", pathname);
int fd = open(pathname, O_RDONLY);

pthread_t thread2;
pthread_create(&thread2, NULL, readFile, &pathname);
pthread_join(thread2, NULL);
free(pathname);
//int fd = open(direntptr->d_name, O_RDONLY);

   //take care of case where open returns -1
   if(fd<0){
   perror("Line 83: Could not open file");
   continue;
   }


   off_t bytesread = lseek(fd,0, SEEK_END);

   //Take care of case where lseek returns -1
   if(bytesread==-1){
   printf("Could not read file\n");
   }
   else{
   printf("  size: %li\n", bytesread);
   }
   close(fd);
   }

closedir(dirptr);	

//free(pathname);
}


int main(int argc, char** argv){
printf("argc: %d\n", argc);
printf("argv: %s\n", *argv);

if(argc!=2){
return 1;
}

printDirectoryContents(argv[1]);
/*
int *ptr = malloc(sizeof(int)*5);
ptr[0]=100;

printDirectoryContents(argv[1]);
pthread_t thread1;
pthread_create(&thread1, NULL, readFile, &ptr[0]);



pthread_join(thread1, NULL);

free(ptr);	
*/
return 0;
}
