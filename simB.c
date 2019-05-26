#include <stdio.h>
#include <stdlib.h>

//struct for checkout
struct checkout
{
  float ocupy;
  int idle;
  unsigned int positive:1 ;
  float openTime;
  float cWaitTime;
  float cInlineTime;
  float aSTime;
  int served;
};

//create queue
struct node
{
  float aTime;
  float sTime;
  struct node *next;
};

struct queue
{
  int size;
  struct node *front;
  struct node *rear;
};

struct queue *initQueue()
{
  struct queue *q = (struct queue*)malloc(sizeof(struct queue));
  q->front = NULL;
  q->rear = NULL;
  return q;
}

struct node* newNode(float atime, float stime)
{
  struct node *temp = (struct node*)malloc(sizeof(struct node));
  temp->aTime = atime;
  temp->sTime = stime;
  temp->next = NULL;
  return temp;
}

struct node *dequeue(struct queue *q){
  if (q->front == NULL)
    return NULL;
  struct node *temp = q->front;
  q->front = q->front->next;
  if(q->front == NULL)
    q->rear = NULL;
  q->size--;
  return temp;
}

void enqueue(struct queue *q, float atime, float stime){
  struct node *temp = newNode(atime, stime);
  if ( q->rear == NULL)
  {
    q->front = temp;
    q->rear = temp;
  }
  else
  {
    q->rear->next = temp;
    q->rear = temp;
  }
  q->size++;
}

int main(){
  //initialize var
  float atime, stime;
  //create queue and node
  struct queue *q;
  struct node *cust;
  //initialize q to zeros
  q = initQueue();
  //make char array of 100
  char c[100];
  const int SIZE = 10;
  //init var for index and line for checkout and queue
  int line = 0, i, biggestIndex = 0;
  float biggestOTime;
  //create 10 checkout
  struct checkout checkout[SIZE];
  //read customer to determine num of line
  FILE *file = fopen("customers", "r");
  if( file!= NULL ){
    //read one line at a time
    while ( fgets(c, sizeof c, file) != NULL){
	//increment line and scan and enqueue
        line++;
	sscanf(c, "%f%f\n", &atime, &stime);
        enqueue(q, atime, stime);
    }   
    //check if file end was reached
    if ( !feof(file)){
      printf("Error occurred in read.\n");
      fclose(file);
      return -1;
    }
    //close file
    fclose(file);
  }
  //if file was not read
  else
    perror("customers");

  //init variable
  float waitedTime;
  int j;
  int custRemain = 0;
  //operation seconds
  float oSec = line*5;
  //set all checkout data to 0
  for (i = 0; i< SIZE; i++){
    //checkout openTime will be = to oSec
    checkout[i].openTime = oSec;
    checkout[i].served = 0;
    checkout[i].positive = 0;
    checkout[i].idle = 0;
    checkout[i].ocupy = 0;
    checkout[i].cInlineTime = 0;
    checkout[i].aSTime = 0;
    checkout[i].cWaitTime = 0;
  }
  
  //set default operation time
  biggestOTime = checkout[0].openTime;
  biggestIndex = 0;
  
  
    //loop based on line in customer
    int loop;
    for ( loop = 0; loop < line; loop++){
 
  //find checkout with biggest operation time
    for (j = 0; j <SIZE ; j++)
    {
      //if checkout j opentime is bigger than default biggest open time
      if ( (checkout[j].openTime > biggestOTime) && (checkout[j].positive ==0))
      {
        //make biggest index = new biggest index (j)
        biggestIndex = j;
        biggestOTime = checkout[j].openTime;
        //set new biggest open time
      }
    }
      //find difference of biggest checkout time and original open time
      float laspe = oSec - checkout[biggestIndex].openTime;
      //if difference is less than first customer in queue
      if ( laspe < q->front->aTime)
      { 
        //add difference to idle
	checkout[biggestIndex].idle = 
		checkout[biggestIndex].idle +
		(q->front->aTime - laspe);
      }
    //if no customer arrived yet 
    if ( checkout[biggestIndex].openTime == oSec)
    {
      //increment customer served   
      checkout[biggestIndex].served++;
      //add customer service time
      checkout[biggestIndex].cInlineTime =  
		checkout[biggestIndex].cInlineTime +
		q->front->sTime;
      //first customer so no wait time
      checkout[biggestIndex].cWaitTime = 0;
      //subtract opentime with by service time
      checkout[biggestIndex].openTime = 
		checkout[biggestIndex].openTime -
		q->front->sTime; 
      //remember customer service time
      checkout[biggestIndex].aSTime = q->front->sTime;
    //dequeue customer
	dequeue(q);
    }
    //if opentime is still available
    else if ( checkout[biggestIndex].openTime > 0)
    {
      //increment served
      checkout[biggestIndex].served++;
      //dequeue into customer
      cust = dequeue(q);

      //subtract opentime with cust sTime
      checkout[biggestIndex].openTime = 
 		 checkout[biggestIndex].openTime - 
		 cust->sTime;
      //determine wait time
      if ( checkout[biggestIndex].aSTime > cust->aTime)
      {
        //get waited time
	waitedTime = checkout[biggestIndex].aSTime - cust->aTime;
        //record waited time into checkout
	checkout[biggestIndex].cWaitTime =  
		checkout[biggestIndex].cWaitTime + waitedTime;
        //record total time spent inline
	checkout[biggestIndex].cInlineTime = 
		checkout[biggestIndex].cInlineTime +
		waitedTime + cust->sTime;
      }
      else{
        //get wait time by subtracting bigger from smaler
	waitedTime = cust->aTime - checkout[biggestIndex].aSTime;
        //record wait time
	checkout[biggestIndex].cWaitTime = 
		checkout[biggestIndex].cWaitTime + waitedTime;
        //record total time
	checkout[biggestIndex].cInlineTime = 
		checkout[biggestIndex].cInlineTime +
		waitedTime + cust->sTime;
      }
      //record new sTime
      checkout[biggestIndex].aSTime = cust->sTime;
    }
    //if line is negative increment remaining customer
    else
      custRemain++;      
    //if opentime is negative change positive
    if (checkout[biggestIndex].openTime < 0)
      checkout[biggestIndex].positive = 1;
  //set checkouts open time
  biggestOTime = checkout[biggestIndex].openTime;
  }

  //declare variable to store averages
  int served = 0;
  int idle = 0;
  waitedTime = 0;
  float inlineTime = 0;
  //calculate averages
  for (i = 0; i<SIZE; i++){
    served = served + checkout[i].served;
    idle = idle + checkout[i].idle;
    waitedTime = waitedTime + checkout[i].cWaitTime;
    inlineTime = inlineTime + checkout[i].cInlineTime;
  }
  idle = idle/SIZE;
  waitedTime = waitedTime/served;
  inlineTime = inlineTime/served;
  //display results
  printf("Customers served: %d\n", served);
  printf("Customers remaining: %d\n", custRemain);
  printf("Average wait time: %.0f sec\n", waitedTime);
  printf("Average time spent inline: %.0f sec\n", inlineTime);
  printf("Average idle time: %d sec\n", idle);
  //free allocated mem
  free(q);
}
