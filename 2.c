#include<stdio.h>


void initialize(int graph[7][7], int cost[7][7], int next[7][7])
{
  int i,j;

  for(i=0;i<7;++i)
  {
      for(j=0;j<7;++j)
      {
         graph[i][j]=0;
      }
  }

  for(i=0;i<7;i++)
  {
     for(j=0;j<7;j++)
     {
        
        if(i==j)
        {
 	    cost[i][j]=0;
            next[i][j]=i; 
        }
        else
        {
            cost[i][j]=9;
        }
          
     }
  }

} 




void construct_graph(int graph[7][7], int cost[7][7],int next[7][7])
{
  int e,start,end,i;
  printf("\n Enter the number of edges ");
  scanf("%d",&e);

  for(i=0;i<e;++i)
  {
     printf("\n Enter end points of the link ");
     scanf("%d %d",&start,&end);
     graph[start][end]=graph[end][start]=1;
     cost[start][end]=cost[end][start]=1;
     next[start][end]=end;
     next[end][start]=start; 
  }

}

void share(int graph[7][7], int cost[7][7], int next[7][7])
{
   int i,j,k;
   for(i=0;i<7;i++)
   {
       for(j=0;j<7;j++)
       {
            if(graph[i][j]==1)
            {    
                 int m=0; 
                 for(m=0;m<7;m++)
                 {
                     int val1,val2;

                     val1=1+cost[i][m];
                     val2=cost[j][m];
              
                     if(val1 < val2)
                     {
                             printf("\n came here! ");
			     cost[j][m]=val1;
                             next[j][m]=i;
                     }
                }
             }
         }
     }
}

void print(int mat[7][7])
{
    int i,j;
    for(i=0;i<7;++i)
    {
       for(j=0;j<7;++j)
       {
            printf("%d   ",mat[i][j]);
       }
       printf("\n");
    }
}
 
   
int main()
{
   int graph[7][7];
   int next[7][7]; 
   int cost[7][7];

   initialize(graph,cost,next);
   printf("\n The cost matrix initially is as follows :\n");
   print(cost);

   construct_graph(graph,cost,next);
   printf("\n The constructed graph is as follows : \n");
   print(graph);

   printf("\n The cost matrix now is as follows :\n");
   print(cost);
   printf("\n");

   share(graph,cost,next);
   printf("\n The cost matrix constructed is as follows :\n");
   print(cost);
   printf("\n\n The next hop matrix is as follows : \n");
   print(next);
   printf("\n");
 
  return 0;
}
