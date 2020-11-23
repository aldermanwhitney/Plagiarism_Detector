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
//./detector /ilab/users/wa125/cs214/asst2testcases/  

//static pthread_mutex_t threadLLmut = PTHREAD_MUTEX_INITIALIZER;

/**Struct to hold arguments needed by threads
 */
struct ThreadArgs{
char *filepath;
};

int threadsadded = 0;
int threadsjoined = 0;

/**Function takes values for ThreadArgs fields
 *and creates a newly malloc'ed ThreadArg struct
 returns pointer to it
 */
struct ThreadArgs* createThreadArgsStruct(char *filepath){

//copy file path string
int arg_size = strlen(filepath);
char *argument = malloc(sizeof(char)*arg_size+1);
strncpy(argument, filepath, arg_size);
argument[arg_size] = '\0';

//printf("argument copied in create thread arg struct: %s\n", argument);

//Malloc new thread arg struct and set fields
struct ThreadArgs *threadargs = malloc(sizeof(struct ThreadArgs));
threadargs->filepath = argument;
//printf("THREAD ARGS FP:%s", threadargs->filepath);
return threadargs;
}


/**Linked List to keep track of threads in use
 * Used for properly joining all threads
 */
struct ThreadNode{
pthread_t threadID;
struct ThreadArgs *args;
struct ThreadNode *next;
};
struct ThreadNode* head = NULL;



/**Function takes a pointer to a thread arg struct as a void pointer
 *handles file operations, and returns the same pointer
 */
