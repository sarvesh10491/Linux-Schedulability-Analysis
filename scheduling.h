//################################################################################################
//
// Program      : Real-time task scheduling algorithm analysis in Linux
// Library file : scheduling.c
// Authors      : Sarvesh Patil & Vishwakumar Doshi
// Date         : 3 March 2018
//
//################################################################################################


#define _GNU_SOURCE

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>

#define M 3.0

#define MAX_TASKS 25          //The max no of tasks that can be there in a taskset
#define MAX_TASKSETS 5000     //The max no of tasksets that are possible  
#define MAX_FILESIZE 1000     //The max memory available


//################################################
// Global variables & Functions declaration area
//################################################

float*** task_m;
int* num_rows;
int num_tasks;
int SUCCESS=0; 

// Link list
struct Node
{
    float t,h;
    struct Node* next;
    struct Node* prev;
};

// Structure for randoms task data
struct Task_Body
{
  int tnum;     
  float deadline;
  int period;
  float WCET;   
  float utilization;  
};

struct Task_set_struct
{
  int total_tasks;
  struct Task_Body task[MAX_TASKS];
};

struct Task_set_struct task_set[MAX_TASKSETS];

FILE *file;

void UUniFast(int,float,float[]);
void printList(struct Node* a);
void file_ops();
int rt_analysis(int a,int n,int c,int *index,char h);
void sortArray(float *arr,int size);
int findIndex(float* arr,int size,float value);
int rm_func(int a,int l,int* index,char h);
int rm_test();
int dm_test();
void updateList(struct Node* head);
int ld_fact(int a);
int edf_test(void);


//##############################
// Task generating functions
//##############################

// Function to read from input file (Part 1)
//==========================================
void file_ops(void)
{
    char buffer[1024] ;   
    char *record,*line;   
    
    
    // File operations
    //=================
    FILE *fstream = fopen("new_input.txt","r");   // opening input file
    if(fstream == NULL)   
    {      
       printf("\nFile opening failed.\n");      
       return ;   
    }

    line = fgets(buffer,sizeof(buffer),fstream);   // Reading line and storing string in line variable
    record = strtok(line," ");                     // Separating tokens based on space delimeter

        
    num_tasks = atoi(record);  

    task_m = (float***)malloc(num_tasks * sizeof(float**)); 
    num_rows = (int*)malloc(num_tasks * sizeof(int));                  
    
    printf("Number of tasksets : %d\n", num_tasks);
    
    for(int i=0;i<num_tasks;i++)
    { 
        line=fgets(buffer,sizeof(buffer),fstream);
        record = strtok(line," "); 
        num_rows[i] = atoi(record);
        
        task_m[i] = (float**)malloc(num_rows[i] * sizeof(float*));

        for(int j=0;j<num_rows[i];j++)
        {
          task_m[i][j] = (float*)malloc(3 * sizeof(float));

          line=fgets(buffer,sizeof(buffer),fstream);
          record = strtok(line," "); 
          task_m[i][j][0]= atof(record);

          for(int k=1;k<3;k++)
          {
            record = strtok(NULL," ");     
            task_m[i][j][k] = atof(record);
          }
          
        }
    }

    for(int i =0;i<num_tasks;i++)
    {
      for(int j = 0;j<num_rows[i];j++)
      {
        for(int k = 0;k<3;k++)
          printf("%1.1f\t",task_m[i][j][k]);
        printf("\n");
      }

      printf("\n\n\n\n");

    }
}



