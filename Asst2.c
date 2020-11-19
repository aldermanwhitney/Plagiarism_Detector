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

/**Linked List to keep track of threads in use
 * Used for properly joining all threads
 */
struct ThreadNode{
pthread_t threadID;
char *param;
struct ThreadNode *next;
};
struct ThreadNode* head = NULL;

/*Creates a new thread and stores it in a newly malloced ThreadNode
 *Also adds it to a linked list for later freeing
 */
struct ThreadNode* createThreadandStoreinLinkedList(void *(*start_routine) (void *), void *arg){ 
struct ThreadNode *threadnode = malloc(sizeof(struct ThreadNode));
puts("d");

int arg_size = strlen((*(char**)arg));
if(arg_size){

}


//int arg_size = 40;
printf("arg size: %d", arg_size);

char *argument = malloc(sizeof(char)*arg_size+1);
if(argument){}


memcpy(argument, &arg, arg_size+1);

puts("HERE");
threadnode->param = argument;

pthread_create(&(threadnode->threadID), NULL, start_routine, argument);

if(head!=NULL){
threadnode->next = head;
head = threadnode;
}
else{
threadnode->next = NULL;	
head = threadnode;
}
puts("thread added");
//threadnode->param = argument;
return threadnode;
}

void freeAndJoinLinkedList(struct ThreadNode* head){
puts("in free and join linked list");
if (head==NULL){
return;
}


struct ThreadNode *prev = NULL;
struct ThreadNode *current = head;

while (current!=NULL){
puts("before join");
int error = pthread_join(current->threadID, NULL);	
if (error){
puts("COULD NOT JOIN THREADS");
}
puts("before pointer change");
prev = current;
current=current->next;
puts("before free");
free(prev->param);
free(prev);
}
	
}


/*
void* directoryHandler(void* ptr){
puts("directory handler");
printf("thread is here, pathname: %s\n", *(char**)ptr);

if (ptr==NULL){
return NULL;
}

char* directory_path = *(char**)ptr;

DIR *dirptr = opendir(directory_path);

if(dirptr==NULL){
printf("Could not open directory");
return NULL;
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
else if (direntptr->d_type==DT_REG) {
//if regular file, print filename in light green
 printf("Filename: " LTGREEN "%s" RESETCOLOR, direntptr->d_name);
}
else{ //Not DT_REG or DT_DIR, skip over
   continue;
} 

//will probably need to append full string here
char* pathname = appendString(directory_path, direntptr->d_name);
printf("\tpathname: %s\t", pathname);
int fd = open(pathname, O_RDONLY);

pthread_t thread2;
pthread_create(&thread2, NULL, fileHandler, &pathname);
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


return ptr;
}
*/
void* fileHandler(void* ptr){
puts("file handler");
printf("thread is here, pathname: %s\n", *(char**)ptr);

return ptr;
}



/**Function which takes two pointers to char arrays
 * And returneds a pointer to a newly allocated char array
 * which contains the second string appended to the first
 */
char* appendString(char* string1, char* string2){
int space_needed = strlen(string1) + strlen(string2) + 1;

int string1_space = strlen(string1);
int string2_space = strlen(string2);
printf("here, space needed: %i\n", space_needed);
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

//char **ptr = malloc(sizeof(int)*10);
//int i = 0;

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

head = createThreadandStoreinLinkedList(fileHandler, &pathname);


//ptr[i] = pathname;


//head = createThreadandStoreinLinkedList(fileHandler, &ptr[i]);
//head = createThreadandStoreinLinkedList(fileHandler, (appendString(directory_path, direntptr->d_name)));

if(head==NULL){

}

//pthread_t thread2;
//pthread_create(&thread2, NULL, fileHandler, &pathname);
//pthread_join(thread2, NULL);
//free(pathname);
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
   free(pathname);
   }

closedir(dirptr);	

}






int main(int argc, char** argv){
printf("argc: %d\n", argc);
printf("argv: %s\n", *argv);

if(argc!=2){
return 1;
}

printDirectoryContents(argv[1]);
freeAndJoinLinkedList(head);
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
