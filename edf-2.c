/*******************************************************************************
* Filename : edf.c
* Author : Alisha Patel
* Date : 3/1/24
* Description : Earliest Deadline First Scheduling Algorithm
* Pledge : I pledge my honor that I have abided by the Stevens Honor System. 
******************************************************************************/
//max_lcm inspired from: https://www.geeksforgeeks.org/lcm-of-given-array-elements/

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <stdbool.h>

struct Process{
  int id;
  int cpu_time;
  int period;
  //int remaining_time;
  int deadline;
  int arrival;
} Process;


//A function to calculate the GCD to be used in calculating the LCM
int gcd(int a, int b){
  while(b != 0){
    int temp = b;
    b = a%b;
    a = temp;
  }
  return a;
}
//uses gcd to compute lcm
int lcm(int a, int b){
  int temp = (a*b)/(gcd(a,b));
  return temp;
}
//from the array of periods from proc, calculates the lcm for max time
int max_lcm(int period[], int num_process){
  int result = period[0];
  for(int i = 1; i < num_process; i++){
    result = lcm(period[i], result);
  }
  return result;
}

//check deadlines to find the earliest
int check_deadlines(struct Process* arr_proc, int num_process){
  int ed = arr_proc[0].deadline;
  int edi = 0;
  for(int i = 1; i < num_process; i++){
    if(arr_proc[i].deadline < ed){
      ed = arr_proc[i].deadline;
      //update the index to the process with earliest deadline
      edi = i;
    } 
  }
  return edi;
}
//sorted array of processes
/**
void revised_bubble(struct Process** arr_proc, int* num_process) {
  for (int i = 0; i < *num_process - 1; i++) {
    for (int j = 0; j < *num_process - i - 1; j++) {
      if ((*arr_proc)[j]->id > (*arr_proc)[j + 1]->id) {
        struct Process* temp = (*arr_proc)[j];
        (*arr_proc)[j] = (*arr_proc)[j + 1];
        (*arr_proc)[j + 1] = temp;
      }
    }
  }
}
*/
//missed deadline checking

//checks to see if we missed the deadline, uses sorted array to print out num_process
void missed_deadline(struct Process** arr_proc,int num_process, int current_time){
  struct Process temp[num_process];
  //for(int i = 0; i < num_process; i++){
    //temp[i] = (*arr_proc)[i];
  //}
  
  memcpy(temp, *arr_proc, num_process*sizeof(struct Process));
  //bubble sort
  ///revised_bubble(temp, &num_process);
  bool isSwap = false;
  for (int i =0; i< num_process-1; i++){
   
    for(int j =0; j< num_process-i -1; j++){
      if(temp[j].id >temp[j+1].id){
        struct Process tmp= temp[j];
        temp[j]=temp[j+1];
        temp[j+1]=tmp;
        isSwap = true;
      }
    }
    if(!isSwap){
      break;
    }
  }
 //iterating through sorted array
  for(int i = 0; i < num_process; i++){
    if(temp[i].deadline == current_time && temp[i].cpu_time!=0){
        int next=temp[i].period+temp[i].deadline;
        temp[i].deadline = next;
        printf("%d: process %d missed deadline (%d ms left), new deadline is %d\n", current_time, temp[i].id, temp[i].cpu_time, temp[i].deadline); 
    }
  }
   //update the arr_proc with the same info
  for(int i =0; i < num_process; i++){
    if((*arr_proc)[i].deadline == current_time && (*arr_proc)[i].cpu_time!=0){
        int next=(*arr_proc)[i].period+(*arr_proc)[i].deadline;
        (*arr_proc)[i].deadline = next;
    }
  }
  
  
}
////check for preempted scheduling -- CHECK THIS AGAIN
//return a value for preemptive scheduling or else keep it going
int preemptive(struct Process **arr_proc, int num_process, int index, int *ppid, int time){
  if(num_process==0){
    return 0;
  }
  struct Process* copy = *arr_proc;
  int tmp = check_deadlines(copy, num_process);
  if(tmp != index){
    printf("%d: process %d preempted!\n",time,copy[index].id);
    *ppid = tmp;
    return 1;
  }
  return 0;
}


//adds new process to array
void add_proc(struct Process **first, struct Process add,int* num_process){
  int len = (*num_process) +1;
  if(len == 1){
    free(*first);
    *first= (struct Process*) malloc(len*sizeof(struct Process));
  }
  else if(len>1){
    //free(*first);
    *first= (struct Process*) realloc(*first, len*sizeof(struct Process));
  }
  (*first)[len-1]=add;
  *num_process=len;
  //free(first);
}

