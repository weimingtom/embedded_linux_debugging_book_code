/****************************************************************
This programme is created by Rockins
It is freely distributed under GPL license
You can modify and redistribute it with conbide by GNU license
CopyRight(r) 2005.
All CopyRight are reserved by Rockins
*******************************************************************/

/******************************************************************
说明：
（这是一个兄弟在BLOG上给我留的一道题，我花了三个小时，惭愧）
动态规划算法是一类很重要的算法，虽然和递归很相似，但是在复杂度上却
大大小于递归程序。如果不用递归而用迭代来实现，则计算效率会更高。本程序
因为写得比较仓促，因此只给出了递归方式的实现。 
动态规划有两种推解方式：逆向推解和正向推解 ，通常情况下使用的都是逆向推解
而本程序使用了正向推解的方式。
同时，本程序参考了《中国工程科学》（2005年2月第7卷第2期）上的一篇论文,
该论文的题目是：《动态规划的正向推解方法》，在此谨向三位作者致以衷心的感谢。
为了方便参考，我将该论文也在附件中一并附上。
本程序在WIN2000上使用DEV-C++进行编译，如果要用VC++编译的话，可能需要做适当的修改。
DEV-C++可以从www.sourceforge.net 得到.
程序的运行结果如下所示：
 
max sum: 30
depth: 5
path: 7->3->8->7->5
请按任意键继续 . . .

*******************************************************************/ 

/********************************************************************
问题描述: 
动态规划举例 
10．2节给出了动态规划的逆推解法，下面我们给出另一种解法—从始点向终点方向寻找最佳路线的顺推解法，若以Uk表示第k阶段的一个决策点，从始点开始，依顺向求出第一阶段，第二阶段，……，第n阶段中各点到达始点的最佳路线，最终求出始点到终点的最佳路线。 
图10-3（a）示出了一个数字三角形，请编一个程序，计算从顶至底的某处的一条路劲，使该路劲所经过的数字的总和最大。 
（1） 每一步可沿左斜线向下或右斜线向下； 
（2） 1＜三角形行数≤100； 
（3） 三角形中的数字为0，1，……99。 
输入数据： 
由INPUT.TXT文件中首先读到的是三角形的行数，在例子中INPUT.TXT表示如图13-3（b）. 

    7 
   3 8 
  8 1 0 
 2 7 4 4 
4 5 2 6 5 

图10-3（a） 
请各位帮忙用C语言的方法做一下拜托了!!
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
