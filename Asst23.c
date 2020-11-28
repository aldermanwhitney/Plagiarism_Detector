#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <math.h>

#define LTBLUE "\x1B[38;5;33m"
#define LTGREEN "\x1B[38;5;48m"
#define BOLDRED "\e[1;31m"
#define WHITE   "\x1B[37m"
#define RESETCOLOR "\x1B[0m"

//run: ./detector /ilab/users/wa125/cs214/hw3/  
// ./detector ./
//./detector /ilab/users/wa125/cs214/asst2testcases/  
//./detector ./../Plagiarism_Detector/direct1/ 
//./detector ../Plagiarism_Detector/direct1/
//./detector ./ 
//./detector ./direct1/   

//static pthread_mutex_t threadLLmut = PTHREAD_MUTEX_INITIALIZER;

struct TokenNode{
char *token;
double probability;
int numOccurances;
int size;
struct TokenNode *next;
};

struct FileNode{
char *filepath;
double num_tokens;
struct TokenNode *nexttoken;
struct FileNode *nextfile;
};

struct FileNode *filehead = NULL;

//Node used to create linked list of mean probabilities
struct MeanProbNode{
char *token;
double meanProb;
struct MeanProbNode *nextmp;
};





struct TokenNode* createTokenNode(char *token){

//copy token into the heap
int arg_size = strlen(token);
char *argument = malloc(sizeof(char)*arg_size+1);
strncpy(argument, token, arg_size);
argument[arg_size] = '\0';
//printf("copied: %s", argument);
			
//create token node
struct TokenNode *tokennode = malloc(sizeof(struct TokenNode));
//tokennode->token = token;
tokennode->token = argument;
tokennode-> probability = 0;
tokennode-> next = NULL;
tokennode->size = arg_size;
tokennode->numOccurances = 1;

free(token);
return tokennode;	
}


/**Function compares two tokens alphabetically
 *Returns -1 if token1 should after  token2
 Returns 0 if they are equal
 Returns 1 if token2 should come after token1
 */
int compare(char *token1, char *token2){

int defaultValue = 0;
int smallestSize;

//Compare lengths to order shorter words before longer words (ie, an before and)
if(strlen(token1)<strlen(token2)){
smallestSize = strlen(token1);
defaultValue = 1;
}
else if(strlen(token2)<strlen(token1)){
smallestSize = strlen(token2);
defaultValue = -1;
}
else{
smallestSize = strlen(token2);
defaultValue = 0;
}

//printf("compare %s <-> %s defaultValue:%d \n", token1, token2, defaultValue);
for(int i=0; i<smallestSize; i++){
	
if(token1[i]<token2[i]){

//printf("compare %s <-> %s defaultValue:%d return 1 \n", token1, token2, defaultValue);
//puts("compare finished");
return 1;
}
if(token1[i]>token2[i]){

//printf("compare %s <-> %s defaultValue:%d return -1\n", token1, token2, defaultValue);
//puts("compare finished");
return -1;
}


}


//printf("compare %s <-> %s defaultValue:%d  return 0 \n", token1, token2, defaultValue);
return defaultValue;	
}



void addTokenNodetoLL(struct TokenNode *tokennode, struct FileNode *filenode){
	
if(filenode==NULL){
printf("Attempted to add token node to null file node");
return;
}


//increment token count
filenode->num_tokens = ((filenode->num_tokens) + 1);

//case where file node has no token nodes yet
if(filenode->nexttoken==NULL){
filenode->nexttoken = tokennode;
return;
}


struct TokenNode *curr = filenode->nexttoken;
struct TokenNode *prev = NULL;

while(curr!=NULL){

//token to insert must be inserted here
if(compare((curr->token), (tokennode->token))==-1){

if(prev!=NULL){
tokennode->next = curr;
prev->next = tokennode;
return;	
}
else{
tokennode->next = curr;
filenode->nexttoken = tokennode;
return;
}
}

//found the same token
if(compare((curr->token), (tokennode->token))==0){

curr->numOccurances = (curr->numOccurances) + 1;
return;
}	
	
prev = curr;
curr = curr->next;
}
prev->next = tokennode;
puts("NO ORDERING");
return;	
}

/**Function takes a pointer to a Struct Filenode
 *Computes probabilities for each token based on total tokens and number of occurances
 *Adds these values to struct TokenNode tokens
 *Will return immediately if there are no tokens, or filenode is NULL
 */
