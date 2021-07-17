/****************************************************************
This programme is created by Rockins
It is freely distributed under GPL license
You can modify and redistribute it with conbide by GNU license
CopyRight(r) 2005.
All CopyRight are reserved by Rockins
*******************************************************************/

/******************************************************************
˵����
������һ���ֵ���BLOG�ϸ�������һ���⣬�һ�������Сʱ��������
��̬�滮�㷨��һ�����Ҫ���㷨����Ȼ�͵ݹ�����ƣ������ڸ��Ӷ���ȴ
���С�ڵݹ����������õݹ���õ�����ʵ�֣������Ч�ʻ���ߡ�������
��Ϊд�ñȽϲִ٣����ֻ�����˵ݹ鷽ʽ��ʵ�֡� 
��̬�滮�������ƽⷽʽ�������ƽ�������ƽ� ��ͨ�������ʹ�õĶ��������ƽ�
��������ʹ���������ƽ�ķ�ʽ��
ͬʱ��������ο��ˡ��й����̿�ѧ����2005��2�µ�7���2�ڣ��ϵ�һƪ����,
�����ĵ���Ŀ�ǣ�����̬�滮�������ƽⷽ�������ڴ˽�����λ�����������ĵĸ�л��
Ϊ�˷���ο����ҽ�������Ҳ�ڸ�����һ�����ϡ�
��������WIN2000��ʹ��DEV-C++���б��룬���Ҫ��VC++����Ļ���������Ҫ���ʵ����޸ġ�
DEV-C++���Դ�www.sourceforge.net �õ�.
��������н��������ʾ��
 
max sum: 30
depth: 5
path: 7->3->8->7->5
�밴��������� . . .

*******************************************************************/ 

/********************************************************************
��������: 
��̬�滮���� 
10��2�ڸ����˶�̬�滮�����ƽⷨ���������Ǹ�����һ�ֽⷨ����ʼ�����յ㷽��Ѱ�����·�ߵ�˳�ƽⷨ������Uk��ʾ��k�׶ε�һ�����ߵ㣬��ʼ�㿪ʼ����˳�������һ�׶Σ��ڶ��׶Σ���������n�׶��и��㵽��ʼ������·�ߣ��������ʼ�㵽�յ�����·�ߡ� 
ͼ10-3��a��ʾ����һ�����������Σ����һ�����򣬼���Ӷ����׵�ĳ����һ��·����ʹ��·�������������ֵ��ܺ���� 
��1�� ÿһ��������б�����»���б�����£� 
��2�� 1��������������100�� 
��3�� �������е�����Ϊ0��1������99�� 
�������ݣ� 
��INPUT.TXT�ļ������ȶ������������ε���������������INPUT.TXT��ʾ��ͼ13-3��b��. 

    7 
   3 8 
  8 1 0 
 2 7 4 4 
4 5 2 6 5 

ͼ10-3��a�� 
���λ��æ��C���Եķ�����һ�°�����!!
********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#define ROW 5
#define COL 9

typedef struct path					//path structure
{
    #define MAX_NODES 100			//MAX PATH DEPTH
    int path_arr[MAX_NODES];     	//nodes chain
    int path_len;					//node chain's length
    int acc;                		//accumulate
}PATH;

PATH path[ROW][COL];				//global path stucture

int array[ROW][COL] ={{-1,-1,-1,-1, 7,-1,-1,-1,-1},		//global array of the primitive triangle array
                      {-1,-1,-1, 3,-1, 8,-1,-1,-1},
                      {-1,-1, 8,-1, 1,-1, 0,-1,-1},
                      {-1, 2,-1, 7,-1, 4,-1, 4,-1},
                      { 4,-1, 5,-1, 2,-1, 6,-1, 5}};
                      
static int recur(int row,int col);

int
main(void)
{    
    PATH *ptr;
    PATH node;
    int i,j;
    
    for(i=0;i<ROW;i++)						//initialize path structure
    	for(j=0;j<COL;j++)
    		{
    		    path[i][j].acc = -1;
    		    path[i][j].path_len = 0;	//donot need to initialize path_arr
    		}
    
    for(i=0;i<ROW;i++)                      	//scan row
    {
        for(j=0;j<COL;j++)                  	//scan col
        {
            if(array[i][j]<0)				//aussume array[i][j] won't less than 0
            	continue;					//so ignore this condition
           	if(i==0)						//initialize the first node
           	{
           		ptr = &path[i][j];			//ptr point to path[i][j]
           		ptr->path_arr[0] = array[i][j];
           		ptr->path_len = 1;
           		ptr->acc = array[i][j];
      		}
      		recur(i,j);						//involve recurtion,maybe this can be replaced by iteration method to save calculation
        }    
    }
    
    i = ROW - 1;							//last row
    node = path[i][0];						
    for(j=1;j<COL;j++)						//select the max accumulation node
    {
        if(node.acc<path[i][j].acc)
        	node = path[i][j];
    }
    
    printf("max sum: %d\n",node.acc);
    printf("depth: %d\n",node.path_len);
    printf("path: ");
    for(i=0;i<node.path_len;i++)
    {
        printf("%d",node.path_arr[i]);
        if(i != (node.path_len - 1))
        	printf("->");
     }
     printf("\n");         
    
    system("pause");
    return(0);
}

static int
recur(int row,int col)
{
    int i=row;
    int j=col;
    int len;			//temp variable,length of the path_arr
    int t;				//temp counter
    
    if(j == 0)
    {
        path[i][j].acc = path[i-1][j+1].acc + array[i][j];
        for(t=0;t<path[i-1][j+1].path_len;t++)							//copy node list to path[i][j]
        	path[i][j].path_arr[t] = path[i-1][j+1].path_arr[t];
        len = path[i-1][j+1].path_len + 1;
        path[i][j].path_len = len;
        path[i][j].path_arr[len-1] = array[i][j];
    	return 0;
   	}   	
   	if(j == 8)
   	{
   	    path[i][j].acc = path[i-1][j-1].acc + array[i][j];
        for(t=0;t<path[i-1][j-1].path_len;t++)							//copy node list to path[i][j]
        	path[i][j].path_arr[t] = path[i-1][j-1].path_arr[t];
        len = path[i-1][j-1].path_len + 1;
        path[i][j].path_len = len;
        path[i][j].path_arr[len-1] = array[i][j];
   		return 0;
    }      		
    if(path[i-1][j-1].acc > path[i-1][j+1].acc)
    {
        path[i][j].acc = path[i-1][j-1].acc + array[i][j];
        for(t=0;t<path[i-1][j-1].path_len;t++)
        	path[i][j].path_arr[t] = path[i-1][j-1].path_arr[t];
        len = path[i-1][j-1].path_len + 1;
        path[i][j].path_len = len;
        path[i][j].path_arr[len-1] = array[i][j];
    }    
    if(path[i-1][j-1].acc < path[i-1][j+1].acc)
    {
        path[i][j].acc = path[i-1][j+1].acc + array[i][j];
        for(t=0;t<path[i-1][j+1].path_len;t++)
        	path[i][j].path_arr[t] = path[i-1][j+1].path_arr[t];
        len = path[i-1][j+1].path_len + 1;
        path[i][j].path_len = len;
        path[i][j].path_arr[len-1] = array[i][j];
    }
}    
