/*COP 3502C Assignment 2
This program is written by: Aleah Blain*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "leak_detector_c.h" //immediately after including all the header files

#define LINE_SIZE 12
#define EMPTY -1

typedef struct customer
{
	char *name;
	int items;
	int lineNum;
	int time;
}customer;

typedef struct node
{
	struct customer *customer;
	struct node *next;
}node;

typedef struct queue
{
	struct node *front;
	struct node *back;
}queue;


void initialize(queue *q){
	q->front = NULL;
	q->back = NULL;
}

// IN: This function receives the Queue pointer (containing the current front and back of the list)
//     and the integer value needed to create a new node.
// OUT: Then the function sends an integer value of 1 (success) or 0 (failed) to creating a new node.
int enqueue(queue* q, customer* val)
{
  node *temp = malloc(sizeof(node));

  if (temp != NULL) 
  {
    temp->customer = val;
    temp->next = NULL;

    if (q->back != NULL)
      q->back->next = temp;
		
    if (q->front == NULL)
    	q->front = temp;
	
    q->back = temp;
    return 1;
  } else {
    return 0;
	}
}

// IN: This function receives the queue pointer (we will mainly work with front and in a special case the back)
// OUT: returns the value of the front if != NULL, EMPTY otherwise
customer* dequeue(queue *q)
{
  node *temp; 
  customer* retval;

  if (q->front == NULL)
    return NULL;

  temp = q->front;
  retval = q->front->customer;
  q->front = q->front->next;

  if (q->front == NULL)
    q->back = NULL; 

  free(temp);
  return retval;
}

int empty(queue *q)
{
  return (q->front == NULL);
}

customer* peek(queue *q)
{
  if(q->front != NULL)
    return q->front->customer;
  else
    return NULL;
}

customer *fillCustomer(FILE* fp, int *numCustomers)
{
	customer* c = (customer*)malloc( sizeof(customer));
	c->name = (char*)malloc(30 * sizeof(char));
	fscanf(fp, "%d %d %s %d", &c->time, &c->lineNum, c->name, &c->items);
	return c;
}

queue *queueMaker()
{
	queue* q = (queue*)malloc(LINE_SIZE * sizeof(queue));
	queue* lines[LINE_SIZE];
	initialize(q);	
	for(int i = 0; i < LINE_SIZE; i++){
		lines[i] = q;
	}
	return *lines; 
}

void checkout(FILE* fp, FILE* fp2, queue* q, int *numCustomers)
{
	int time, lineNum, n = *numCustomers;
	customer *curr;
	queue *lines = queueMaker();	
	for(int i = 0; i < *numCustomers; i++){
		curr = fillCustomer(fp, numCustomers);
		lineNum = curr->lineNum;
		enqueue(&lines[lineNum - 1], curr);
	}
	
	customer *lowestItems= peek(&lines[0]);
	time = 0;
  
	while(n > 0) 
	{
    int lane = -1;
    int min_items = 1000;
    unsigned long min_time =100000000;
		for(int j = 0; j < LINE_SIZE; j++)
    {
      
			if(empty(&lines[j]) == 1 || lines[j].front->customer->time > time){ 
				continue;
			} 
			lowestItems = peek(&lines[j]);
			
			if(lowestItems->items < min_items) {
					lane = j;
          min_items = lowestItems->items;
      }
    }
		if(lane == -1)
    {
      for(int j = 0; j < LINE_SIZE; j++)
      {
        if(empty(&lines[j]) == 1 )
				{ 
					continue;
			  } 
			  lowestItems = peek(&lines[j]);
			
				if(lowestItems->time < min_time) 
				{
					lane = j;
          min_time = lowestItems->time;
          time = lowestItems->time;
      	}
      }
    }

    time += 30 + lines[lane].front->customer->items * 5;
    lowestItems = peek(&lines[lane]);
    fprintf(fp2,"%s from line %d checks out at time %d.\n", lowestItems->name, lane + 1, time);
    customer * free = dequeue(&lines[lane]);
		free(free->name);
		free(free);
    n--;
	}
	free(lines);
}


int main(void){
atexit(report_mem_leak); //add this line to your code
	FILE *fp = fopen("in.txt", "r");
	FILE *fp2 = fopen("out.txt", "w");
  int numTestCases, numCustomers;
	queue* q;
  fscanf(fp, "%d", &numTestCases);
	for(int i = 0; i < numTestCases; i++){
		fscanf(fp, "%d", &numCustomers);
		checkout(fp, fp2, q, &numCustomers);
	}

	fclose(fp);
	fclose(fp2);

}
	