// Function to generate synthetic tasksets (Part 2)
//=================================================
void Taskset_generator(int MAX_TASK,int DCT)
{

  char *FILE_NAME=malloc(sizeof(char)*MAX_FILESIZE); // File used to collect output

  srand(time(NULL));
  int i;
  struct Task_set_struct t;
  int taskset_cur=0;
  int STATUS=0;

  float U_BAR=0.05;
  float utilization[MAX_TASK];
  int EDF_SUCCESSFUL=0;
  int RM_SUCCESSFUL=0;
  int DM_SUCCESSFUL=0;
  float Deadline_bound_U;
  float Deadline_bound_L;
  
  sprintf(FILE_NAME,"%s_%d_%d","Random_TS",MAX_TASK,DCT);
  file=fopen(FILE_NAME,"w");
  if(file==NULL)
  {
      printf("Error opening file...Exiting\n");
      return;
  }

  fprintf(file,"U\t\tEDF\t\tRM\t\tDM\n");

  if(DCT)
    printf("RANDOM ANALYSIS FOR %d TASKS PER TASKSET and deadlines between [C+(T-C)/2,T]\n",MAX_TASK);
  else
    printf("RANDOM ANALYSIS FOR %d TASKS PER TASKSET and deadlines between [C,T]\n",MAX_TASK);

  while(U_BAR<=1)
  {
    printf("####################################################\n");
    printf("Generating and Testing for U_BAR=%f\n",U_BAR);

    while(taskset_cur++<MAX_TASKSETS)
    {
      //printf("Generating Random Task Sets\n");
      t.total_tasks=MAX_TASK;

      for(i=0;i<t.total_tasks;i++)
      {
        t.task[i].tnum=i;     

        if(i<=t.total_tasks/M)
          t.task[i].period=rand()/(RAND_MAX*1.0)*(100-10)+10;
        else if(i>t.total_tasks/M && i<(2.0/M)*t.total_tasks)
          t.task[i].period=rand()/(RAND_MAX*1.0)*(1000-100)+100;
        else
          t.task[i].period=rand()/(RAND_MAX*1.0)*(10000-1000)+1000;

      }

      UUniFast(t.total_tasks,U_BAR,utilization);

      for(i=0;i<t.total_tasks;i++)
      {
        t.task[i].utilization=utilization[i];
        t.task[i].WCET=utilization[i]*t.task[i].period;
        
        Deadline_bound_U=t.task[i].period*1.0;
        Deadline_bound_L=t.task[i].WCET+(DCT*((t.task[i].period-t.task[i].WCET)/2.0));
        t.task[i].deadline=rand()/(RAND_MAX*1.0)*(Deadline_bound_U-Deadline_bound_L)+Deadline_bound_L;
      }

      num_tasks = 1;  

      task_m = (float***)malloc(num_tasks * sizeof(float**)); 
      num_rows = (int*)malloc(num_tasks * sizeof(int));


      printf("\n************************* TASK SET ************************\n");
      printf("Total tasks : %d\n",t.total_tasks);
      for(int i=0;i<num_tasks;i++)
      { 
        num_rows[i] = t.total_tasks;
        
        task_m[i] = (float**)malloc(num_rows[i] * sizeof(float*));

        for(int j=0;j<num_rows[i];j++)
        {
          task_m[i][j] = (float*)malloc(3 * sizeof(float));
  
          task_m[i][j][0] = t.task[j].WCET;
          task_m[i][j][1] = t.task[j].deadline;
          task_m[i][j][2] = t.task[j].period;
          
        }

        for(int i =0;i<num_tasks;i++)
        {
            for(int j = 0;j<num_rows[i];j++)
            {
              for(int k = 0;k<3;k++)
                printf("%1.1f\t",task_m[i][j][k]);
              printf("\n");
            }

            printf("\n\n");
        }
      }
        printf("\n###############################\n\n");
        printf("EDF Analysis\n================\n");
        STATUS=edf_test();
        if(STATUS==1)
          EDF_SUCCESSFUL+=1;
        STATUS=0;
          
        printf("\n###############################\n\n");
        printf("RM Analysis\n================\n");
        STATUS=rm_test();
        if(STATUS==1)
          RM_SUCCESSFUL+=1;
        STATUS=0;
        
        printf("\n###############################\n\n");
        printf("DM Analysis\n================\n");
        STATUS=dm_test();
        if(STATUS==1)
          DM_SUCCESSFUL+=1;
        STATUS=0;
    }

    fprintf(file,"%f\t%f\t%f\t%f\n",U_BAR,(1.0*EDF_SUCCESSFUL)/MAX_TASKSETS,(1.0*RM_SUCCESSFUL)/MAX_TASKSETS,(1.0*DM_SUCCESSFUL)/MAX_TASKSETS);

    EDF_SUCCESSFUL=0;
    RM_SUCCESSFUL=0;
    DM_SUCCESSFUL=0;
    taskset_cur=0;
    U_BAR+=0.1;
  }

fclose(file);

}

