/*
  Name: HuffmanTree.c
  Copyright: All copyrights reserved by rockins
  Author: rockins
  Date: 03-12-05 15:30
  Description: this programme implements huffman codeing algorithm just for perceptual consolidation
  Note: there's no warrant for any damage if you run it on your computer!!!
  Detail:
      
      N		leaf node numbers
      M		huffman tree node numbers
      message:		{'A', 'B', 'C', 'D'}
      pobability:	{0.1, 0.2, 0.3, 0.4}
      codeing result:
          'A':110
          'B':111
          'C':10
          'D':0

  Programme output:
      	#1 : token=A,code=110
        #2 : token=B,code=111
        #3 : token=C,code=10
        #4 : token=D,code=0
        
  BUG reports: please report any bug to: ybc2084@163.com
  		or report it on my BLOG:www.dormforce.net/blog/rockins

  Enjoy yourself!
*/

#include <stdio.h>
#include <stdlib.h>

#define N 			4
#define M 			2*N-1
#define TRUE		1
#define FALSE		0
#define MAX			32767
#define CODE_LEN	5
#define CODE_NUM	N

/*debug switch*/
#ifdef __DEBUG__
//#define __DEBUG__
#undef __DEBUG__
#endif

/*typedef two necessary data structure*/
typedef struct HuffmanNodeType{				/* tree node type */
    int 	data;							/* data or keyword */
    int 	parent;							/* point to parent */
    int 	lchild;							/* point to left child*/
    int 	rchild;							/* point to right child*/
}HuffmanNode, *HuffmanNodePtr;

typedef struct HuffmanCodeType{				/* code type */
	char	token;							/* token: here is just 'A','B','C',and 'D'*/
    char	code[CODE_LEN];					/* code array */
    int 	start;							/* code start index */
}HuffmanCode, *HuffmanCodePtr;    

/*prototype declaration start*/
int HuffmanInit(HuffmanNode huffmanNodeArr[], int data[]);
int HuffmanSelect(HuffmanNode huffmanNodeArr[], int n, int *s, int *t);
int HuffmanGenerate(HuffmanNode huffmanNodeArr[]);
int HuffmanCodeing(HuffmanNode huffmanNodeArr[], HuffmanCode huffmanCodeArr[]);
int HuffmanCodeingInit(HuffmanCode huffmanCodeArr[], char token[]);
int HuffmanCodeOutput(HuffmanCode huffmanCodeArr[]);
int HuffmanDebug(HuffmanNode huffmanNodeArr[], HuffmanCode huffmanCodeArr[]);
/*end prototype declaration*/


/*
Function:
    init the tree's nodes, according the data[]
Params:
    HuffmanNode huffmanNodeArr[]	--all nodes of the tree
    int data[]						--the probability of each token in message,suppose 'A','B','C','D'
Return:
    (None)
*/
int HuffmanInit(HuffmanNode huffmanNodeArr[], int data[])
{
    int i;
    for (i = 1; i <= M; i++){
        huffmanNodeArr[i].parent = 0;
        huffmanNodeArr[i].lchild = 0;
        huffmanNodeArr[i].rchild = 0;
    }
    for (i = 1; i <= N; i++){
    	huffmanNodeArr[i].data = data[i];
    }
}

/*
Function:
    select two minimum node
Params:
    HuffmanNode huffmanNodeArr[]	--all nodes of the tree
    n			--current generating node index
    s			--point to the minimum node
    t			--point to the secondary minimum node
Return:
    success	: TRUE
    fail	: FALSE
*/
int HuffmanSelect(HuffmanNode huffmanNodeArr[], int n, int *s, int *t)
{
    int i;
    int x;

    *s = 0;
    *t = 0;
    x = MAX;
    for (i = 1; i < n; i++){
        if ((huffmanNodeArr[i].parent == 0) && (huffmanNodeArr[i].data < x)){
            *s = i;
            x = huffmanNodeArr[i].data;
        }    
    }
    x = MAX;    
    for (i = 1; i < n; i++){
        if ((i != *s) && (huffmanNodeArr[i].parent ==0) && (huffmanNodeArr[i].data < x)){
            *t = i;
            x = huffmanNodeArr[i].data;
        }    
    }    
    if ((*s == 0) || (*t == 0))		return(FALSE);
    return(TRUE);
}    
    