void computeProbabilities(struct FileNode *filenode){
	
if(filenode==NULL){
printf("Attempted to compute probabilities on null file node");
return;
}

//case where file node has no token nodes
if(filenode->nexttoken==NULL){
return;
}


double total_tokens = filenode->num_tokens;

struct TokenNode *curr = filenode->nexttoken;

while(curr!=NULL){
curr->probability = ((curr->numOccurances)/total_tokens);
curr = curr->next;
}
return;	
}


struct FileNode* createFileNode(char *filepath){

struct FileNode *filenode = malloc(sizeof(struct FileNode));
filenode->filepath = filepath;
filenode->num_tokens = 0;
filenode->nexttoken = NULL;
filenode->nextfile = NULL;

return filenode;
}



void addFileNodetoLL(struct FileNode *filenode, struct FileNode **headptr){

//printf("filenode filepath: %s", filenode->filepath);

if((*headptr)==NULL){
printf("head is null %s\n", filenode->filepath);
////headptr = &filenode;
*headptr = filenode;
return;
}

//printf("headptr filepath: %s", (*headptr)->filepath);

struct FileNode *curr = *headptr;
struct FileNode *prev = NULL;
//printf("HERE 2 %s", filenode->filepath);

//printf("curr filepath: %s", curr->filepath);
while(curr!=NULL){
prev = curr;
curr = curr->nextfile;
}
prev->nextfile = filenode;

return;	
}


void sortFileNodeLL(struct FileNode **headptr){

if((*headptr)==NULL){
printf("head is null");
return;
}


struct FileNode *curr = *headptr;
//struct FileNode *prev = NULL;

struct FileNode *swapNode = *headptr;
struct FileNode *minNode = curr;
double minNumTokens = curr->num_tokens;


while(swapNode!=NULL){
//printf("\nSWAP NODE: %s,  %f", swapNode->filepath, swapNode->num_tokens);



curr=swapNode;
minNode = curr;
minNumTokens = swapNode->num_tokens;
//find node after swap node with lowest number of tokens
while(curr!=NULL){

if((curr->num_tokens)<minNumTokens){
minNumTokens = curr->num_tokens;
minNode = curr;
//printf("\nMINIMUM NODE FOUND: %s,  %f", minNode->filepath, minNode->num_tokens);
}	
//prev = curr;
curr = curr->nextfile;
}


//printf("\nMINIMUM NODE IS %s, num tokens: %f", minNode->filepath, minNode->num_tokens);

//swap values stored in minNode and swapNode
//store info from minimum node in temp variable for swap
struct FileNode *temp = malloc(sizeof(struct FileNode));
temp->filepath = minNode->filepath;
temp->num_tokens = minNode->num_tokens;
temp->nexttoken = minNode->nexttoken;
//temp->nextfile = minNode->nextfile;

minNode->filepath = swapNode->filepath;
minNode->num_tokens = swapNode->num_tokens;
minNode->nexttoken = swapNode->nexttoken;
//minNode->nextfile = swapNode->nextfile;

swapNode->filepath = temp->filepath;
swapNode->num_tokens = temp->num_tokens;
swapNode->nexttoken = temp->nexttoken;
//swapNode->nextfile = temp->nextfile;

free(temp);
//increment swapnode
swapNode = swapNode->nextfile;
}

return;	
}



void printFileNodeLL(struct FileNode **headptr){

if((*headptr)==NULL){
printf("attempt to print LL: head points to null\n");
return;
}

	
/*	
if (filehead==NULL){
printf("filehead is null");
return;
}

struct FileNode *curr = filehead;
struct TokenNode *currtok;
*/


struct FileNode *curr = *headptr;
struct TokenNode *currtok;


int filecount = 0;
while(curr!=NULL){
printf("____________________________\n");
printf("filepath: %s\n", curr->filepath);	
printf("numTokens: %f\n", curr->num_tokens);


int fd = open(curr->filepath, O_RDONLY);

   //take care of case where open returns -1
   if(fd<0){
   perror("Line 83: Could not open file\n");
   }


   off_t bytesInFile = lseek(fd,0, SEEK_END);
	
   //return to beggining of file
   lseek(fd, 0, SEEK_SET);
   //Take care of case where lseek returns -1
   if(bytesInFile==-1){
   printf("Could not read file\n");
   }
   else{
   printf("  size: %li\n", bytesInFile);
   }


int buffersize = 200;
char buf[buffersize];
int bytesread;

while((bytesread = read(fd, buf, 200))>0){
printf("read %d bytes. \nText in File:\n\n %s\n", bytesread, buf);
}

close(fd);






currtok = curr->nexttoken;
while(currtok!=NULL){
printf("\t\n-->| token: %s", currtok->token);
printf("\t probability: %f|", currtok->probability);
printf("\t size: %d|", currtok->size);
printf("\t Number of Occurances: %d|", currtok->numOccurances);
currtok = currtok->next;
}


printf("\n____________________________\n");
printf("\t\t\t|\n");
printf("\t\t\tv\n");
filecount++;
curr = curr->nextfile;
}
printf("NULL\n");
printf("files in linked list: %d", filecount);

}