// UUnifast algorithm implementaion function
//===========================================
void UUniFast(int total_tasks,float u_bar,float utilization[])
{
  int i;

  float sumU=u_bar;
  float nextSumU;

  for(i=0;i<total_tasks-1;i++)
  {
    nextSumU=sumU*pow((rand()/(RAND_MAX*1.0)),(1.0/(total_tasks-i)));
    utilization[i]=sumU-nextSumU;
    sumU=nextSumU;
  } 

  utilization[total_tasks-1]=sumU;
}


// Function to print nodes in a given linked list
//================================================
int lf_List(struct Node *node)
{
  struct Node* temp;
  float lf;
  int s_flag = 0;
  while(node!=NULL)
  {
     lf = node->h/node->t;
     if(lf>1)
      s_flag = 1;
     temp = node;
     //printf("t is %1.2f\t\th is %1.2f\t\tloading factor is %1.2f\n", node->t,node->h,lf);
     node = node->next;
     free(temp);
  }

  if(s_flag == 1)
  {
    printf("Loading factor isn't bounded at all deadlines, hence this task set not schedulable!\n");
    return 0;
  }
  else
  {
    printf("Loading factor is bounded at all deadlines, hence this task set is schedulable!\n");
    return 1;
  }
}
 
// Function to insert a node at the beginging of the linked list 
//===============================================================
void push(struct Node** node, float new_t,float diff)
{
  struct Node* temp = *node;
  struct Node* updt = NULL;
  struct Node* p;
  int rtn_flag=0;

  while(temp!=NULL)
  {
    if(temp->t == new_t)
      {
        //printf("found equal %f\n",new_t);
        updt = temp;
        while(updt!=NULL)
        {
          updt->h+=diff;
          //printf("adding data %f to %f\n",diff ,updt->t);
          updt = updt->next;
        }
        rtn_flag = 1;
        break;
      }

    if((temp == *node) && (new_t < temp->t))
    {
      /* allocate node */
        struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
        
        /* put in the data  */
        new_node->t  = new_t;
        new_node->h  = diff;
        new_node->next = temp;
        updt = temp;
        while(updt!=NULL)
        {
          updt->h+=diff;
          //printf("adding data %f to %f\n",diff ,updt->t);
          updt = updt->next;
        }
        *node = new_node;
        //printf("%f inserted before %f because of inversion\n",new_node->t,temp->t);
        rtn_flag=1;
        break;
    }

    if((temp->next!=NULL) && (new_t > temp->t) && (new_t < temp->next->t))
      {
        /* allocate node */
        struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
        
        /* put in the data  */
        //printf("making new node %f insert between %f and %f\n",new_t,temp->t,temp->next->t);
        new_node->t  = new_t;
        new_node->h  = temp->h +diff;
        new_node->next = temp->next;
        temp->next=new_node;
        updt=new_node->next;
        while(updt!=NULL)
        {
          updt->h+=diff;
          //printf("adding data %f to %f\n",diff ,updt->t);
          updt=updt->next;
        }
        rtn_flag=1;
        break;
      }
      temp = temp->next;
  }

  if(rtn_flag == 1)
    return;


//printf("Equal failed\n");

  /* allocate node */
  struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
  
  /* put in the data  */
  //printf("making new node to add\n");
  new_node->t  = new_t;

  /* link the old list off the new node */
  new_node->next = NULL;
  if((*node) == NULL)
    {
      (*node) = new_node;
      new_node->h = diff;
    }
  else
  {
    p = (*node);
    while(p->next!= NULL)
      p = p->next;
    p->next = new_node;
    new_node->h = p->h + diff;
  }

} 

