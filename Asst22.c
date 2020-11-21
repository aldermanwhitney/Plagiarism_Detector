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

//static pthread_mutex_t threadLLmut = PTHREAD_MUTEX_INITIALIZER;

/**Linked List to keep track of threads in use
 * Used for properly joining all threads
 */
struct ThreadNode{
pthread_t threadID;
char *param;
struct ThreadNode *next;
};
struct ThreadNode* head = NULL;


void* fileHandler(void* ptr){
printf("thread in fileHandler, pathname: %s\n", *(char**)ptr);
char* pathname = *(char**)ptr;
int fd = open(pathname, O_RDONLY);

   //take care of case where open returns -1
   if(fd<0){
   perror("Line 83: Could not open file");
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
printf("arguemnt: %s\n", (*(char**)arg));

int arg_size = strlen((*(char**)arg));

printf("arg size: %d", arg_size);

char *argument = malloc(sizeof(char)*arg_size+1);

strncpy(argument, (*(char**)arg), arg_size);
argument[arg_size] = '\0';

puts("HERE");
printf("argument copied: %s\n", argument);
threadnode->param = argument;

pthread_create(&(threadnode->threadID), NULL, start_routine, &(threadnode->param));

if(head!=NULL){
threadnode->next = head;
head = threadnode;
}
else{
threadnode->next = NULL;	
head = threadnode;
}
puts("thread added");

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

//printf("Thread ID: %ld", current->threadID);
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


/**Function which takes two pointers to char arrays
 * And returneds a pointer to a newly allocated char array
 * which contains the second string appended to the first
 */
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
//printf("end of string cat function\n");

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
puts("here");
printf("\tpathname: %s\t", pathname);

head = createThreadandStoreinLinkedList(fileHandler, &pathname);

if(head==NULL){

}
free(pathname);
}
closedir(dirptr);	
}

/**Prints Directory
 *Used for debugging
 */
void printDirectoryContents2(char* directory_path){

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
	         continue;    
	      	}

	      //will probably need to append full string here
	
         char* pathname = appendString(directory_path, direntptr->d_name);
         printf("\tpathname: %s\t", pathname);
         int fd = open(pathname, O_RDONLY);
	
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
  printf("\nEND OF PRINT DIRECTORY FUNCTION\n\n");
  }


int main(int argc, char** argv){
printf("argc: %d\n", argc);
printf("argv: %s\n", *argv);

if(argc!=2){
return 1;
}

printDirectoryContents2(argv[1]);
printDirectoryContents(argv[1]);
freeAndJoinLinkedList(head);

return 0;

}