/*
Function:
    generating the huffman tree
Params:
    HuffmanNode huffmanNodeArr[]	--all nodes of the tree
Return:
    (None)
*/
int HuffmanGenerate(HuffmanNode huffmanNodeArr[])
{
    int i;
    int s, t;
    for (i = N + 1; i <= M; i++){
        if (HuffmanSelect(huffmanNodeArr, i, &s, &t)){
            huffmanNodeArr[i].data = huffmanNodeArr[s].data + huffmanNodeArr[t].data;
            huffmanNodeArr[i].lchild = s;
            huffmanNodeArr[i].rchild = t;
            huffmanNodeArr[s].parent = i;
            huffmanNodeArr[t].parent = i;
        }else{
            printf("error");
        }        
    }    
}      		

/*
Function:
    according the generating huffman tree to encode the message token
Params:
    HuffmanNode huffmanNodeArr[]	--all nodes of the tree
    HuffmanCode huffmanCodeArr[]	--codes array
Return:
    (None)
*/
int HuffmanCodeing(HuffmanNode huffmanNodeArr[], HuffmanCode huffmanCodeArr[])
{
    int s, p;
    int c;
    for (s = 1; s <= N; s++){
        c = s;
        p = huffmanNodeArr[s].parent;
        while (p != 0){
            if (c == huffmanNodeArr[p].lchild){
                huffmanCodeArr[s].start -= 1;
                huffmanCodeArr[s].code[huffmanCodeArr[s].start] = '0';
            }else if (c == huffmanNodeArr[p].rchild){
                huffmanCodeArr[s].start -= 1;
                huffmanCodeArr[s].code[huffmanCodeArr[s].start] = '1';
            }
            c = p;
            p = huffmanNodeArr[p].parent;    
        }   
    }    
}
    
/*
Function:
    init the coding array
Params:
    HuffmanCode huffmanCodeArr[]	--codes array
    char token[]					--all token dwell in this array
Return:
    (None)
*/
int HuffmanCodeingInit(HuffmanCode huffmanCodeArr[], char token[])
{
    int i;
    for (i = 1; i <= N; i++){
        huffmanCodeArr[i].token = token[i];
        huffmanCodeArr[i].start = CODE_LEN;
        huffmanCodeArr[i].code[CODE_LEN] = '\0';
    }    
}

/*
Function:
    output coding result
Params:
    HuffmanCode huffmanCodeArr[]	--codes array
Return:
    (None)
*/
int HuffmanCodeOutput(HuffmanCode huffmanCodeArr[])
{
    int i;
    for (i = 1; i <= N; i++){
        printf("#%d : token=%c,code=%s\n",
        		 i, 
           		 huffmanCodeArr[i].token,
           		 huffmanCodeArr[i].code + huffmanCodeArr[i].start);
    }
}    

/*
Function:
    function as auxiliary debug and test facilitation
Params:
    HuffmanNode huffmanNodeArr[]	--all nodes of the tree
    HuffmanCode huffmanCodeArr[]	--codes array
Return:
    (None)
*/
int HuffmanDebug(HuffmanNode huffmanNodeArr[], HuffmanCode huffmanCodeArr[])
{
    int i;
    printf("Huffman Tree Gernerate:\n");
    for (i = 1; i <=M ; i++){
        printf("#%d : data=%d,parent=%d,lchild=%d,rchild=%d\n", i,
        		huffmanNodeArr[i].data,
        		huffmanNodeArr[i].parent,
        		huffmanNodeArr[i].lchild,
        		huffmanNodeArr[i].rchild);
    }
    printf("Huffman Code:\n");
    for (i = 1; i <= N; i++){
        printf("#%d : token=%c,code=%s\n",
        		 i, 
           		 huffmanCodeArr[i].token,
           		 huffmanCodeArr[i].code + huffmanCodeArr[i].start);
    }
}
    
int main(void)
{
    char		token[N + 1] = 	{0, 'A', 'B', 'C', 'D'};			// 0 is unused
    int			data[N + 1] = 	{0,  1,   2,   3,   4 };			// 0 is unused
    HuffmanNode	huffmanNodeArr[M + 1];								// huffmanNodeArr[0] unused
    HuffmanCode huffmanCodeArr[N + 1];								// huffmanCodeArr[0] unused
    
    HuffmanInit(huffmanNodeArr, data);
    HuffmanCodeingInit(huffmanCodeArr, token);
    HuffmanGenerate(huffmanNodeArr);
    HuffmanCodeing(huffmanNodeArr,huffmanCodeArr);
    HuffmanCodeOutput(huffmanCodeArr);
    
#ifdef __DEBUG__
    HuffmanDebug(huffmanNodeArr, huffmanCodeArr);
#endif

    return(0);
}
