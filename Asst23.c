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

int arg_size = strlen(filepath);

printf("arg size create thread arg struct: %d", arg_size);

char *argument = malloc(sizeof(char)*arg_size+1);

strncpy(argument, filepath, arg_size);
argument[arg_size] = '\0';

printf("argument copied in create thread arg struct: %s\n", argument);
//athreadnode->param = argument;


struct ThreadArgs *threadargs = malloc(sizeof(struct ThreadArgs));
threadargs->filepath = argument;
printf("THREAD ARGS FP:%s", threadargs->filepath);
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

void* fileHandler(void* ptr){

struct ThreadArgs *threadargs = ((struct ThreadArgs*)ptr);
//struct ThreadArgs *threadargs = *(struct ThreadArgs**)ptr;	
printf("thread in fileHandler, pathname3: %s\n", threadargs->filepath);
//printf("thread in fileHandler, pathname: %s\n", *(char**)ptr);
//printf("thread in fileHandler, pathname2: %s\n", (*(struct ThreadArgs**)ptr)->filepath);

//char* pathname = *(char**)ptr;
char* pathname = threadargs->filepath;
int fd = open(pathname, O_RDONLY);

   //take care of case where open returns -1
   if(fd<0){
   perror("Line 40: Could not open file\n");
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

/*Creates a new thread and stores it in a newly malloced ThreadNode
 *Also adds it to a linked list for later freeing
 */
struct ThreadNode* createThreadandStoreinLinkedList(void *(*start_routine) (void *), void *arg){ 
struct ThreadNode *threadnode = malloc(sizeof(struct ThreadNode));
/*
printf("arguemnt: %s\n", (*(char**)arg));

int arg_size = strlen((*(char**)arg));

//printf("arg size: %d", arg_size);

char *argument = malloc(sizeof(char)*arg_size+1);

strncpy(argument, (*(char**)arg), arg_size);
argument[arg_size] = '\0';

//printf("argument copied: %s\n", argument);
threadnode->param = argument;






struct ThreadArgs *threadargs = malloc(sizeof(struct ThreadArgs));
//char *filepath = malloc(sizeof(char)*arg_size+1)
threadargs->filepath = argument;

//threadnode->args = threadargs;
*/

puts("a");

//struct ThreadArgs threadargs3 = (*(struct ThreadArgs*)arg);


struct ThreadArgs *threadargs3 = ((struct ThreadArgs*)arg);

//struct ThreadArgs *threadargs2 = (*(struct ThreadArgs**)arg);
//if(threadargs2==NULL){puts("null");}
//puts("here");
//printf("ARGS: %s\n", (threadargs2->filepath));
//threadargs2->filepath = argument;

printf("thread args file path: %s\n", (threadargs3->filepath));
threadnode->args = threadargs3;

puts("b");
//threadnode->args = threadargs3;
/*
puts("c");
pthread_create(&(threadnode->threadID), NULL, start_routine, &(threadargs->filepath));
*/

//pthread_create(&(threadnode->threadID), NULL, start_routine, &(threadargs2->filepath));
pthread_create(&(threadnode->threadID), NULL, start_routine, (threadargs3));

//pthread_create(&(threadnode->threadID), NULL, start_routine, &(threadnode->param));

if(head!=NULL){
threadnode->next = head;
head = threadnode;
}
else{
threadnode->next = NULL;	
head = threadnode;
}
printf("(thread added)\n");
threadsadded++;
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
//puts("before free");
/////free(prev->param);

struct ThreadArgs *threadargs = prev->args;
free(threadargs->filepath);
free(prev->args);
free(prev);
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



void* directoryHandler(void* directory_path){
puts("DIRHANDLER");

struct ThreadArgs *threadargs = (struct ThreadArgs*)directory_path;


//struct ThreadArgs *threadargs = *(struct ThreadArgs**)directory_path;	

printf("thread in dirHandler, pathname3: %s\n", threadargs->filepath);
//printf("thread in fileHandler, pathname: %s\n", *(char**)ptr);
//printf("thread in dirHandler, pathname2: %s\n", (*(struct ThreadArgs**)directory_path)->filepath);

//char* pathname = *(char**)ptr;
//char* pathname = threadargs->filepath;
char* dirpath = threadargs->filepath;




//printf("thread in dirHandler, pathname: %s\n", pathname);
//char* dirpath = (*(char**)directory_path);

DIR *dirptr = opendir(dirpath);
if(dirptr==NULL){
printf("Could not open directory: %s\t", *(char**)directory_path);
perror("error: ");
return directory_path;
}

//pointer for each entry in directory
struct dirent *direntptr;

//iterate through each entry in directory
while ((direntptr = readdir(dirptr))){

//If the entry is a sub directory, print name in blue
if(direntptr->d_type==DT_DIR){
//printf("FOUND SUB DIRECTORY");
//printf("Directory Name: " LTBLUE "./%s\n"  RESETCOLOR, direntptr->d_name);

if (strcmp(direntptr->d_name, ".") == 0 || strcmp(direntptr->d_name, "..") == 0){
continue;
}



char *pathname = appendString(dirpath, direntptr->d_name);
//char slash[] = "/";
char *slash = malloc(sizeof(char)*2);
slash[0]='/';
slash[1]='\0';
char *pathnameSlash = appendString(pathname, slash);
printf("DIRECTORY PATHNAME: %s\n", pathnameSlash);
//printf("");
/////
struct ThreadArgs *threadargs = createThreadArgsStruct(pathnameSlash);

printf("ANOTHER: %s", threadargs->filepath);
head = createThreadandStoreinLinkedList(directoryHandler, threadargs);
////
free(slash);
//free(threadargs);
free(pathname);
free(pathnameSlash);
continue;
}

else if (direntptr->d_type==DT_REG) {
//if regular file, print filename in light green
// printf("Filename: " LTGREEN "%s\n" RESETCOLOR, direntptr->d_name);
 }
  
else{ //else print white
  // printf("Filename: " WHITE "%s\n" RESETCOLOR, direntptr->d_name);
continue;   
} 

//will probably need to append full string here

char* pathname = appendString(dirpath, direntptr->d_name);
//printf("\tpathname: %s\t\n", pathname);

//head = createThreadandStoreinLinkedList(fileHandler, &pathname);


/////
struct ThreadArgs *threadargs = createThreadArgsStruct(pathname);
head = createThreadandStoreinLinkedList(fileHandler, threadargs);
////




if(head==NULL){

}
free(pathname);
//free(threadargs);

}
closedir(dirptr);	

return directory_path;
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

//char *string1 = "/ilab/users/wa125/cs214/asst2testcases/directory2";
//char *string2 = "/";
//appendString(string1, string2);


int arg_size = strlen(argv[1]);
char *argument = malloc(sizeof(char)*arg_size+1);
strncpy(argument, argv[1], arg_size);
argument[arg_size] = '\0';

printf("argument param: %s\n", argument);


//directoryHandler(&argument);


//struct ThreadArgs *threadargs = createThreadArgsStruct(argument);

struct ThreadArgs *threadargs = malloc(sizeof(struct ThreadArgs));
threadargs->filepath = argument;


printf("MAIN: %s\n", threadargs->filepath);
directoryHandler(threadargs);



freeAndJoinLinkedList(head);
free(argument);
free(threadargs);
puts("END OF PROGRAM");
printf("threads added: %d, threads joined: %d", threadsadded, threadsjoined);
return 0;

}