//*******************************************************************************************

//######################################
// Scheduling tests function area
//######################################

//+++++++++++++++++
// EDF scheduling
//+++++++++++++++++

int edf_test(void)
{
  float sum_u=0;
  int dlp_reln;
  int EDF_flag=0;

  for(int i =0;i<num_tasks;i++)
    {
      dlp_reln=0;
      
      for(int j = 0;j<num_rows[i];j++)
      {
        sum_u += task_m[i][j][0]/task_m[i][j][1];
        if(task_m[i][j][1] < task_m[i][j][2])
          dlp_reln=1;
      }

      printf("Utilisation is %1.3f\n",sum_u);

      if(sum_u <= 1.0)
      {
        printf("Task set %d is EDF schedulable\n\n",i);
        EDF_flag++;
      }

      else if(dlp_reln == 1)
        {
          printf("Task set %d has Utilisation more than 1 and dl<pd so loading factor analysis\n\n",i);
          if(ld_fact(i))
            EDF_flag++;
        }

      else
        printf("Task set %d is not schedulable\n\n",i);
          
      sum_u = 0;

    }

  return EDF_flag;
}

// Function to calculate Loading factors
//=======================================
int ld_fact(int a)
{
  int c=0,exit_flag;
  double prev_l=0,curr_l=0,t=0;
  
  
  struct Node* list = NULL;

  for(int i=0;i<num_rows[a];i++)
    prev_l+= task_m[a][i][0];

  while(1)
  { 
    curr_l = 0;
    for(int j =0;j<num_rows[a];j++)
    {
      curr_l += ceil(prev_l/task_m[a][j][2])*task_m[a][j][0];
    }
    //printf("current L : %1.2f\n", curr_l);

    if(prev_l == curr_l)
      break;
    else
      prev_l = curr_l;

  }
  
  
  while(1)
  {
      exit_flag = 0;
      for(int i = 0;i<num_rows[a];i++)
      { 
        t = task_m[a][i][1] + c*task_m[a][i][2];
        if(t > curr_l)
          continue;
        else
          {
            exit_flag = 1;
            push(&list,t,task_m[a][i][0]);  
          }
      }
    if(exit_flag==0)
      break;
    c++;
  }

  return lf_List(list);
}


//+++++++++++++++++
// DM scheduling
//+++++++++++++++++

int dm_test(void)
{
  //float sum_u=0;
  float* usrt,*srt;
  int* d_index;
  int DM_flag=0, DM_fail_flag=0;

  
  for(int i =0;i<num_tasks;i++)
  {  
    DM_fail_flag=0;

    srt = (float*)malloc(num_rows[i]* sizeof(float));
    usrt = (float*)malloc(num_rows[i]* sizeof(float));
    d_index = (int*)malloc(num_rows[i]* sizeof(int));

    
    for(int p=0;p<num_rows[i];p++)
    {
      usrt[p] = task_m[i][p][1];
      srt[p] = task_m[i][p][1];
    }

    sortArray(srt,num_rows[i]);

    for(int p = 0;p<num_rows[i];p++)
      {
        d_index[p] = findIndex(usrt,num_rows[i],srt[p]);
      }

    for(int j=0;j<num_rows[i];j++)
    {
      if(rm_func(i,j,d_index,'D'))
      {
        DM_fail_flag=1;
        break;
      }
    }

    if(DM_fail_flag == 0)
      DM_flag++;

    free(srt);
    free(usrt);
    free(d_index);
  }

return DM_flag;
}



//+++++++++++++++++
// RM scheduling
//+++++++++++++++++