void freeFileNodeLL(struct FileNode **headptr){

if((*headptr)==NULL){
printf("attempt to free LL: head points to null\n");
return;
}


struct FileNode *curr = *headptr;
struct FileNode *prevfile = NULL;
struct TokenNode *currtok;
struct TokenNode *prevtok = NULL;


int filecount = 0;
while(curr!=NULL){

currtok = curr->nexttoken;

while(currtok!=NULL){
prevtok=currtok;
currtok = currtok->next;
free(prevtok->token);
free(prevtok);
}

filecount++;
prevfile = curr;
curr = curr->nextfile;
free(prevfile->filepath);
free(prevfile);
}

printf("\nfiles freed in linked list: %d", filecount);

}



/**Struct to hold arguments needed by threads
 */
struct ThreadArgs{
char *filepath;
pthread_mutex_t *lockptr;
struct FileNode **headptr;
};

int threadsadded = 0;
int threadsjoined = 0;

/**Function takes values for ThreadArgs fields
 *and creates a newly malloc'ed ThreadArg struct
 returns pointer to it
 */
struct ThreadArgs* createThreadArgsStruct(char *filepath, pthread_mutex_t *lockptr, struct FileNode **headptr){

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
threadargs->lockptr = lockptr;
threadargs->headptr = headptr;

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


char* substring(char* string, int begin, int end){


int substr_length = end - begin + 1;

char *substr = malloc(sizeof(char)*(substr_length+1));


for(int i = 0; i<substr_length; i++){
substr[i] = string[begin+i];
}
substr[substr_length] = '\0';


return substr;	
}

/**Function creates a new string from the given string
 *which removes all illegal characters (anything not alphabetical, or a dash)
 */
char* removeUnwantedChars(char* string){

int newsize = 0;

//iterate once through to determine size needed
for(int i = 0; i<strlen(string); i++){
if(isalpha(string[i])|| string[i]=='-'){
newsize++;
}
}

////printf("new size: %d\n", newsize);


char* result = malloc(newsize+1);

int k = 0;
for(int j = 0; j<strlen(string); j++){
//printf("string k: %c\n", string[k]);
if(isalpha(string[j]) || string[j]=='-'){
result[k] = string[j];
k++;
}

}
result[newsize]='\0';

//printf("RETURN\n");
//free(string);
return result;
}





/**Function takes a pointer to a thread arg struct as a void pointer
 *handles file operations, and returns the same pointer
 */
void* fileHandler(void* ptr){

struct ThreadArgs *threadargs = ((struct ThreadArgs*)ptr);

printf("thread in fileHandler, pathname3: %s\n", threadargs->filepath);

char* pathname = threadargs->filepath;
pthread_mutex_t *lockptr = threadargs->lockptr;
struct FileNode **headptr = threadargs->headptr;


int fd = open(pathname, O_RDONLY);

   //take care of case where open returns -1
   if(fd<0){
   perror("Line 83: Could not open file\n");
   return ptr;
   }


   off_t bytesInFile = lseek(fd,0, SEEK_END);
	
   //return to beggining of file
   lseek(fd, 0, SEEK_SET);
   //Take care of case where lseek returns -1
   if(bytesInFile==-1){
   printf("Could not read file\n");
   }
   else{
   printf("  size: %li\n", bytesInFile);
   }





//lock access of shared data structure: FileNodes Linked List
pthread_mutex_lock(lockptr);  
struct FileNode *filenode = createFileNode(pathname);   
addFileNodetoLL(filenode, headptr);
//printFileNodeLL();
pthread_mutex_unlock(lockptr);

//read one token to the buffer at a time
//tokenize the token, then read next token to the buffer


//read as much as possible into the buffer
//tokenize token
//again read as much as possible into the buffer   
//from offset

//read 10 bytes into the buffer
//tokenize
//if the last char was not a new line or a space
//read totalLastRead - tokencutoff  (ex 8 bytes), throw away
//read next 10 bytes into buffer


int buffersize = 100000;
char buf[buffersize];
int bytesread;

int totalRead = 0;


//read into buffer 15 bytes at a time
while((bytesread = read(fd, buf, 100000))>0){
////printf("read %d bytes. Buffer: %s\n", bytesread, buf);
totalRead+=bytesread;


//iterate over buffer char by char to tokenize
int i = 0;
int tokenbegin = 0;
int tokenend = 0;
while (i<bytesread){
//if the last character read is not whitespace, rollback next read to include it	
if(!isspace(buf[i]) && (i==bytesread-1) && (totalRead<bytesInFile)){

int fileposition = totalRead - bytesread + tokenbegin;
totalRead = totalRead-bytesread+tokenbegin;

//rewind pointer to begging of token
lseek(fd, fileposition, SEEK_SET);
break;
}
//if white space reached and not at beginning of position, tokenize previous	
if(isspace(buf[i]) && (i!=0)){
//need a substring method!
char *prelimtoken = substring(buf, tokenbegin, tokenend-1);
////printf("Prelim Token: %s\n", prelimtoken);
char *finaltoken = removeUnwantedChars(prelimtoken);
////printf("Final Token: %s\n", finaltoken);
struct TokenNode *tokennode = createTokenNode(finaltoken);   
addTokenNodetoLL(tokennode, filenode);
free(prelimtoken);

//reset variables
tokenbegin = i+1;
tokenend = i+1;
}
//if reached an alphabetical char, make lowercase
else if(isalpha(buf[i])){
buf[i]=tolower(buf[i]);
tokenend++;
}
else if(buf[i]=='-'){
tokenend++;
}
else if(isspace(buf[i])){
buf[i]='S';
tokenbegin=i+1;
}
else{
tokenend++;
}

i++;	
}






}
computeProbabilities(filenode);
   close(fd);
  
////pthread_mutex_unlock(lockptr);

return ptr;
}

/*Function takes a void/void function and a pointer to a ThreadArg struct
 * Creates a new thread and stores it in a newly malloced ThreadNode
 *Also adds it to a linked list for later freeing
 */
struct ThreadNode* createThreadandStoreinLinkedList(void *(*start_routine) (void *), void *arg){ 

struct ThreadNode *threadnode = malloc(sizeof(struct ThreadNode));

struct ThreadArgs *threadargs = ((struct ThreadArgs*)arg);
//printf("thread args file path: %s\n", (threadargs->filepath));
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


//printf("(thread added), position: %d\n", pos);
threadsadded++;
return head;
}


/**Function iterates through linked list of ThreadNodes 
 *and joins each thread, starting at the head
 */
void freeAndJoinLinkedList(struct ThreadNode* head){
////puts("in free and join linked list");

if (head==NULL){
return;
}

struct ThreadNode *prev = NULL;
struct ThreadNode *current = head;

if(prev){}

while (current!=NULL){
//puts("before join");

////printf("Thread ID: %ld", current->threadID);
int error = pthread_join(current->threadID, NULL);	
if (error){
puts("COULD NOT JOIN THREADS");
}
////printf("(thread joined)\n");
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

//printf("thread in dirHandler, pathname: %s\n", threadargs->filepath);

//extract necc. info
char* dirpath = threadargs->filepath;
pthread_mutex_t *lockptr = threadargs->lockptr;
struct FileNode **headptr = threadargs->headptr;

DIR *dirptr = opendir(dirpath);
if(dirptr==NULL){
printf("Could not open directory: %s\t", *(char**)ptr);
perror("error: ");
//freeAndJoinLinkedList(head);
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
//printf("DIRECTORY PATHNAME: %s\n", pathnameSlash);

//create new ThreadArgsStruct out of subdirectory pathname
struct ThreadArgs *threadargs = createThreadArgsStruct(pathnameSlash, lockptr, headptr);

//printf("ANOTHER: %s", threadargs->filepath);

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


struct ThreadArgs *threadargs = createThreadArgsStruct(pathname, lockptr, headptr);
createThreadandStoreinLinkedList(fileHandler, threadargs);

free(pathname);

}
}

closedir(dirptr);	

//freeAndJoinLinkedList(head);
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



/**Given a pointer to a MeanProbNode to add to the LL, and a pointer to the last added node
 *The function adds the MeanProbNode to the last poisition in the linked list
 *And returns the updated pointer to the last added node
 */
struct MeanProbNode* addMeanProbNodetoLL(struct MeanProbNode *mpnode, struct MeanProbNode *lastadded){

if(mpnode==NULL){
printf("Error, attempted to add a null node\n");
return lastadded;
}


//no nodes yet added to the list
if(lastadded==NULL){
mpnode->nextmp = NULL;
lastadded = mpnode;
//printf("first add\n");
return lastadded;
}



lastadded->nextmp=mpnode;
mpnode->nextmp = NULL;
lastadded = mpnode;

//printf("another add\n");
return lastadded;

	
}

/**Function takes two FileNode pointers
 *Creates a linked list of MeanProbNodes 
 *for the mean probability of tokens in both lists
 *Returns a pointer to the head of the MeanProbNode linked list
 */
struct MeanProbNode* createMeanProbTokenList(struct FileNode *file1, struct FileNode *file2){

if(file1==NULL || file2==NULL){
printf("one of the files is null\n");
return NULL;
}


if(file1->nexttoken==NULL || file2->nexttoken==NULL){
printf("one of the files is empty\n");
return NULL;
}

//pointers for traversing the list
struct TokenNode *file1tokenptr = file1->nexttoken;
struct TokenNode *file2tokenptr = file2->nexttoken;

//pointers for head and tail of list
struct MeanProbNode *lastadded = NULL;
struct MeanProbNode *head = NULL;


double file1tokens = file1->num_tokens;
double file2tokens = file2->num_tokens;
double totaltokens = file1tokens + file2tokens;

printf("total tokens: %f\n", totaltokens);

//iterate the two (sorted) linked lists of token nodes
while((file1tokenptr!=NULL) || file2tokenptr!=NULL){

//if we've reached the end of file two's LL, append rest of file one's tokens to the MeanProb LL
if (file1tokenptr!=NULL && file2tokenptr==NULL){

while(file1tokenptr!=NULL){
struct MeanProbNode *mpn = malloc(sizeof(struct MeanProbNode));
mpn->token = file1tokenptr->token;
mpn->meanProb = ((file1tokenptr->probability)/2);

//add to linked list
lastadded = addMeanProbNodetoLL(mpn, lastadded);
if(head==NULL){
head = lastadded;
}

file1tokenptr=file1tokenptr->next;
}

break;	
}

//if we've reached the end of file one's LL, append rest of file two's tokens to the mean prob LL
else if (file1tokenptr==NULL && file2tokenptr!=NULL){

while(file2tokenptr!=NULL){
struct MeanProbNode *mpn = malloc(sizeof(struct MeanProbNode));
mpn->token = file2tokenptr->token;
mpn->meanProb = ((file2tokenptr->probability)/2);

//add to linked list
lastadded = addMeanProbNodetoLL(mpn, lastadded);
if(head==NULL){
head = lastadded;
}


file2tokenptr=file2tokenptr->next;
}

break;	
}
//if the tokens are equal
else if(strcmp(file1tokenptr->token, file2tokenptr->token)==0){

printf("tokens equal\n");
struct MeanProbNode *mpn = malloc(sizeof(struct MeanProbNode));
mpn->token = file1tokenptr->token;
mpn->meanProb = (((file1tokenptr->probability)+(file2tokenptr->probability))/2);

//function to add token to linked list, possibly return a pointer to the last added
lastadded = addMeanProbNodetoLL(mpn, lastadded);
if(head==NULL){
head=lastadded;
}
//advance both pointers
file1tokenptr = file1tokenptr->next;
file2tokenptr = file2tokenptr->next;
}
//if file one's token is alphabetically before file two's token
else if(strcmp(file1tokenptr->token, file2tokenptr->token)<0){

printf("file1<file2\n");

struct MeanProbNode *mpn = malloc(sizeof(struct MeanProbNode));
mpn->token = file1tokenptr->token;
mpn->meanProb = ((file1tokenptr->probability)/2);

//function to add token to linked list, possibly return a pointer to the last added

lastadded = addMeanProbNodetoLL(mpn, lastadded);
if(head==NULL){
head = lastadded;
}
//advance first pointer
file1tokenptr = file1tokenptr->next;
}
//if file two's token is alphabetically before file one's token
else{ //if(strcmp(file1tokenptr->token, file2tokenptr->token)>0){
struct MeanProbNode *mpn = malloc(sizeof(struct MeanProbNode));
mpn->token = file2tokenptr->token;
mpn->meanProb = ((file2tokenptr->probability)/2);

//function to add token to linked list, possibly return a pointer to the last added

printf("file2<file1\n");
lastadded = addMeanProbNodetoLL(mpn, lastadded);
if(head==NULL){
head = lastadded;
}
//advance first pointer
file2tokenptr = file2tokenptr->next;
}


}



return head;
}

/**Function which takes a pointer to the head of a MeanProbNode linked list
 *and prints the contents of the list
 *Used for debugging
 */
void printMeanProbLL(struct MeanProbNode *head){

struct MeanProbNode *curr = head;
while(curr!=NULL){
printf("Token: %s MeanProbability: %f\n", curr->token, curr->meanProb);	
curr=curr->nextmp;	
}

return;	
}


double computeJensenShannonDistance(struct MeanProbNode *head, struct FileNode *file1, struct FileNode *file2){
if(head==NULL || file1==NULL || file2==NULL){
return -1;
}

struct TokenNode *file1ptr = file1->nexttoken;	
struct TokenNode *file2ptr = file2->nexttoken;	
struct MeanProbNode *meanptr = head;

double KLDfile1 = 0;
double KLDfile2 = 0;
double JSD = 0;


//iterate through file1 and find matching tokens from mean list
while(file1ptr!=NULL){

//found a match, add calculation to KLD sum
if(strcmp(file1ptr->token, meanptr->token)==0){
KLDfile1 += ((file1ptr->probability)*log10((file1ptr->probability)/(meanptr->meanProb)));
meanptr=meanptr->nextmp;
file1ptr=file1ptr->next;
}
//mean list has a node not in file1, advance mean ptr
else{ //if(strcmp(file1ptr->token, meanptr->token)<0){
meanptr=meanptr->nextmp;
}	
	
}
printf("KLD file 1: %f\n", KLDfile1);

meanptr = head;

//iterate through file2 and find matching tokens from mean list
while(file2ptr!=NULL){
//aputs("here: %s, %s\n", file2pt);
//found a match, add calculation to KLD sum
if(strcmp(file2ptr->token, meanptr->token)==0){
KLDfile2 += ((file2ptr->probability)*log10((file2ptr->probability)/(meanptr->meanProb)));
meanptr=meanptr->nextmp;
file2ptr=file2ptr->next;
}
//mean list has a node not in file1, advance mean ptr
else{ //if(strcmp(file1ptr->token, meanptr->token)<0){
meanptr=meanptr->nextmp;
}	
	
}
printf("KLD file 2: %f\n", KLDfile2);



JSD = ((KLDfile1+KLDfile2)/2);


return JSD;



}



int main(int argc, char** argv){
printf("argc: %d\n", argc);
printf("argv: %s\n", *argv);

if(argc!=2){
return 1;
}

printDirectoryContents(argv[1]);

pthread_mutex_t *lock = malloc(sizeof(pthread_mutex_t));
pthread_mutex_init(lock, NULL);

//struct FileNode *headptr = malloc(sizeof(struct FileNode*));
struct FileNode *headptr = NULL;
//headptr = NULL;

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
threadargs->lockptr = lock;
threadargs->headptr = &headptr;
//threadargs->filepath = argv[1]; this works too

directoryHandler(threadargs);

////printFileNodeLL((&headptr));

//directoryHandler(&argv[1]);

freeAndJoinLinkedList(head);

printf("threads added: %d, threads joined: %d", threadsadded, threadsjoined);
////printFileNodeLL((&headptr));
//freeAndJoinLinkedList(head);
////printFileNodeLL(&headptr);
sortFileNodeLL(&headptr);
printFileNodeLL((&headptr));


struct FileNode *file1ptr = headptr;
struct FileNode *file2ptr = file1ptr->nextfile;
struct MeanProbNode *headptr2 = NULL;

while(file1ptr!=NULL){

printf("file 1: %s\n\n", file1ptr->filepath);	

file2ptr = file1ptr->nextfile;
while(file2ptr!=NULL){

printf("file 2: %s\n", file2ptr->filepath);		
headptr2 = createMeanProbTokenList(file1ptr, file2ptr);

printf("head points to: %s\n", headptr2->token);
printMeanProbLL(headptr2);

double result = computeJensenShannonDistance(headptr2, file1ptr, file2ptr);
printf("JSD: %f", result);
file2ptr=file2ptr->nextfile;	
}


	
file1ptr = file1ptr->nextfile;	
//file2ptr = file1ptr->nextfile;
}







freeFileNodeLL(&headptr);
free(argument);
free(threadargs);
puts("END OF PROGRAM");

//printf("threads added: %d, threads joined: %d", threadsadded, threadsjoined);
puts("\nhere");
return 0;
}
