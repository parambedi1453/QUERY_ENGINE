#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>

int page = 0;
char page_url[150]={0};
typedef struct countnode
{
  int pageno;
  char url[150];
  int count;
  struct countnode *countnext;
}countnode;

typedef struct wordnode
{
  char word[30];
  int word_key;
  struct wordnode *next;
  struct countnode *list;
}wordnode;

typedef struct querynode
{
    int linkno;
    char link[150];
    int totalcount;
    int rank;
    struct querynode *querynext;
}querynode;
querynode *qhead = NULL;
querynode *qlast = NULL;

wordnode *head = NULL;
struct hash
{
  struct wordnode *first;
  struct wordnode *last;
};
struct hash h[50];
int hash_func(char s[])
{
			/*int i=0,sum=0;
			for(i=0;s[i]!='\0';i++)
			{
				sum=sum+s[i];
			}
			return (sum%50);*/
            return strlen(s);
}
wordnode* check_duplicate(wordnode *k)
{
  wordnode *chk = head;
  while(chk!=NULL)
  {
    if(strcmp(chk->word,k->word)==0)
    {
      return chk;
    }
    chk=chk->next;
  }
  return NULL;
}
void create_node(char wrd[])
{

      wordnode *temp=NULL;
      temp = (wordnode*)calloc(1,sizeof(wordnode));
      strcpy(temp->word,wrd);
      int index = hash_func(wrd);
      temp->word_key = index;
      temp->next=NULL;
      temp->list = NULL;
      if(head==NULL)
      {
            head = temp;
            h[index].first = temp;
            h[index].last = temp;

            countnode *countTemp=NULL;
            countTemp = (countnode*)calloc(1,sizeof(countnode));
            countTemp->pageno = page;
            countTemp->count = 1;
            strcpy(countTemp->url,page_url);
            countTemp->countnext=NULL;
             head->list = countTemp;
      }
      else
      {
        wordnode *link = check_duplicate(temp);
          if(link==NULL)
          {
                    //create new because it donot exist earler
                    countnode *countTemp=NULL;
                    countTemp = (countnode*)calloc(1,sizeof(countnode));
                    countTemp->pageno = page;
                    countTemp->count = 1;
                    strcpy(countTemp->url,page_url);
                    countTemp->countnext=NULL;

                    temp->list = countTemp;

                      if(h[index].first==NULL)
                      {
                              h[index].first = temp;
                              h[index].last  = temp;
                              wordnode *p = NULL;
                              p=head;
                              while(p->next!=NULL)
                              p=p->next;
                              p->next = temp;
                      }
                      else
                      {

                          //this is to insert the node at the for the same key
                              wordnode *k=NULL;
                              k=h[index].last;
                              if(k->next!=NULL)
                              {
                                  temp->next=k->next;
                                  k->next = temp;
                              }
                              else
                              {
                                temp->next=NULL;
                                k->next=temp;
                              }
                              h[index].last = temp;



                      }
          }
          else
          {
            //if word exist then increment the count of the word

                    countnode *counthead=NULL;
                    counthead = link->list;
                    countnode *ptrcount = counthead;

                    int flag = 0;//to check that if that pages node exist or not

                    while(ptrcount!=NULL)
                    {
                      if(ptrcount->pageno == page)
                      {
                        ptrcount->count +=1;
                        flag=1;
                        break;
                      }
                      ptrcount=ptrcount->countnext;
                    }
        // if p2 donot exist then create and link it
                    if(flag==0)
                    {

                      countnode *countTemp=NULL;
                      countTemp = (countnode*)calloc(1,sizeof(countnode));
                      countTemp->pageno = page;
                      countTemp->count = 1;
                      strcpy(countTemp->url,page_url);
                      countTemp->countnext=NULL;
                      ptrcount = counthead;
                      while(ptrcount->countnext!=NULL)
                      ptrcount=ptrcount->countnext;
                      ptrcount->countnext = countTemp;

                    }
                      free(temp);
          }
      }
}
void extract_word(char *start,char *end)
{
    int d = end-start;
    //printf("%d\n",d);
    char *str = (char*)calloc(1,d);
    int k=0;
    char *temp = start;
    for(;temp!=end;temp++)
    str[k++]=*temp;
    char *wrd = (char*)calloc(1,30);
    int i=0,j=0,f=0;
    for(i=0;i<strlen(str);i++)
    {
          if(str[i]==']'||str[i]=='['||str[i]=='!'||str[i]==' '||str[i]==','||str[i]==':'||str[i]=='-'||str[i]=='.'||str[i]==')'||str[i]=='('||str[i]=='&'||str[i]==';'||str[i]=='-')
          {
            if((wrd[0]>='A'&&wrd[0]<='Z')||(wrd[0]>='a'&&wrd[0]<='z')&&(strlen(wrd)>2))
            {
                wrd[j]='\0';
              if(strlen(wrd)>=2)
                {
                  create_node(wrd);
                }
            }
          //  printf("\n%s\n",wrd);
            j=0;
          }
          else
          {
            wrd[j++]=str[i];
          }
    }
    if(j!=0)
    {
      if((wrd[0]>='A'&&wrd[0]<='Z')||(wrd[0]>='a'&&wrd[0]<='z'))
      {
          wrd[j]='\0';
          if(strlen(wrd)>2)
            {
              create_node(wrd);
            }
      }
  //    printf("\n%s\n",wrd);
    }

}
void print()
{
  wordnode *p=NULL;
  p=head;
  while(p!=NULL)
  {
    printf("%s %d == ",p->word,p->word_key);
    countnode *k = p->list;
    while(k!=NULL)
    {
      //printf(" %s  %d  %d \n",k->url,k->pageno,k->count);
     printf(" %d %d ",k->pageno,k->count);
      k=k->countnext;
    }
    printf("\n\n\n");
    p=p->next;
  }
}
void read_title(char *file,int file_size)
{
    char *tstart,*tend;
    for(int i=0;i<file_size;i++)
    {
      if(file[i]=='<'&&file[i+1]=='t')
      {
        tstart = &file[i];
        tstart = strpbrk(tstart,">");
        tstart++;
        break;
      }
    }
    tend = strpbrk(tstart,"<");
    extract_word(tstart,tend);

    /*printf("\n%ld\n",(tend-tstart));
    char *t ="CHITKARA UNIVERSITY - Best University in North India";
    printf("\n%ld\n",strlen(t));*/

}