//checks to see if we are at a period for a certain tasks, adds new num_process to the array, and prints our current num_process
int update_process(struct Process* first_copy, struct Process **arr_proc, int* num_process, int num, int current_time){
  if(current_time==0){
    printf("%d: processes (oldest first):",current_time);
    for(int i=0; i<(*num_process); i++){
      printf(" %d (%d ms)",(*arr_proc)[i].id, (*arr_proc)[i].cpu_time);
      
    }
    printf("\n");
    return 0;
  }
  //checks first_copy which holds all the information about all num_process 
  struct Process* new=(struct Process*)malloc(sizeof(struct Process)*num);
  int n=0;
  int time=0;
  for(int i =0; i<num;i++){
    if(current_time%first_copy[i].period==0){
     
      first_copy[i].deadline=first_copy[i].deadline+first_copy[i].period;
      first_copy[i].arrival=current_time;
      new[n]=first_copy[i];
      n++;
      time=1;
    }
  }
 
  for(int k=0; k<n; k++){
    add_proc(arr_proc, new[k],num_process);
  }
 
  if (time){
    printf("%d: processes (oldest first):",current_time);
    for(int i=0; i<(*num_process); i++){
      printf(" %d (%d ms)",(*arr_proc)[i].id, (*arr_proc)[i].cpu_time);
      
    }
    printf("\n");
  }
  free(new);
  //free(*first);
  return n;
}

//might want a method for adding and remove of an array

//edf scheduler
// - checks the current index
// - check missed deadlines
// - max time
// - waiting time calculation and check periods for processes
// - preemptive

void edf_schedule(struct Process* arr_proc, int num_process, int max_time){
  int total_waiting=0;
  int total_process=0;
  total_process += num_process;
 
  struct Process *first_copy= (struct Process*)malloc(num_process*sizeof(struct Process));
  int num= num_process;
  memcpy(first_copy,arr_proc,(num_process*sizeof(struct Process)));
  //start at the check_deadlines time
  int index = check_deadlines(arr_proc,num_process);
  struct Process current_proc = arr_proc[index];
  for(int current_time=0;current_time<=max_time; current_time++){
    //checks to see if the cpu finished
    if(current_proc.cpu_time == 0){
      printf("%d: process %d ends\n",current_time,current_proc.id);
     
      
      
      total_waiting+=(current_time-current_proc.arrival)-(first_copy[current_proc.id-1].cpu_time);
      
      int len = num_process-1;
      struct Process *copy = (struct Process*) malloc(len*sizeof(struct Process));
      int j=0;
      for(int i =0; i<num_process;i++){
          if(i == index){
              continue;
          }
          copy[j]=arr_proc[i];
          j++;
      }
      num_process--;
      free(arr_proc);
      arr_proc=copy;
     
    }
    
    if (current_time!= max_time){
      missed_deadline(&arr_proc,num_process,current_time);
    }
    
    if (current_time == max_time){
      printf("%d: Max Time reached\n",current_time);
      
      for(int i =0; i<num_process; i++){
      
      total_waiting+=(current_time-arr_proc[i].arrival)-((first_copy[arr_proc[i].id-1].cpu_time)-( arr_proc[i].cpu_time));
      

      }
      printf("Sum of all waiting times: %d\n", total_waiting);
      printf("Number of processes created: %d\n", total_process);
      float awt=(float) total_waiting/total_process;
      printf("Average Waiting Time: %.2f\n", awt);

      if(num_process>=0){
        free(arr_proc);
      }
      free(first_copy);
      //free
      return;
    }
    total_process+=update_process(first_copy,&arr_proc,&num_process,num,current_time);
    if(current_proc.cpu_time<=0){
      if(num_process!=0){
        index=check_deadlines(arr_proc,num_process);
        current_proc=arr_proc[index];
        printf("%d: process %d starts\n",current_time,current_proc.id);
      }
    }
    //int preemptive(struct Process **arr_proc, int num_process, int index, int *ppid, int time){
    //part 3: preemptive
    // update_process(Process** arr_proc, int* num_process, int time, int num, Process* first_copy)
    //int preemptive(Process **arr_proc, int num_process; int current_index; int *p, int time)
     int ppid = index;
    int pp = preemptive(&arr_proc, num_process, index, &ppid, current_time);
    if(current_time == 0){
      printf("%d: process %d starts\n",current_time,current_proc.id);
    }
    if(pp){
      index = ppid;
      current_proc = arr_proc[index];
      printf("%d: process %d starts\n", current_time,current_proc.id);
    }
    
    current_proc.cpu_time--;
    int x = arr_proc[index].cpu_time;
    arr_proc[index].cpu_time= x-1;
    //free(arr_proc);
  }
}

int main() {
    int num_process;
    //ask the user for number of processes
    printf("Enter the number of processes to schedule: ");
    scanf("%d", &num_process);
    int period[num_process];
  //read the process information from the user for each process indicated
  //an array of processes, arr_proc to keep track
    //Process* arr_proc[num_process];
    struct Process* arr_proc = (struct Process*)malloc(num_process * sizeof(struct Process));
    for(int i=0; i < num_process; i++){
      printf("Enter the CPU time of process %d: ", i+1);
      scanf("%d", &arr_proc[i].cpu_time);
    
      printf("Enter the period of process %d: ", i+1);
      scanf("%d", &arr_proc[i].period);
    
      //arr_proc[i].remaining_time = arr_proc[i].cpu_time;
      arr_proc[i].deadline = arr_proc[i].period;
      arr_proc[i].id = i+1;
      arr_proc[i].arrival = 0;
      period[i] = arr_proc[i].period;
    }
     
    //calculate the max time representing the LCM
    int max_time;
    max_time = max_lcm(period, num_process);
    
    
    edf_schedule(arr_proc, num_process, max_time);
    
    
    return 0;
}