void* fileHandler(void* ptr){

struct ThreadArgs *threadargs = ((struct ThreadArgs*)ptr);

printf("thread in fileHandler, pathname3: %s\n", threadargs->filepath);

char* pathname = threadargs->filepath;
int fd = open(pathname, O_RDONLY);

   //take care of case where open returns -1
   if(fd<0){
   perror("Line 83: Could not open file\n");
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
  

return ptr;
}

/*Function takes a void/void function and a pointer to a ThreadArg struct
 * Creates a new thread and stores it in a newly malloced ThreadNode
 *Also adds it to a linked list for later freeing
 */
struct ThreadNode* createThreadandStoreinLinkedList(void *(*start_routine) (void *), void *arg){ 

struct ThreadNode *threadnode = malloc(sizeof(struct ThreadNode));

struct ThreadArgs *threadargs = ((struct ThreadArgs*)arg);
printf("thread args file path: %s\n", (threadargs->filepath));
threadnode->args = threadargs;

pthread_create(&(threadnode->threadID), NULL, start_routine, threadargs);

//Add ThreadNode holding ThreadID to end of linked list
int pos = 0;
if(head==NULL){
threadnode->next = NULL;
head = threadnode;
}
else{
struct ThreadNode *curr = head;
struct ThreadNode *prev = NULL;

while(curr!=NULL){
pos++;
prev = curr;
curr = curr->next;
}
prev->next = threadnode;
threadnode->next = NULL;
}


printf("(thread added), position: %d\n", pos);
threadsadded++;
return head;
}


/**Function iterates through linked list of ThreadNodes 
 *and joins each thread, starting at the head
 */
void freeAndJoinLinkedList(struct ThreadNode* head){
puts("in free and join linked list");

if (head==NULL){
return;
}

struct ThreadNode *prev = NULL;
struct ThreadNode *current = head;

if(prev){}

while (current!=NULL){
//puts("before join");

printf("Thread ID: %ld", current->threadID);
int error = pthread_join(current->threadID, NULL);	
if (error){
puts("COULD NOT JOIN THREADS");
}
printf("(thread joined)\n");
threadsjoined++;
//puts("before pointer change");
prev = current;

current=current->next;
//head = current;
//puts("before free");
/////free(prev->param);
/*
struct ThreadArgs *threadargs = prev->args;
free(threadargs->filepath);
free(prev->args);
free(prev);
*/
}
	
}


/**Function which takes two pointers to char arrays
 * And returneds a pointer to a newly allocated char array
 * which contains the second string appended to the first
 */
char* appendString(char* string1, char* string2){
int space_needed = strlen(string1) + strlen(string2) + 1;

int string1_space = strlen(string1);
int string2_space = strlen(string2);
//printf("append string, space needed: %i, str1: %s str2: %s\n", space_needed, string1, string2);
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
//printf("end of string cat function\n");

return newstring;
}


/**Thread Function
 *Takes a pointer to a ThreadArgs struct
 *Recursively iterates though directory and subdirectories
 *If a new sub directory is found, a new thread is created to handle it in DirectoryHandler
 *If a regular file is found, a new thread is created to handle it in FileHandler
 *Returns the argument pointer
 */
void* directoryHandler(void* ptr){

//cast argument as pointer to thread args
struct ThreadArgs *threadargs = (struct ThreadArgs*)ptr;

printf("thread in dirHandler, pathname: %s\n", threadargs->filepath);

//extract necc. info
char* dirpath = threadargs->filepath;

DIR *dirptr = opendir(dirpath);
if(dirptr==NULL){
printf("Could not open directory: %s\t", *(char**)ptr);
perror("error: ");
return ptr;
}

//pointer for each entry in directory
struct dirent *direntptr;

//iterate through each entry in directory
while ((direntptr = readdir(dirptr))){

//If the entry is a sub directory, spin off a new thread to recursively inspect
if(direntptr->d_type==DT_DIR){
//printf("FOUND SUB DIRECTORY");
//printf("Directory Name: " LTBLUE "./%s\n"  RESETCOLOR, direntptr->d_name);

	
//skip over any references to current or parent directory
if (strcmp(direntptr->d_name, ".") == 0 || strcmp(direntptr->d_name, "..") == 0){
continue;
}

//concatenate subdirectory name and slash to current directory
char *pathname = appendString(dirpath, direntptr->d_name);
//char slash[] = "/";
char *slash = malloc(sizeof(char)*2);
slash[0]='/';
slash[1]='\0';
char *pathnameSlash = appendString(pathname, slash);
printf("DIRECTORY PATHNAME: %s\n", pathnameSlash);

//create new ThreadArgsStruct out of subdirectory pathname
struct ThreadArgs *threadargs = createThreadArgsStruct(pathnameSlash);

printf("ANOTHER: %s", threadargs->filepath);

//create a Thread to handle it, and store it in a linked list
createThreadandStoreinLinkedList(directoryHandler, threadargs);

free(slash);
//free(threadargs);
free(pathname);
free(pathnameSlash);
continue;
}

//Found a regular file, spin off new thread with this file in filehandler function
if (direntptr->d_type==DT_REG) {
// printf("Filename: " LTGREEN "%s\n" RESETCOLOR, direntptr->d_name);

char* pathname = appendString(dirpath, direntptr->d_name);
//printf("\tpathname: %s\t\n", pathname);


struct ThreadArgs *threadargs = createThreadArgsStruct(pathname);
createThreadandStoreinLinkedList(fileHandler, threadargs);

free(pathname);

}
}

closedir(dirptr);	

return ptr;
}

/**Prints Directory
 *Used for debugging
 */
void printDirectoryContents(char* directory_path){

 printf("\n\t*****BEGINNING OF PRINT DIRECTORY FUNCTION*****\n\n");

DIR *dirptr = opendir(directory_path);
	
if(dirptr==NULL){
printf("Could not open directory\n");
return;
}

//pointer for each entry in directory
struct dirent *direntptr;

//iterate through each entry in directory
while ((direntptr = readdir(dirptr))){
	
	//If the entry is a sub directory, print name in blu
	
	
if(direntptr->d_type==DT_DIR){
printf("Directory Name: " LTBLUE "./%s\n"  RESETCOLOR, direntptr->d_name);

if (strcmp(direntptr->d_name, ".") == 0 || strcmp(direntptr->d_name, "..") == 0){
continue;
}



char* pathname = appendString(directory_path, direntptr->d_name);
char slash[] = "/";
char *pathnameSlash = appendString(pathname, slash);
//printf("DIRECTORY PATHNAME: %s\n", pathnameSlash);

//directoryHandler(&pathnameSlash);
//printf("\t");
printDirectoryContents(pathnameSlash);


free(pathname);
free(pathnameSlash);
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
	         continue;    
	      	}

	      //will probably need to append full string here
	
         char* pathname = appendString(directory_path, direntptr->d_name);
         printf("\tpathname: %s\t", pathname);
         int fd = open(pathname, O_RDONLY);
	
 	 //take care of case where open returns -1
          if(fd<0){
          perror("Line 83: Could not open file");
          free(pathname);
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
  printf("\n\t*****END OF PRINT DIRECTORY FUNCTION*****\n\n");
  }


int main(int argc, char** argv){
printf("argc: %d\n", argc);
printf("argv: %s\n", *argv);

if(argc!=2){
return 1;
}

printDirectoryContents(argv[1]);

int arg_size = strlen(argv[1]);
char *argument = malloc(sizeof(char)*arg_size+1);
strncpy(argument, argv[1], arg_size);
argument[arg_size] = '\0';

printf("argument param: %s\n", argument);


//directoryHandler(&argument);


//struct ThreadArgs *threadargs = createThreadArgsStruct(argument);

//create first thread arg struct to send initial directory argument in
struct ThreadArgs *threadargs = malloc(sizeof(struct ThreadArgs));
threadargs->filepath = argument;
//threadargs->filepath = argv[1]; this works too

directoryHandler(threadargs);


//directoryHandler(&argv[1]);


freeAndJoinLinkedList(head);
free(argument);
free(threadargs);
puts("END OF PROGRAM");

printf("threads added: %d, threads joined: %d", threadsadded, threadsjoined);
puts("\nhere");
return 0;
}
