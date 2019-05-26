#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//create queue
struct node
{
    float custNum;
    float sTime;
    float aTime;
    struct node *next;
};

//queue structure with data for averages
struct queue
{ 
  int custServed;
  int custRemain;
  unsigned int check:1 ;
  float previousSTime;
  float currentATime;
  float timeSpent;
  float timeWait;
  int idleTime;
  float openTime;
  int size;
  struct node *front;
  struct node *rear;
};

struct queue *initQueue()
{
  //create queue and set all value to null and zero
  struct queue *q = (struct queue*)malloc(sizeof(struct queue));
  q->front = NULL;
  q->rear = NULL;
  q->size = 0;
  q->custServed = 0;
  q->custRemain = 0;
  q->check = 0;
  q->timeSpent = 0;
  q->timeWait = 0;
  q->idleTime = 0;
  return q;
}

struct node* newNode(float custNum, float stime, float atime)
{
  //create node, add data and return node
  struct node *temp = (struct node*)malloc(sizeof(struct node));
  temp->sTime = stime;
  temp->custNum = custNum;
  temp->aTime = atime;
  temp->next = NULL;
  return temp;
}

struct node *dequeue(struct queue *q){
  //if queue is empty return null
  if (q->front == NULL)
    return NULL;
  //else pop the front and return the popped node
  struct node *temp = q->front;
  q->front = q->front->next;
  q->size--;
  if(q->front == NULL){
    q->size = 0;
    q->rear = NULL;
  }
  return temp;
}

int enqueue(struct queue *q, float custNum, float stime, float atime){
  //create node to be added
  struct node *temp = newNode(custNum, stime, atime);
  //check for empty queue
  if ( q->rear == NULL)
  {
    q->front = temp;
    q->rear = temp;
    q->size = 1;
    q->custServed++;  
    q->openTime = q->openTime - stime;
    q->timeSpent = q->timeSpent + stime;
    q->previousSTime = stime;
    return 0;
  }
  else
  {
    q->rear->next = temp;
    q->rear = temp;
    q->size++;
  }

  //if still has time, put customer in line
  if ( q->openTime > 0)
  {
    q->custServed++;       
    q->openTime = q->openTime - stime;

    //place arival time    
    float currentWaitTime;
    if ( q->previousSTime > atime)
    {
    currentWaitTime = q->previousSTime - atime;
    q->timeWait = q->timeWait + currentWaitTime;
    }
    else
    {
    currentWaitTime = atime - q->previousSTime;
    q->timeWait = q->timeWait + currentWaitTime;
    }
    //update service time to calculate next cust wait time
    q->previousSTime = stime;

    //update this customer's time
    q->timeSpent = currentWaitTime + q->timeSpent + stime;
  }
  //if out of time increment remain
  else
    q->custRemain++;  
  return 0;
}



int main(){

  //initialize array of 10 queues
  const int SIZE = 10;
  struct queue *q[SIZE];
  //create i for loops
  int i;
  //initialize all queues
  for ( i = 0; i < SIZE; i++){
    q[i] = initQueue();
  }
  //create var for data storage
  float aTime, sTime, custNum;
  //open file for reading
  FILE *file = fopen("customers", "r");
  char c[100];
  float line = 0;
  //if file opened successfully
  if( file!= NULL )
  {
  //get number of lines to determine how long the store was open

  while ( fgets(c, sizeof c, file) != NULL ){
    line++;
  }
  //convert from line to seconds
  for ( i = 0; i < SIZE; i++){
    q[i]->openTime = line*5;
  }
  //check if file was read til the end
  if ( !feof(file))
  {
    printf("Error occurred in first read.\n");
    fclose(file);
    return -1;
  }
  //close file
  fclose(file);
  }
  else 
  {
  perror ("customers");
  }
  float hour = line*5/3600;
  //open new file
  FILE *file2 = fopen("customers","r");
  if ( file2 != NULL )
  { 

    i = 0;

    //loop through data
    while ( fgets(c, sizeof c, file2) != NULL )
    {
      //get data using sscanf
      sscanf(c,"%f%f", &aTime, &sTime);
      //give customer numbers
      custNum = aTime/5;
      //for place customer to checkout counters
      i = i %10;
      

        //if checkout size is empty
        if (q[i]->size == 0)
        {
	  //put customer into this line

          int j;
          for ( j = i; j < SIZE; j++ )
          {
	  //add idle time before
          q[j]->idleTime = q[j]->idleTime + 5;
          }
          //add customer to line
          enqueue(q[i],custNum, sTime, aTime);
        }
        else
        {
          //find smallest index
          int smallestIndex = i;
          int smallest = q[i]->size;
          for ( i = 1; i < SIZE; i++)
          {
             if ( smallest > q[i]->size )
             {
               smallestIndex = i;
               smallest = q[i]->size;
             }
          }
          //place customer at smallest line
          enqueue( q[smallestIndex],custNum, sTime, aTime);
        }
       i++;
    }
    //check if file read was a success
    if ( feof(file2))
    {
      printf("File read successfully.\n");
    }
    else
      printf("Error occurred\n");
  //init var to calculate averages
  float served = 0;
  int remain = 0;
  float sInline = 0;
  float wTime = 0;
  int idle= 0;
  //calculate averages
  for ( i = 0; i < SIZE; i++ )
  {
    served = served + q[i]->custServed;
    remain = remain + q[i]->custRemain;
    wTime = wTime + q[i]->timeWait;
    idle = idle + q[i]->idleTime;
    sInline = sInline + q[i]->timeSpent;
  }
  idle = idle/SIZE;
  sInline = sInline/served;
  wTime = wTime/served;
  //output data
  printf("After %.0f hour:\n", hour);
  printf("Customers served : %.0f\n", served);
  printf("Customers remaining: %d\n", remain);
  printf("Average waiting time: %.0f sec\n", wTime);
  printf("Average time spent in line: %.0f sec\n", sInline);
  printf("Average idle time: %d sec\n", idle);
  }
  else
    perror ("customers");
  //free allocated memory and close file
  for ( i = 0; i < SIZE; i++){
    free(q[i]);
  }
  fclose(file2);
}