int rm_test(void)
{
  float ef_ut=0,Hn=0;
  int dlp_reln,dp_order,p,ordered_tasks;
  float* usrt,*srt;
  int* p_index,*d_index;
  int exit_flag=0;
  int RM_flag=0, RM_fail_flag=0;

  
  for(int i =0;i< num_tasks;i++)
  {  
    exit_flag=0;
    RM_fail_flag=0;

    srt = (float*)malloc(num_rows[i]* sizeof(float));
    usrt = (float*)malloc(num_rows[i]* sizeof(float));
    p_index = (int*)malloc(num_rows[i]* sizeof(int));
    d_index = (int*)malloc(num_rows[i]* sizeof(int));

    for(int p=0;p<num_rows[i];p++)
    {
      usrt[p] = task_m[i][p][2];
      srt[p] = task_m[i][p][2];
    }

    sortArray(srt,num_rows[i]);

    for(int p = 0;p<num_rows[i];p++)
      {
        p_index[p] = findIndex(usrt,num_rows[i],srt[p]);
        //printf("P : %d\t",p_index[p]);
      }

      printf("\n");

    for(int p=0;p<num_rows[i];p++)
    {
      usrt[p] = task_m[i][p][1];
      srt[p] = task_m[i][p][1];
    }

    sortArray(srt,num_rows[i]);

    for(int p = 0;p<num_rows[i];p++)
      {
        d_index[p] = findIndex(usrt,num_rows[i],srt[p]);
        //printf("D : %d\t",d_index[p]);
      }
    printf("\n");

    free(srt);
    free(usrt);

    p=0;
    dp_order=1;
    for(ordered_tasks = 0;ordered_tasks<num_rows[i];ordered_tasks++)
    {
      while(p_index[ordered_tasks]!=d_index[p])
      {
        //printf("p index is %d and old d index is %d\n",p_index[ordered_tasks],d_index[p]);
        p++;
              
        if(p==num_rows[i])
        {
          //printf("\n\nCouldn't find equal\n");
          dp_order=0;
          break;
        }
        //printf("p index is %d and new d index is %d\n\n",p_index[ordered_tasks],d_index[p]);
              
      }
      if(dp_order==0)
        {
          break;
        }

      //printf("Found equal\n\n");
    }

    //printf("Number of ordered tasks is %d\n",ordered_tasks);

    for(int j = 0;j<num_rows[i];j++)
    {
      dlp_reln=0;
      
        for(int k =0 ;k<=j;k++)
        {
          if(task_m[i][p_index[k]][1] < task_m[i][p_index[k]][2])
            dlp_reln=1;
        }

        
        if((dlp_reln==0)||((dlp_reln==1)&&(j<ordered_tasks)))
        { 
          if(rm_func(i,j,p_index,'R'))
          {
            RM_fail_flag=1;
            break;
          }
        }
      
        else
          {
            for(int l=0;l<=j;l++)
            {
              Hn=0;
              ef_ut=0;
              for(int k =0;k<=j;k++)
              {
                if(task_m[i][p_index[k]][2] < task_m[i][p_index[l]][1])
                {
                  ef_ut+= (task_m[i][p_index[k]][0]/task_m[i][p_index[k]][2]);
                  Hn++;
                  //printf("If condition : %f\n", ef_ut);
                }
                else //if(task_m[i][p_index[k]][1]<=task_m[i][p_index[l]][1])
                {
                  ef_ut+= (task_m[i][p_index[k]][0]/task_m[i][p_index[l]][1]);
                  //printf("Else condition : %f\n",ef_ut );
                }
              }
              //printf("Eff UT : %f and checking against bound %f\n",ef_ut,(Hn+1)*(pow(2.0,(1.0/(Hn+1)))-1));
              if(ef_ut <= (Hn+1)*(pow(2.0,(1.0/(Hn+1)))-1))
                 printf("Task %d of taskset %d is schedulable by effective Utilisation for %d tasks\n",l+1,i,j+1);
              
              else
              {
                 printf("Task %d of taskset %d is not schedulable for %d tasks\n",l+1,i,j+1);
                if(rt_analysis(i,j,l,p_index,'r'))
                {
                   exit_flag=1;
                   RM_fail_flag=1;
                   break;
                }
              }
            }
            if(exit_flag==1)
              break;
          }
          
    }
      if(RM_fail_flag==0)
        RM_flag++;

      free(d_index);
      free(p_index);   
  }  
  return RM_flag;     
}

