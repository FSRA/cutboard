
/******************************************************************
*����и�����:
*����һ����ΪW�ľ��ΰ壬���ΰ�ĸ߶Ȳ������ơ�����Ҫ�Ӱ��Ϸֱ��и�
*��n���߶�Ϊhi�����Ϊwi�ľ���������и�Ĺ���������ĸ߶ȷ��������
*��ĸ߶ȷ��򱣳�һ�¡�������и�ʹ����ʹ�õľ��ΰ�ĸ߶�h��С��
*ʹ�û����㷨ʵ�ֳ���
*���뷽������cpp�ļ����ڱ����ͬʱ��������,�� "settings->link"
*��link���Object/library modules:�м���Opengl32.lib glu32.lib glaux.lib
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

typedef struct MuKuai{  //ľ��ṹ�嶨��
    float h;            //ľ��߶�
    float w;            //ľ����
    struct MuKuai *next; //������ά��һ��ľ������
}MuKuai;

typedef struct MuBan{  //ľ��ṹ�嶨��
    float x;           //(x,y)Ϊľ������½����꣬���ڻ�ͼ
    float y;
    float h;           //ľ��߶�
    float w;           //ľ����
    struct MuBan *next;//������ά��һ��ľ������
}MuBan;

MuKuai *headMuKuai = NULL;  //ľ������ͷָ��
MuBan *headMuBan = NULL;    //ľ������ͷָ��
MuBan *A = NULL;            //���ڴ�����ս�������ڻ�ͼ

MuBan *X;                   //X���飬���ڱ��浱ǰ�ҵ���·��
MuBan *bestX;               //bestx���飬���ڱ��浱ǰ�ҵ������·��

int n;                       //ľ���ܸ���
int Line;                    //Line���ڿ��Ƶݹ���ȣ�����������ʵ�鱨���н�������ϸ˵��
float W;                     //ľ����
float h = 0;  //��ǰ�ҵ���·��ľ���ܸ߶�
float besth = 35767; //���·��ľ��߶�
int x = 0;  //���ڵ���opengl����Ļ���������
int k = 0;  //���ڶ�̬��ʾľ��仯
float **C = NULL;  //������ɫ����ͷָ��

//--------------------------------------------------
//opengl����
void myinit(void);
void CALLBACK myReshape(GLsizei w,GLsizei h);
void CALLBACK display(void);
void CALLBACK down(void);//�� DOWN
void DrawMyObjects(void);
//--------------------------------------------------

//��������Ķ�ά�����к��ж�̬������λ���飬��������ָ��
float **CreatFloatArray_2(float **Head,int m,int n)
{
    int i,j;
    float *p=NULL;

    if( !(m > 0 && n > 0) ){                              //�ж�ά���Ƿ���ȷ
        printf("���������ά��\n");
        return NULL;
    }
    if( !(p = (float *)malloc(sizeof(float) * m * n))){   //��������ռ�
        printf("Malloc error\n");
        return NULL;
    }
    if( !(Head = (float **)malloc(sizeof(float *)*m))){   //�������ָ��ռ�
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

MuKuai *find(int k)//��ľ�������в��ҵ�k��Ԫ��
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

void sortInsert(MuKuai *Q)  //��headMuKuai�а��շǵݼ��������ľ��ڵ㣬Q��һ�����еĽڵ�
{
    MuKuai *p = headMuKuai,*q = headMuKuai;
    if(p == NULL){   //��������ĵ�һ��Ԫ��
        headMuKuai = Q;
        return ;
    }
    while(p->h > Q->h && p->next != NULL){  //Ѱ�Һ��ʵĲ���λ��
        q = p;
        p = p->next;
    }
    if(p == headMuKuai){   //�滻��һ��Ԫ��
        Q->next = p;
        headMuKuai = Q;
        return ;
    }
    if(p->next == NULL && p->h > Q->h)//��������ĩβ
        p->next = Q;
    else{            //���������м�
        Q->next = p;
        q->next = Q;
    }
}

void freeMuBan(MuBan *X)//�ͷ�ľ������ռ䣬ÿ�����滻���·���ǵ���
{
    MuBan *p = X,*Q = NULL;
    while(p != NULL){
        Q = p->next;
        free(p);
        p = Q;
    }
}

int even(float x,float y)//���ڸ������ľ������⣬�ô��ַ����������
                            //�����������Ƿ����
{
    if(fabs(x-y)<0.00001)
        return 1;
    else
        return 0;
}

 //ɾ��ľ�������е�Ԫ�أ�ʵ��Ԫ�ػ���
void delMuBan(float x,float y,float w,float h)
{
    MuBan *p = X,*q;
    //�жϵ�һ��Ԫ���Ƿ���ͬ
    if(even(X->x,x) && even(X->y,y) && even(X->h,h) && even(X->w,w)){
        X = X->next;
        free(p);
        return ;
    }
    //����Ԫ��
    while(p != NULL && (!even(p->x,x) || !even(p->y,y) || !even(p->w,w) || !even(p->h,h))){
        q = p;
        p = p->next;
    }
    //���û���ҵ���Ӧ��Ԫ��
    if(p == NULL){
        printf("delMuBan error\n");
        exit(1);
    }
    q->next = p->next;
    free(p);
}
//���ҵ���ľ����뵱ǰ·�����������ɽ��
void insertMuBan(float x,float y,float w,float h)
{
    MuBan *p;

    if(X==NULL){  //ľ��������ûӴԪ��
        if(!(X = (MuBan *)malloc(sizeof(MuBan) * 1))){//����ռ�
            printf("malloc error!\n");
            return ;
        }
        X->h = h; X->w=w; X->x = x;
        X->y = y; X->next = NULL;
        return ;
    }
    //���ľ������Ԫ������������ͷ��
    if(!(p = (MuBan *)malloc(sizeof(MuBan) * 1))){//����ռ�
        printf("malloc error!\n");
        return ;
    }
    p->h = h; p->w = w;
    p->x = x; p->y = y;
    p->next=X;
    X=p;
}
//���ݹ���ʵ��
void backtracking(int level,int num)
{

    MuBan *k1,*k2,*k3;   //��ʱָ��
    MuBan *MuBan1,*MuBan2; //���ڴ���и�������ľ��
    MuKuai *l;  //��ǰ��Ҫ���õ�ľ��
    MuBan *pX = NULL,*pbestX = NULL;//pxΪ��ǰ�ҵ���·����ͷָ�룬pbestXΪ���·����Ͷָ��

    if(num > n){
        printf("�ҵ�һ��·��,�߶�:%f �ϴ���Ѹ߶�:%f\n",h,besth);
        printf("���������У����Ժ�...\n\n");
        if(h < besth){   //��ǰ�ҵ���·���ȳ��򱣴����ѻ�Ҫ�ã��滻���·��
            free(bestX); //�ͷ�����·���ռ�
            pX = X;
            if(!(pbestX = (MuBan *)malloc(sizeof(MuBan) * 1))){//����ռ�
                printf("malloc error!\n");
                return ;
            }
            *pbestX = *pX;  //���Ƶ�һ���ڵ�
            pbestX->next = NULL;
            bestX = pbestX;
            pX = pX->next;
            while(pX != NULL){ //��������ڵ�
                if(!(pbestX->next = (MuBan *)malloc(sizeof(MuBan) * 1))){//����ռ�
                    printf("malloc error!\n");
                    return ;
                }
                *(pbestX->next) = *pX;
                pbestX->next->next = NULL;
                pbestX = pbestX->next;
                pX = pX->next;
            }
            besth=h; //�µ����ľ��߶�
        }
        return;
    }
    l = find(num); //��Ҫ�����ľ��
    k1 = headMuBan; k2 = k1->next; k3 = k2->next;
    if(level > Line){  //����ݹ�����Ѿ������ݹ�����ߣ�����̰���㷨���ٵó����
        //�ҵ����ʵĿ��Խ����и��ľ��
        while(!(l->h <= (k2->h + 0.0001)  && l->w <= (k2->w + 0.0001))){
            k1 = k2;
            k2 = k2->next;
        }
        k3 = k2->next;
        level++; num++;
        k1->next=k3;

        if(!(MuBan1 = (MuBan *)malloc(sizeof(MuBan) * 1))){//����ռ�
            printf("malloc error!\n");
            return ;
        }
        if(!(MuBan2 = (MuBan *)malloc(sizeof(MuBan) * 1))){//����ռ�
            printf("malloc error!\n");
            return ;
        }
        MuBan1->next = MuBan2;

        //��ľ���и������MuBan1��MuBan2
        MuBan1->x = l->w + k2->x; MuBan1->y = k2->y;
        MuBan1->w = k2->w - l->w; MuBan1->h = l->h;

        MuBan2->x=k2->x; MuBan2->y=k2->y+l->h;
        MuBan2->w=k2->w; MuBan2->h=k2->h-l->h;

        if(k2->h > 4000){ //��ѡ���и��ľ���ǻ��������ո߶ȵ�ľ�壬Ҫ���м�֦
            k1->next = MuBan1;//�Ѽ��к��ľ�����ľ������
            MuBan2->next = k3;
            h += l->h;//�߶�����
            if(h>=besth){//��֦����
                h -= l->h;
                free(MuBan1);free(MuBan2);
                k1->next=k2;//��ԭľ��
                k2->next=k3;
                level--; num--; //����
                return;
            }
            insertMuBan(k2->x,k2->y,l->w,l->h);//����ѡ�е�ľ��,���ɽ��·��
            backtracking(level,num);//��������
            delMuBan(k2->x,k2->y,l->w,l->h);//����
            h -= l->h;
        }
        else{ //�и�ľ���else,��ѡ���и��ľ�岻�����Ӹ߶�
            k1->next=MuBan1;//�Ѽ��к��ľ�����ľ������
            MuBan2->next=k3;
            insertMuBan(k2->x,k2->y,l->w,l->h);//����ѡ�е�ľ��,���ɽ��·��
            backtracking(level,num);//��������
            delMuBan(k2->x,k2->y,l->w,l->h);//����
        }
        free(MuBan1); free(MuBan2);
        k1->next=k2;//����
        k2->next=k3;
        level--; num--;
    } //�ݹ��if
    else{ //����ݹ�ĸ߶Ȳ���̫���������������ռ�
        while(k2 != NULL){  //���ݷ��ҵ��и��ľ��K2
            if((l->h <= (k2->h + 0.0001)  && l->w <= (k2->w + 0.0001))){
                level++; num++;  //�������
                k1->next = k3;
                if(!(MuBan1 = (MuBan *)malloc(sizeof(MuBan) * 1))){//����ռ�
                    printf("malloc error!\n");
                    return ;
                }
                if(!(MuBan2 = (MuBan *)malloc(sizeof(MuBan) * 1))){//����ռ�
                    printf("malloc error!\n");
                    return ;
                }
                MuBan1->next = MuBan2;
                //��ľ���и������MuBan1��MuBan2
                MuBan1->x = l->w + k2->x; MuBan1->y = k2->y;
                MuBan1->w = k2->w - l->w; MuBan1->h = l->h;

                MuBan2->x=k2->x; MuBan2->y=k2->y+l->h;
                MuBan2->w=k2->w; MuBan2->h=k2->h-l->h;

                if(k2->h < 4000){ //��ѡ���и��ľ���ǻ��������ո߶ȵ�ľ�壬Ҫ���м�֦
                    k1->next = MuBan1;//�Ѽ��к��ľ�����ľ������
                    MuBan2->next = k3;
                    insertMuBan(k2->x,k2->y,l->w,l->h);//����ѡ�е�ľ��,���ɽ��·��
                    backtracking(level,num);//��������
                    delMuBan(k2->x,k2->y,l->w,l->h);//����
                }else{
                    k1->next = MuBan1;
                    MuBan2->next = k3;
                    h += l->h;//�߶�����
                    if(h >= besth){  //��֦
                        h -= l->h;
                        free(MuBan1); free(MuBan2);
                        k1->next=k2; k2->next=k3;
                        level--; num--;
                        return;
                    }
                    insertMuBan(k2->x,k2->y,l->w,l->h);//����ѡ�е�ľ��,���ɽ��·��
                    backtracking(level,num);//��������
                    delMuBan(k2->x,k2->y,l->w,l->h);//����
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

    char FileName[20];  //�����ļ�
    clock_t start,end; //���Գ�������ʱ��
    FILE *fp;

    printf("\nע��:��������ʱ���������ڣ�����̨���ں�ͼ�δ���,���ڳ�������Ҫ�ڿ���̨���������������ļ������ʿ�ʼʱͼ�δ���");
    printf("��û�м����,�������������ļ�����ͼ�δ��ڼ�������ɰ������ϵġ����·��������̬��ʾľ���и����\n\n");
    printf("�����������ļ�:");
    scanf("%s",FileName);
    fflush(stdin);
    if(!(fp = fopen(FileName,"r"))){  //�������ļ�
        printf("����������ļ�����%s��\n",FileName);
        return -1;
    }
    start = clock();

    fscanf(fp,"%d %f",&n,&W);
    x = W;         //���ڵ���opengl����Ļ���������

    while(i <= n){
        if(!(p = (MuKuai *)malloc(sizeof(MuKuai) * 1))){//����ռ�
            printf("malloc error!\n");
            return -1;
        }
        fscanf(fp,"%f %f",&(p->h),&(p->w));
        p->next = NULL;
        sortInsert(p);
        i++;
    }
    //���ݲ�ͬ�������ļ������Ƶݹ����
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
    if(!(A = (MuBan *)malloc(sizeof(MuBan) * n))){//����ռ�
        printf("malloc error!\n");
        return -1;
    }
    if(!(headMuBan = (MuBan *)malloc(sizeof(MuBan) * 1))){//����ռ�
        printf("malloc error!\n");
        return -1;
    }
    if(!(headMuBan->next = (MuBan *)malloc(sizeof(MuBan) * 1))){//����ռ�
        printf("malloc error!\n");
        return -1;
    }
    //�������ͷ�ڵ㸳ֵ
    headMuBan->x = 0; headMuBan->y = 0;
    headMuBan->w = 0; headMuBan->h = 0;
   //�������ľ�帳ֵ
    headMuBan->next->x = 0; headMuBan->next->y = 0;
    headMuBan->next->w = W; headMuBan->next->h = 65767;
    headMuBan->next->next = NULL;

    backtracking(1,1);
    MuBan *pA = bestX;
    for(i = n-1; i >=0; i--, pA = pA->next)
        A[i] = *pA;
    end = clock();

    printf("��������%lf��\n",(double)(end - start) /  CLK_TCK);

        //������ɫ���飬���ڻ���ͼ��
    C = CreatFloatArray_2(C,n,3); //��̬������λ���飬���ڴ����ɫ
    for(i = 0; i < n; i++){
        C[i][0] = (rand()%3)/3.0 + 0.01;
        C[i][1] = (rand()%6)/6.0 + 0.3;
        C[i][2] = (rand()%9)/7 + 0.05;
    }
//opengl����
/////////////////////////////////////////////////////////////////////////
    auxInitDisplayMode(AUX_SINGLE|AUX_RGBA);                           //
    auxInitPosition(250,100,800,800);                                  //
    auxInitWindow("�����㷨��Ӧ��:����и�����");                          //
    myinit();                                                          //
    auxKeyFunc(AUX_DOWN,down);                                         //
    auxReshapeFunc(myReshape);                                         //
    auxMainLoop(display);                                              //
/////////////////////////////////////////////////////////////////////////
    return 1;
}


//���º���Ϊopenglͼ�λ��ƺ���
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
    glTranslatef(-(x * 0.5),-(x-0.1*x),0.0);//�ƶ���Ļ���굽ľ�����·�

    glBegin(GL_LINE_STRIP);    //����ľ��
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
