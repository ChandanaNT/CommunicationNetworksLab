#include<string.h>
#include<stdio.h>

int i,j,len;
char gen[]="10001000000100001";
char data[50],crc[50];

void calc_crc()
{
  for(i=0;i<strlen(gen);++i)
      crc[i]=data[i];

  do
  {

   if(crc[0]=='1')
   {
      for(j=1;j<strlen(gen);++j)
      {
         crc[j]=((crc[j]==gen[j])?'0':'1');
      }
   }

   for(j=0;j<strlen(gen)-1;++j)
      crc[j]=crc[j+1];

   crc[j]=data[i++];
      
  }while(i<=len+strlen(gen)-1); 
}

int main()
{

 printf("\n Enter data you want ");
 scanf("%s",data);

 len=strlen(data);
 for(i=len;i<len+strlen(gen)-1;++i)
 {
     data[i]='0';
 }
 printf("\n Modified data word is %s",data);

 calc_crc();
 for(i=len;i<len+strlen(gen)-1;++i)
 {
     data[i]=crc[i-len];
 }
 printf("\n Final codeword is %s",data);

 printf("\n Insert errors(1:Yes)(0:No)");
 scanf("%d",&i);
 if(i==1)
 {
    printf("\n Enter position to insert error");
    scanf("%d",&j);
    data[j]=((data[j]=='0')?'1':'0');
    calc_crc();
    for(j=0;j<(strlen(gen)-1) && crc[j]!='1';++j);
    if(j<strlen(gen)-1)
      printf("\n Error Detected !!! ");
    else
     printf("\n NO error was detected! ");
 }
 printf("\n");
 return 0;
}