int rm_func(int a,int l,int* index,char alg)
{
 float sum_u=0;
 for(int k = 0;k<=l;k++)
  sum_u += task_m[a][index[k]][0]/task_m[a][index[k]][1];

  printf("\n%cM Utilisation for %d tasks in taskset %d is %1.3f\n",alg,l+1,a,sum_u);
          

  if(sum_u > 1)
  {
    printf("Taskset %d is not %cM schedulable for %d tasks cz exceeds 1!!\n",a,alg,l+1);
    return 1;
  }

  else if(sum_u <= (l+1)*(pow(2.0,(1.0/(l+1)))-1))
  {
    printf("Task set %d is %cM schedulable for %d tasks\n\n\n",a,alg,l+1);
    return 0;
  }

  else 
  {
    printf("Task set %d is not %cM schedulable for %d tasks by Utilisation test.So response time analysis\n\n\n",a,alg,l+1);
    return rt_analysis(a,l,l,index,alg);    
  }
}


// Response Time Analysis
//========================

int rt_analysis(int a,int n,int c,int *index,char alg)
{
  float new_w=0,prev_w=0;

    for(int k=0;k<=n;k++)
      prev_w+=task_m[a][index[k]][0];

    //printf("prev is %f\n",prev_w );

    while(new_w <= task_m[a][index[c]][1])
    {
      
      new_w = task_m[a][index[c]][0];
      //printf("initial new is %f\n", new_w);
      if(alg == 'R')
      {
        //printf("%cM test\n",alg );
        for(int j=0;j<n;j++)
        {
          new_w += ceil(prev_w/task_m[a][index[j]][2]) * task_m[a][index[j]][0];
          //printf("Den %f\t\tMul: %f\n",task_m[a][index[j]][2],task_m[a][index[j]][0] );
        }
      }

      else if(alg=='D')
      {
        for(int j=0;j<n;j++)
        {
          if(task_m[a][index[j]][1] < task_m[a][index[c]][1])
            new_w+=ceil(prev_w/task_m[a][index[j]][1]) * task_m[a][index[j]][0];
          else 
            new_w+=task_m[a][index[j]][0];
        }
      }
      else if(alg == 'r')
      {
        //printf("effective failure RT test\n");
          
        for(int j=0;j<n;j++)
        {
          if(task_m[a][index[j]][2] < task_m[a][index[c]][1])
            new_w+=ceil(prev_w/task_m[a][index[j]][2]) * task_m[a][index[j]][0];
          else if(task_m[a][index[j]][1] <= task_m[a][index[c]][1])
            new_w+=task_m[a][index[j]][0];
        }
      }
 //printf("new %f\n",new_w );
      if(new_w == prev_w)
      {
        printf("Task %d of taskset %d is schedulable and lower priority are not\n\n",n+1,a);
        return 0;
      }

      else
        prev_w = new_w;
     
    }

    printf("Task %d of taskset %d is not schedulable by RT analysis\n\n",n+1,a);
    return 1;

}

//########################
// Utility functions
//########################

// Function to sort sort array
//============================
void sortArray(float *arr,int size)
{
  float tmp;

  for(int i=0;i<size;i++)
    for(int j=i+1;j<size;j++)
      if(arr[i]>arr[j])
      {
        tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
      }

}

int findIndex(float* arr,int size,float value)
{
    int index = 0;

    while ( index < size && arr[index] != value ) ++index;

    return index;
}


//######################################
//
// End of scheduling.h
//
//######################################