void read_heading(char *file,int file_size)
{

  char *hstart,*hend;
  for(int i=0;i<file_size;i++)
  {
    int flag=0;
    if(file[i]=='<'&&file[i+1]=='h'&&file[i+2]>='1'&&file[i+2]<='6')
    {
      hstart = &file[i];
      hstart = strpbrk(hstart,">");
      hstart++;
      hend = strpbrk(hstart,"<");
      extract_word(hstart,hend);
    }
  }


}
void get_page_url(char *file,int file_size)
{
    int j=0;
    for(int i=0;i<file_size;i++)
    {
        if(file[i]=='\n')
        {
            break;
        }
        else
        {
            page_url[j++]=file[i];
        }
    }
    page_url[j]='\0';
}

void extract_file(char *str)
{
  struct stat st;
  stat(str,&st);
  int file_size  = st.st_size;
  char *file = (char*)calloc(1,file_size);
  FILE *temp = fopen(str,"r");
  char ch;
  int i=0;
  while((ch=fgetc(temp))!=EOF)
  {
      file[i++]=ch;
  }
  file[i]='\0';
//  printf("%s",file);
    get_page_url(file,file_size);
  read_title(file,file_size);
  read_heading(file,file_size);
 // print();
}
void query_list(char subq[],int key)
{
    wordnode *start = NULL,*end=NULL;
    countnode *found =NULL;
    start = h[key].first;
    end = h[key].last;
    while(start!=end)
    {
        if(strcmp(start->word,subq)==0)
        {
            found = start->list;
            break;
        }
        start = start->next;
    }
    if(found)
    {
        while(found!=NULL)
        {
            querynode *qtemp =NULL;
            qtemp = (querynode*)calloc(1,sizeof(querynode));
            qtemp->linkno = found->pageno;
            strcpy(qtemp->link,found->url);
            qtemp->totalcount = found->count;
            qtemp->querynext = NULL;
            qtemp->rank = 1;
            if(qhead == NULL)
            {
                qhead = qtemp;
                qlast = qtemp;
            }
            else
            {
                querynode *qptr =NULL;
                qptr = qhead;
                while(qptr!=NULL)
                {
                    if(qptr->linkno == found->pageno)
                    {
                        qptr->totalcount = qptr->totalcount+found->count;
                        qptr->rank = qptr->rank+1;
                        break;
                    }
                    qptr=qptr->querynext;
                }
                if(qptr==NULL)
                {
                    qlast->querynext =qtemp;
                    qlast = qtemp;
                }
                else
                {
                    free(qtemp);
                }
            }
            found = found->countnext;
        }

    }
}
void print_query_list()
{
    querynode *p = qhead;
    while(p!=NULL)
    {
        printf("%d %d %s %d\n",p->linkno,p->rank,p->link,p->totalcount);
        p=p->querynext;
    }
}
int main()
{
     /*char str[]="/home/parambedi/Desktop/index/1.txt";
     extract_file(str);
     strcpy(str,"/home/parambedi/Desktop/index/2.txt");
     extract_file(str);
     strcpy(str,"/home/parambedi/Desktop/index/3.txt");
     extract_file(str);
     strcpy(str,"/home/parambedi/Desktop/index/4.txt");
     extract_file(str);
     print();*/
    /*int i,index;
    for(i=1;i<=66;i++)
    {
      page=i;
      index =i;
      char indexarr[5]={0};
      sprintf(indexarr,"%d",index);
      char str[100]={0};
      strcat(str,"/home/parambedi/Desktop/index/");
      strcat(str,indexarr);
      strcat(str,".txt");
      extract_file(str);

  }*/
   int i=1,index;
    while(1)
    {
        page=i;
        index = i;
        char indexarr[5]={0};
        sprintf(indexarr,"%d",index);
        char str[100]={0};
        strcat(str,"/home/parambedi/search_engine/");
        strcat(str,indexarr);
        strcat(str,".txt");
        FILE *file = fopen(str,"r");
        if(file)
        {
             extract_file(str);
        }
        else
        {
            break;
        }
        i++;
    }
    //print();




    /********QUERY CODE************************/
    char query[250]={0};
    printf("Enter your Query To be Searched = \n");
    scanf("%[^\n]s", query);
    char subq[50]={0};
    int k=0,key,countword=0;
    for(int j=0;query[j]!='\0';j++)
    {
        if(query[j]==' ')
        {
            subq[k]='\0';
            /*if(helping word then discard)
            else (count it)*/
            countword++;
            key = hash_func(subq);
            query_list(subq,key);
            k=0;
        }
        else
        {
            subq[k++]=query[j];
        }
    }
    if(k!=0)
    {
        subq[k]='\0';
        /*if(helping word then discard)
        else (count it)*/
        countword++;
        key = hash_func(subq);
        query_list(subq,key);
        k=0;
    }
    printf("%d\n",countword);

    querynode *I,*J;
    int ltemp,totaltemp,rtemp;
    char utemp[150];
    for(I = qhead;I->querynext!=NULL;I=I->querynext)
    {
        for(J = I->querynext;J!=NULL;J=J->querynext)
        {
            /*if((I->rank*I->totalcount)<=(J->rank*J->totalcount))
            {
                ltemp = I->linkno;
                I->linkno = J->linkno;
                J->linkno = ltemp;

                strcpy(utemp,I->link);
                strcpy(I->link,J->link);
                strcpy(J->link,utemp);

                totaltemp = I->totalcount;
                I->totalcount = J->totalcount;
                J->totalcount = totaltemp;

                rtemp = I->rank;
                I->rank = J->rank;
                J->rank = rtemp;
            }*/
            if(I->rank<J->rank)
            {
                ltemp = I->linkno;
                I->linkno = J->linkno;
                J->linkno = ltemp;

                strcpy(utemp,I->link);
                strcpy(I->link,J->link);
                strcpy(J->link,utemp);

                totaltemp = I->totalcount;
                I->totalcount = J->totalcount;
                J->totalcount = totaltemp;

                rtemp = I->rank;
                I->rank = J->rank;
                J->rank = rtemp;
            }
            else if(I->rank==J->rank)
            {
                if((I->rank*I->totalcount)<=(J->rank*J->totalcount))
                {
                    ltemp = I->linkno;
                    I->linkno = J->linkno;
                    J->linkno = ltemp;

                    strcpy(utemp,I->link);
                    strcpy(I->link,J->link);
                    strcpy(J->link,utemp);

                    totaltemp = I->totalcount;
                    I->totalcount = J->totalcount;
                    J->totalcount = totaltemp;

                    rtemp = I->rank;
                    I->rank = J->rank;
                    J->rank = rtemp;
                }
            }

        }
    }
    print_query_list();

}
/*int count_word(char *start,char *end,char *word)
{
    char *temp;
    temp=start;
    int wordlen = strlen(word);
    int c=0,found =0;
    for(;temp!=end;temp++)
    {
      found=1;
      for(int j=0;j<wordlen;j++)
      {
        //printf("\nIN WORD LOOP\n");
        if(*(temp+j)!=word[j])
        {
          //printf("%c ",*(tstart+j));
          found=0;
          break;
        }
      }
      if(found==1)
      {
          c++;
          //printf("%s %d\n",word,c);
      }
    }
    printf("\n%d\n",c);
}*/
