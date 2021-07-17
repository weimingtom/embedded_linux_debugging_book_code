/*
  Name: DijkstraShortPath.c
  Copyright: All Copyrights reserved by rockins
  Author: rockins
  Date: 03-12-05 23:30
  Description: this file implement the dijkstra shortest path
  Note: There isn't any warrant for damage if you run it on your PC
  Detail:
      adjacent matrix:	6x6
  Test Result:
      	1 -> 1 = INF(no path)
        1 -> 2 = INF(no path)
        1 -> 3 = 10(1,3,)
        1 -> 4 = 50(1,5,4,)
        1 -> 5 = 30(1,5,)
        1 -> 6 = 60(1,5,4,6,)
  BUG report: ybc2084@163.com
  		or www.dormforce.net/blog/rockins
  I'm sorry for concrete comment and explornation
*/

#include <stdio.h>
#include <stdlib.h>

#define 	N		6
#define 	INF		16384
#define		TRUE	1
#define		FALSE	0

typedef int		WeightArrayType[N+1];
typedef int 	AdjMatrixType[N+1][N+1];
typedef int 	SetElemType;
typedef struct 	SetType{
    SetElemType		nodes[N+1];
    int				last;
}Set, *SetPtr;

int				PathArray[N+1][N+1];						// Path Matrix

const int 		AdjMatrix[N+1][N+1] = {						// Adjacent Matrix
    				{INF, INF, INF, INF, INF, INF, INF},
					{INF, INF, INF,  10, INF,  30, 100},
					{INF, INF, INF,   5, INF, INF, INF},
					{INF, INF, INF, INF,  50, INF, INF},
					{INF, INF, INF, INF, INF, INF,  10},
					{INF, INF, INF, INF,  20, INF,  60},
					{INF, INF, INF, INF, INF, INF, INF}};
					
/* prototype declare begin */
int InitSet(SetPtr s);
int InitPath(void);
int AddPath(int p, int n);
int CopyPath(int dst, int src);
int AddElemToSet(SetPtr s, SetElemType e);
int MergeSet(SetPtr s, SetPtr t);
int IsElemInSet(SetPtr s, SetElemType e);
int PrintPath(SetElemType v, WeightArrayType w);
int DijkstraShortPath(const AdjMatrixType matrix, SetElemType vertex, 
						WeightArrayType weight);

/*
Function:
    init S set
*/
int InitSet(SetPtr s)
{
    s->last = 0;
}    

/*
Funtion:
    init path matrix
*/
int InitPath(void)
{
    int i, j;
    for (i = 0; i <= N; i++){
        for (j = 0; j<= N; j++){
        	PathArray[i][j] = 0;
       	}   	
    }    
}

/*
Function:
    add node into p path
*/
int AddPath(int p, int n)
{
    int k;
    PathArray[p][0] += 1;
    k = PathArray[p][0];
    PathArray[p][k] = n;
}    

/*
Function:
    copy src's path into dst's path
*/
int CopyPath(int dst, int src)
{
    int i;
    for (i = 0; i <= N; i++){
        PathArray[dst][i] = PathArray[src][i];
    }
}        

/*
Function:
    add e into s as an element
*/
int AddElemToSet(SetPtr s, SetElemType e)
{
    if ((0 <= s->last) && (s->last < N)){
        s->last += 1;
        s->nodes[s->last] = e;
        return(TRUE);
    }
    return(FALSE);    
}        

/*
Funciton:
    test if e is an element of s
*/
int IsElemInSet(SetPtr s, SetElemType e)
{
    int i;
    for (i = 1; i <= s->last; i++){
        if (s->nodes[i] == e)	return(TRUE);
    }
    return(FALSE);    
}    

/*
Function:
    the main algorithm which is proposed by dijkstra
*/
int DijkstraShortPath(const AdjMatrixType matrix, SetElemType vertex, 
						WeightArrayType weight)
{
    int i, j, k;
    Set	s;
    SetElemType v = vertex;
    
    for(i = 1; i <= N; i++){
        weight[i] = matrix[v][i];
        if (weight[i] < INF){
            AddPath(i, v);
            AddPath(i, i);					// init path[i]
        }else{
            // do nothing
        }
    }
    InitSet(&s);
    AddElemToSet(&s, v);
    for(i = 1; i <= N-1; i++){
        int min_cost = INF;
        for (j = 1; j <= N; j++){
            if ((!IsElemInSet(&s, j)) && (weight[j] < min_cost)){
                k = j;
                min_cost = weight[j];
            }
        }
        AddElemToSet(&s, k);
        for (j = 1; j<= N; j++){
            if ((!IsElemInSet(&s, j)) && (weight[k] + matrix[k][j] <weight[j])){
                weight[j] = weight[k] + matrix[k][j];
                CopyPath(j, k);
                AddPath(j, j);
            }
        }
    }                   
}

/*
Function:
    print the ultimate result path
*/
int PrintPath(SetElemType v, WeightArrayType w)
{
    int i, j, k;
    for (i = 1; i <= N; i++){
        if (w[i] < INF){
        	printf("%d -> %d = %d(", v, i, w[i]);
        	for (j = 1; j <= PathArray[i][0]; j++){
        	    printf("%d,", PathArray[i][j]);
        	}
        	printf(")\n");
       	}        
       	else
       		printf("%d -> %d = %s\n", v, i, "INF(no path)");
    }  		
}
    
int main(void)
{
    SetElemType vertex = 1;
    WeightArrayType weight;
    
    InitPath();										// init path matrix
    DijkstraShortPath(AdjMatrix, vertex, weight);
    PrintPath(vertex, weight);
//    system("pause");
    return(0);
}    

