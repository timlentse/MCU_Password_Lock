#include<reg52.h> 
/*����������Ҫ�˽�����ܵ���ʾԭ��*/
sbit duan_select=P2^2; //��������ʹ�ܶ˿� �����棨��������ʾʲô���֣�
sbit wei_select=P2^3; //  λ���棨��������һλ��ʾ��
sbit led_wrong=P2^0;
sbit led_right=P2^1;
sbit sound=P2^4;

#define datasend P0 //�������ݶ˿�������P0������� ����������datasend ����P0�滻
#define Matrix_key P1	//������̵�״ֱ̬�������P1��

 // ����һ�����飬��ʾ����ֵ0~9���Է�����ʱ���ã���code'��˼�����������ROM��
unsigned char code display_value[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
 //display_place ��λ�����飬0xfe����ʹ��һλ���������0xfd����ʹ�ڶ�λ���������������� 
unsigned char code display_place[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};

unsigned char digital_display[8],temp[8],new_password[6]; //����Ķ���
unsigned char password[6]={1,1,1,1,1,1};//�����ֵ
unsigned char key_value,i,j,k,count,m,n,l,freq;                  
bit sign=0,detect=0,siv=0;	//bitΪ�����������ݣ���ֵΪ0��1������ʾ���٣�

//�������Ӻ��������� 
void change_password();
void input_password();
void Delay(unsigned char t); //��ʱ���������Ǿ�ȷ����ʱ
void Display(unsigned char num);//�������ʾ����
void Init_Timer(void);	 //��ʱ��0������
void baojing();
/*------------------------------------------------
��Ȼ������Ҫʹ�þ�����̣��Ǿ�ǣ�浽������̵Ķ�ȡ
���º����Ǽ�⾿�����ĸ���������
����ɨ�躯��������ɨ���ֵ
------------------------------------------------*/
unsigned char keyscan(void)  //����ɨ�躯����ʹ�����з�תɨ�跨
{
unsigned char row,column;//����ֵ�м����
Matrix_key=0x0f;            //�������ȫΪ0
row= Matrix_key&0x0f;     //��������ֵ
 if(row!=0x0f)    //�ȼ�����ް�������
 {
  Delay(10);        //ȥ��
  if(( Matrix_key&0x0f)!=0x0f)
  {
    row= Matrix_key&0x0f;  //��������ֵ
    Matrix_key=row|0xf0;  //�����ǰ����ֵ
    column= Matrix_key&0xf0;  //��������ֵ

    while(( Matrix_key&0xf0)!=0xf0);//�ȴ��ɿ������

    return(row+column);//������������ֵ
   }
  }return(0xff);     //���ظ�ֵ
}
/*------------------------------------------------
����ֵ������������ɨ��ֵ
------------------------------------------------*/
unsigned char decode(void)
{
 switch(keyscan())
 {
  case 0x7e:return 0;break;//0 ������Ӧ�ļ���ʾ���Ӧ����ֵ
  case 0x7d:return 1;break;//1
  case 0x7b:return 2;break;//2
  case 0x77:return 3;break;//3
  case 0xbe:return 4;break;//4
  case 0xbd:return 5;break;//5
  case 0xbb:return 6;break;//6
  case 0xb7:return 7;break;//7
  case 0xde:return 8;break;//8
  case 0xdd:return 9;break;//9
  case 0xdb:return 't';break;//���ء�t'��������������λ��
  case 0xd7:return 'o';break;//���ء�o'������������ȷ�ϼ�
  case 0xee:return 'c';break;//���ء�c'�������������޸������
  default:return 0xff;break;
 }
}
void clear_screen()	//�����ӳ���
{ for(j=0;j<8;j++)
  digital_display[j]=0;}    
/*------------------------------------------------
                    ������
------------------------------------------------*/
void main ()
{
Init_Timer();
n=0;   //��¼�����������Ĵ���
m=6;   //λ������ʼֵ
while (1)         //��ѭ��
{ Display(8);
baojing();
key_value=decode();	 //��ȡ��������ֵ
if (key_value!=0xff	)
  {	  
  TR0=0;  TR1=0;      //��ʱ�����عر�	
   led_wrong=1;
   led_right=1;
 if ((key_value=='c')&&(n<3))   //��key_value=c��˵�������޸�����������Ҫ������ȷ����������޸�����;  
 { clear_screen();
   digital_display[0]=0x30;
   digital_display[1]=0x37;
   digital_display[2]=0x73;
   digital_display[3]=0x3E;
   digital_display[4]=0x78;
   detect=1;  }

else if((siv)&&(detect))
 {     
 change_password();
 }
//û���޸��������ִ�����³���	
else
  {  
if (n<3)
{
 input_password();
 if(key_value=='o')
   {
      if(sign)
	  //����Ƚ�ȫ����ͬ����־λ��1
	   { sign=0;   //��ʱ����־λ���㣬��ֹ�жϴ���
	      n=0;
		  led_right=0;
		  if(detect)
		  {siv=1; }
	     digital_display[2]=0x5E; 
         digital_display[3]=0x77; 
		 digital_display[4]=0x54;
		 digital_display[5]=0x54;
		 		}
	  else 
	    { n++;
	     led_wrong=0;
		 l=0; 	
		 TR0=1;
		 TR1=1;        //��ʱ�����ش�			 
		 digital_display[0]=0x6D;
         digital_display[1]=0x7C;
			}
	   }
	  }
	}
   }
  }
 }
 /*������ʱ������ԭ������������*/

void Delay(unsigned char t)
{
 unsigned int x ;  
 while(t--)
 {
   for (x=0;x<500;x++)	;
 }
}
/*------------------------------------------------
 ��ʾ���������ڶ�̬ɨ�������
------------------------------------------------*/
void Display(unsigned char para)
{	 static unsigned char s=0;
       datasend=0;   //������ݣ���ֹ�н�����Ӱ
	   duan_select=1;     //��ѡ
       duan_select=0;

	   datasend=display_place[s]; 
	   wei_select=1;		//λѡ 
       wei_select=0;

	   datasend=digital_display[s]; //ȡ��ʾ���ݣ�����
	   duan_select=1;     //��ѡ
       duan_select=0;  
	   s++;
       if(s==para)
	   {   s=0;	}
}
void input_password()
  {
   if(i==0)
    {
   clear_screen();
	}
	//��ɾ����
	if ((key_value=='t')&&(i>=1))
	{ 	
	 i--;
	 for (j=7;j>=1;j--)
	 {
	  digital_display[j]=digital_display[j-1];
	  }
	}	
 if((i<6)&&(key_value!='o')&&(key_value!='t'))
     { temp[i]=key_value;
	 for(j=0;j<=i;j++)  //ͨ��һ��˳��Ѱ����������е�ֵ��ֵ����ʾ��������������������             
      digital_display[7-i+j]=display_value[temp[j]]; 
	 i++;		   //������ֵ�ۼ�
   }
 	   	   
	if(key_value=='o') 	 	//key_value=��o'˵��������ȷ�ϼ�����ִ���ж��������ȷ��		
 {
	  count=i;		  //iΪ��������ĸ���
	  i=0;
	  if (count==m)
  {
	   count=0;
	   sign=1;       //�ȰѱȽ�λ��1
	   for(j=0;j<m;j++)//ѭ���Ƚ�m����ֵ�������һ������� �����ձ�־λsignֵΪ0				      
	   sign=sign&&(temp[j]==password[j]);//�Ƚ�����ֵ�����е�����	 
		  	  }
	  clear_screen();
  }
}
void change_password()
{
if((key_value!='o')&&(key_value!='c'))  //ȷ������ֵΪ���ּ�������λ��
 {  	 
   if(k==0)
       {
      clear_screen();  }
  //����λ��ִ�������ɾ
	if ((key_value=='t')&&(k>=1))
	    {   
	    k--;
		for (j=7;j>=1;j--)
	 {
	    digital_display[j]=digital_display[j-1]; 
		    }
	} 	 
		//�������һλ����ֵ
	else if(key_value!='t')
		{
		if (k<6) //���ֻ��������������
		{ 
		new_password[k]=key_value;	//������ֵ����password
		for(j=0;j<=k;j++)         //ͨ��һ��˳�����ʱ������
	                  //��ֵ��ֵ����ʾ��������������������
        digital_display[7-k+j]=display_value[password[j]]; 
		  k++; //����λ����1
	   }	
	}
 }
   if (key_value=='o')	   //����ȷ�ϼ������޸ĳɹ�
		{siv=0;	  //��ʾ�޸��������
		 detect=0;	 
		 digital_display[0]=0x76; 	 //��ʾH
		if (k!=0)
		{  m=k;   //m�洢�޸������λ��
		   k=0;   //��������      
	   for (j=0;j<6;j++)
	   password[j]=new_password[j];    }
		}
		}
/*------------------------------------------------
                    ��ʱ����ʼ���ӳ���
------------------------------------------------*/
void Init_Timer(void)
{
TMOD |= 0x11;	  //ʹ��ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ��		     
EA=1;            //���жϴ�
ET0=1;           //��ʱ���жϴ�
ET1=1;
}
void baojing(void)
 { Delay(1);
  freq++;
  }
/*------------------------------------------------
                 ��ʱ��0�ж��ӳ���
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{ 
 static unsigned char inner_count;
 TH0=(65536-2000)/256;	
 TL0=(65536-2000)%256;
 inner_count++;
 if (inner_count==150)
 {  inner_count=0;
  l++;
    if (l<7)
	{ for (j=7;j>=1;j--)
	digital_display[j]=digital_display[j-1];
	digital_display[l-1]=0;
	   }
else if (l>=7)
{   for (j=0;j<7;j++)
	digital_display[j]=digital_display[j+1];
	digital_display[14-l]=0;
	if (l==12)
	   l=0;  }
   }
} 
void Timer1_isr(void) interrupt 3
{   TH1=0xfe;
    TL1=freq; 
	sound=!sound; }
 