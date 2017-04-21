
/******************************************************************
*零件切割问题:
*给定一块宽度为W的矩形板，矩形板的高度不受限制。现需要从板上分别切割
*出n个高度为hi，宽度为wi的矩形零件。切割的规则是零件的高度方向与矩形
*板的高度方向保持一致。问如何切割使得所使用的矩形板的高度h最小？
*使用回溯算法实现程序。
*编译方法：打开cpp文件，在编译的同时建立工程,打开 "settings->link"
*在link项的Object/library modules:中加入Opengl32.lib glu32.lib glaux.lib
*
*
*                                      dao0oad@gmail.com
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glaux.h>
#include <time.h>

typedef struct MuKuai{  //木块结构体定义
    float h;            //木块高度
    float w;            //木块宽度
    struct MuKuai *next; //程序中维护一个木块链表
}MuKuai;

typedef struct MuBan{  //木板结构体定义
    float x;           //(x,y)为木板的左下脚坐标，便于绘图
    float y;
    float h;           //木板高度
    float w;           //木板宽度
    struct MuBan *next;//程序中维护一个木板链表
}MuBan;

MuKuai *headMuKuai = NULL;  //木块链表头指针
MuBan *headMuBan = NULL;    //木板链表头指针
MuBan *A = NULL;            //用于存放最终结果，便于画图

MuBan *X;                   //X数组，用于保存当前找到的路径
MuBan *bestX;               //bestx数组，用于保存当前找到的最佳路径

int n;                       //木块总个数
int Line;                    //Line用于控制递归深度，具体作用在实验报告中将会有详细说明
float W;                     //木板宽度
float h = 0;  //当前找到的路径木板总高度
float besth = 35767; //最佳路径木板高度
int x = 0;  //用于调整opengl中屏幕的相对坐标
int k = 0;  //用于动态显示木块变化
float **C = NULL;  //定义颜色数组头指针

//--------------------------------------------------
//opengl函数
void myinit(void);
void CALLBACK myReshape(GLsizei w,GLsizei h);
void CALLBACK display(void);
void CALLBACK down(void);//按 DOWN
void DrawMyObjects(void);
//--------------------------------------------------

//根据输入的二维数组行和列动态创建二位数组，返回数组指针
float **CreatFloatArray_2(float **Head,int m,int n)
{
    int i,j;
    float *p=NULL;

    if( !(m > 0 && n > 0) ){                              //判断维度是否正确
        printf("错误的数组维度\n");
        return NULL;
    }
    if( !(p = (float *)malloc(sizeof(float) * m * n))){   //申请总体空间
        printf("Malloc error\n");
        return NULL;
    }
    if( !(Head = (float **)malloc(sizeof(float *)*m))){   //申请二级指针空间
        printf("Malloc error\n");
        return NULL;
    }
    for(i = 0;i < m;i++)
        Head[i]=p + n * i;
    for(i = 0;i < m;i++)
        for(j = 0;j < n;j++)
            Head[i][j] = 0;
    return Head;
}

MuKuai *find(int k)//在木块链表中查找第k个元素
{
    MuKuai *p = headMuKuai;
    if(k > n){
        printf("Find failed!\n");
        return NULL;
    }
    for(int i = 1; i < k; i++)
        p = p->next;
    return p;
}

void sortInsert(MuKuai *Q)  //在headMuKuai中按照非递减排序插入木块节点，Q是一个已有的节点
{
    MuKuai *p = headMuKuai,*q = headMuKuai;
    if(p == NULL){   //插在链表的第一个元素
        headMuKuai = Q;
        return ;
    }
    while(p->h > Q->h && p->next != NULL){  //寻找合适的插入位置
        q = p;
        p = p->next;
    }
    if(p == headMuKuai){   //替换第一个元素
        Q->next = p;
        headMuKuai = Q;
        return ;
    }
    if(p->next == NULL && p->h > Q->h)//插在链表末尾
        p->next = Q;
    else{            //插在链表中间
        Q->next = p;
        q->next = Q;
    }
}

void freeMuBan(MuBan *X)//释放木板链表空间，每次在替换最佳路径是调用
{
    MuBan *p = X,*Q = NULL;
    while(p != NULL){
        Q = p->next;
        free(p);
        p = Q;
    }
}

int even(float x,float y)//由于浮点数的精度问题，用此种方法检测两个
                            //两个浮点数是否相等
{
    if(fabs(x-y)<0.00001)
        return 1;
    else
        return 0;
}

 //删除木板数组中的元素，实现元素回溯
void delMuBan(float x,float y,float w,float h)
{
    MuBan *p = X,*q;
    //判断第一个元素是否相同
    if(even(X->x,x) && even(X->y,y) && even(X->h,h) && even(X->w,w)){
        X = X->next;
        free(p);
        return ;
    }
    //查找元素
    while(p != NULL && (!even(p->x,x) || !even(p->y,y) || !even(p->w,w) || !even(p->h,h))){
        q = p;
        p = p->next;
    }
    //如果没有找到相应的元素
    if(p == NULL){
        printf("delMuBan error\n");
        exit(1);
    }
    q->next = p->next;
    free(p);
}
//把找到的木板插入当前路径，用于生成结果
void insertMuBan(float x,float y,float w,float h)
{
    MuBan *p;

    if(X==NULL){  //木板数组中没哟元素
        if(!(X = (MuBan *)malloc(sizeof(MuBan) * 1))){//申请空间
            printf("malloc error!\n");
            return ;
        }
        X->h = h; X->w=w; X->x = x;
        X->y = y; X->next = NULL;
        return ;
    }
    //如果木板中有元素则插在链表的头部
    if(!(p = (MuBan *)malloc(sizeof(MuBan) * 1))){//申请空间
        printf("malloc error!\n");
        return ;
    }
    p->h = h; p->w = w;
    p->x = x; p->y = y;
    p->next=X;
    X=p;
}
//回溯功能实现
void backtracking(int level,int num)
{

    MuBan *k1,*k2,*k3;   //临时指针
    MuBan *MuBan1,*MuBan2; //用于存放切割后的两块木板
    MuKuai *l;  //当前正要放置的木块
    MuBan *pX = NULL,*pbestX = NULL;//px为当前找到的路径的头指针，pbestX为最佳路径的投指针

    if(num > n){
        printf("找到一条路径,高度:%f 上次最佳高度:%f\n",h,besth);
        printf("程序运行中，请稍后...\n\n");
        if(h < besth){   //当前找到的路径比程序保存的最佳还要好，替换最佳路径
            free(bestX); //释放已有路径空间
            pX = X;
            if(!(pbestX = (MuBan *)malloc(sizeof(MuBan) * 1))){//申请空间
                printf("malloc error!\n");
                return ;
            }
            *pbestX = *pX;  //复制第一个节点
            pbestX->next = NULL;
            bestX = pbestX;
            pX = pX->next;
            while(pX != NULL){ //复制其余节点
                if(!(pbestX->next = (MuBan *)malloc(sizeof(MuBan) * 1))){//申请空间
                    printf("malloc error!\n");
                    return ;
                }
                *(pbestX->next) = *pX;
                pbestX->next->next = NULL;
                pbestX = pbestX->next;
                pX = pX->next;
            }
            besth=h; //新的最佳木板高度
        }
        return;
    }
    l = find(num); //需要处理的木块
    k1 = headMuBan; k2 = k1->next; k3 = k2->next;
    if(level > Line){  //如果递归深度已经超过递归控制线，采用贪心算法快速得出结果
        //找到合适的可以进行切割的木板
        while(!(l->h <= (k2->h + 0.0001)  && l->w <= (k2->w + 0.0001))){
            k1 = k2;
            k2 = k2->next;
        }
        k3 = k2->next;
        level++; num++;
        k1->next=k3;

        if(!(MuBan1 = (MuBan *)malloc(sizeof(MuBan) * 1))){//申请空间
            printf("malloc error!\n");
            return ;
        }
        if(!(MuBan2 = (MuBan *)malloc(sizeof(MuBan) * 1))){//申请空间
            printf("malloc error!\n");
            return ;
        }
        MuBan1->next = MuBan2;

        //将木板切割成两块MuBan1和MuBan2
        MuBan1->x = l->w + k2->x; MuBan1->y = k2->y;
        MuBan1->w = k2->w - l->w; MuBan1->h = l->h;

        MuBan2->x=k2->x; MuBan2->y=k2->y+l->h;
        MuBan2->w=k2->w; MuBan2->h=k2->h-l->h;

        if(k2->h > 4000){ //被选中切割的木板是会增加最终高度的木板，要进行剪枝
            k1->next = MuBan1;//把剪切后的木板插入木板链表
            MuBan2->next = k3;
            h += l->h;//高度增加
            if(h>=besth){//剪枝部分
                h -= l->h;
                free(MuBan1);free(MuBan2);
                k1->next=k2;//还原木板
                k2->next=k3;
                level--; num--; //回溯
                return;
            }
            insertMuBan(k2->x,k2->y,l->w,l->h);//插入选中的木板,生成结果路径
            backtracking(level,num);//继续搜索
            delMuBan(k2->x,k2->y,l->w,l->h);//回溯
            h -= l->h;
        }
        else{ //切割木块的else,被选中切割的木板不会增加高度
            k1->next=MuBan1;//把剪切后的木板插入木板链表
            MuBan2->next=k3;
            insertMuBan(k2->x,k2->y,l->w,l->h);//插入选中的木板,生成结果路径
            backtracking(level,num);//继续搜索
            delMuBan(k2->x,k2->y,l->w,l->h);//回溯
        }
        free(MuBan1); free(MuBan2);
        k1->next=k2;//回溯
        k2->next=k3;
        level--; num--;
    } //递归的if
    else{ //如果递归的高度不是太深，则继续深度搜索解空间
        while(k2 != NULL){  //回溯法找到切割的木板K2
            if((l->h <= (k2->h + 0.0001)  && l->w <= (k2->w + 0.0001))){
                level++; num++;  //深度搜索
                k1->next = k3;
                if(!(MuBan1 = (MuBan *)malloc(sizeof(MuBan) * 1))){//申请空间
                    printf("malloc error!\n");
                    return ;
                }
                if(!(MuBan2 = (MuBan *)malloc(sizeof(MuBan) * 1))){//申请空间
                    printf("malloc error!\n");
                    return ;
                }
                MuBan1->next = MuBan2;
                //将木板切割成两块MuBan1和MuBan2
                MuBan1->x = l->w + k2->x; MuBan1->y = k2->y;
                MuBan1->w = k2->w - l->w; MuBan1->h = l->h;

                MuBan2->x=k2->x; MuBan2->y=k2->y+l->h;
                MuBan2->w=k2->w; MuBan2->h=k2->h-l->h;

                if(k2->h < 4000){ //被选中切割的木板是会增加最终高度的木板，要进行剪枝
                    k1->next = MuBan1;//把剪切后的木板插入木板链表
                    MuBan2->next = k3;
                    insertMuBan(k2->x,k2->y,l->w,l->h);//插入选中的木板,生成结果路径
                    backtracking(level,num);//继续搜索
                    delMuBan(k2->x,k2->y,l->w,l->h);//回溯
                }else{
                    k1->next = MuBan1;
                    MuBan2->next = k3;
                    h += l->h;//高度增加
                    if(h >= besth){  //剪枝
                        h -= l->h;
                        free(MuBan1); free(MuBan2);
                        k1->next=k2; k2->next=k3;
                        level--; num--;
                        return;
                    }
                    insertMuBan(k2->x,k2->y,l->w,l->h);//插入选中的木板,生成结果路径
                    backtracking(level,num);//继续搜索
                    delMuBan(k2->x,k2->y,l->w,l->h);//回溯
                    h -= l->h;
                }
                free(MuBan1);free(MuBan2);
                k1->next=k2;k2->next=k3;
                level--; num--;
            }//if
            k1=k2;  k2=k2->next;
            if(k2 == NULL)
                break;
            k3 = k2->next;
        }//while
    }
}

int main(void)

{
    int i = 1;

    MuKuai *p = NULL;

    char FileName[20];  //数据文件
    clock_t start,end; //测试程序运行时间
    FILE *fp;

    printf("\n注意:程序运行时有两个窗口，控制台窗口和图形窗口,由于程序中需要在控制台窗口中输入数据文件名，故开始时图形窗口");
    printf("是没有激活的,请在输入数据文件后点击图形窗口激活，激活后可按键盘上的“向下方向键”动态显示木板切割情况\n\n");
    printf("请输入数据文件:");
    scanf("%s",FileName);
    fflush(stdin);
    if(!(fp = fopen(FileName,"r"))){  //打开数据文件
        printf("错误的数据文件名“%s”\n",FileName);
        return -1;
    }
    start = clock();

    fscanf(fp,"%d %f",&n,&W);
    x = W;         //用于调整opengl中屏幕的相对坐标

    while(i <= n){
        if(!(p = (MuKuai *)malloc(sizeof(MuKuai) * 1))){//申请空间
            printf("malloc error!\n");
            return -1;
        }
        fscanf(fp,"%f %f",&(p->h),&(p->w));
        p->next = NULL;
        sortInsert(p);
        i++;
    }
    //根据不同的数据文件，控制递归深度
    if(n > 150)
        Line = 12;
    else if(n == 16)
        Line = 16;
    else if(n == 25)
        Line = 14;
    else if(n == 50)
        Line = 14;
    else
        Line = 15;
    if(!(A = (MuBan *)malloc(sizeof(MuBan) * n))){//申请空间
        printf("malloc error!\n");
        return -1;
    }
    if(!(headMuBan = (MuBan *)malloc(sizeof(MuBan) * 1))){//申请空间
        printf("malloc error!\n");
        return -1;
    }
    if(!(headMuBan->next = (MuBan *)malloc(sizeof(MuBan) * 1))){//申请空间
        printf("malloc error!\n");
        return -1;
    }
    //给虚拟的头节点赋值
    headMuBan->x = 0; headMuBan->y = 0;
    headMuBan->w = 0; headMuBan->h = 0;
   //给最初的木板赋值
    headMuBan->next->x = 0; headMuBan->next->y = 0;
    headMuBan->next->w = W; headMuBan->next->h = 65767;
    headMuBan->next->next = NULL;

    backtracking(1,1);
    MuBan *pA = bestX;
    for(i = n-1; i >=0; i--, pA = pA->next)
        A[i] = *pA;
    end = clock();

    printf("程序运行%lf秒\n",(double)(end - start) /  CLK_TCK);

        //生成颜色数组，用于绘制图形
    C = CreatFloatArray_2(C,n,3); //动态创建二位数组，用于存放颜色
    for(i = 0; i < n; i++){
        C[i][0] = (rand()%3)/3.0 + 0.01;
        C[i][1] = (rand()%6)/6.0 + 0.3;
        C[i][2] = (rand()%9)/7 + 0.05;
    }
//opengl函数
/////////////////////////////////////////////////////////////////////////
    auxInitDisplayMode(AUX_SINGLE|AUX_RGBA);                           //
    auxInitPosition(250,100,800,800);                                  //
    auxInitWindow("回溯算法的应用:零件切割问题");                          //
    myinit();                                                          //
    auxKeyFunc(AUX_DOWN,down);                                         //
    auxReshapeFunc(myReshape);                                         //
    auxMainLoop(display);                                              //
/////////////////////////////////////////////////////////////////////////
    return 1;
}


//以下函数为opengl图形绘制函数
///////////////////////////////////////////////////////////////////////////////////////
void myinit(void)                                                                    //
{                                                                                    //
  glClearColor(0.0,0.0,0.0,0.0);                                                     //
  glClear(GL_COLOR_BUFFER_BIT);
  glShadeModel(GL_FLAT);
}

void CALLBACK down(void)
{
    if(k < n-1)
        k++;
    else
        return ;
}

void CALLBACK myReshape(GLsizei w,GLsizei h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(w<=h)
        glOrtho(-x,x,-x*(GLfloat)h/(GLfloat)w,x*(GLfloat)h/(GLfloat)w,-50.0,50.0);
    else
        glOrtho(-x*(GLfloat)h/(GLfloat)w,x*(GLfloat)h/(GLfloat)w,-x,x,-50.0,50.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void CALLBACK display(void)
{
    DrawMyObjects();
    glFlush();
}

void DrawMyObjects(void)
{
    int i = 0;
    MuBan *p = bestX;
    glClear (GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glColor3f(1.0,0.0,0.0);
    glTranslatef(-(x * 0.5),-(x-0.1*x),0.0);//移动屏幕坐标到木板左下方

    glBegin(GL_LINE_STRIP);    //绘制木板
        glVertex2f(0.0,0.0);
        glVertex2f(0.0,2*x-0.2*x);
        glVertex2f(0.0,0.0);
        glVertex2f(x,0.0);
        glVertex2f(x,0.0);
        glVertex2f(x,2*x-0.2*x);
    glEnd();

    while(i <= k)
    {
        glColor3fv(C[i]);
        glBegin(GL_QUADS );
            glVertex2f(A[i].x,A[i].y + A[i].h);
            glVertex2f(A[i].x,A[i].y);
            glVertex2f(A[i].x + A[i].w,A[i].y);
            glVertex2f(A[i].x + A[i].w,A[i].y + A[i].h);
        glEnd();
        i++;
    }
                                                           //
                                                                          //
}                                                                                 //
////////////////////////////////////////////////////////////////////////////////////
