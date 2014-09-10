#include<reg52.h> 
/*以下两处需要了解数码管的显示原理*/
sbit duan_select=P2^2; //定义锁存使能端口 段锁存（即控制显示什么数字）
sbit wei_select=P2^3; //  位锁存（即控制哪一位显示）
sbit led_wrong=P2^0;
sbit led_right=P2^1;
sbit sound=P2^4;

#define datasend P0 //定义数据端口数据由P0口输出， 程序中遇到datasend 则用P0替换
#define Matrix_key P1	//矩阵键盘的状态直接输出到P1口

 // 定义一个数组，显示段码值0~9，以方便随时调用，‘code'意思即将数组存在ROM中
unsigned char code display_value[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
 //display_place 即位码数组，0xfe就是使第一位数码管亮，0xfd就是使第二位数码管亮，如此类推 
unsigned char code display_place[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};

unsigned char digital_display[8],temp[8],new_password[6]; //数组的定义
unsigned char password[6]={1,1,1,1,1,1};//密码初值
unsigned char key_value,i,j,k,count,m,n,l,freq;                  
bit sign=0,detect=0,siv=0;	//bit为布尔类型数据，其值为0或1（即表示真或假）

//以下是子函数的声明 
void change_password();
void input_password();
void Delay(unsigned char t); //延时函数，不是精确的延时
void Display(unsigned char num);//数码管显示函数
void Init_Timer(void);	 //定时器0的声明
void baojing();
/*------------------------------------------------
当然我们需要使用矩阵键盘，那就牵涉到矩阵键盘的读取
以下函数是检测究竟是哪个键被按下
按键扫描函数，返回扫描键值
------------------------------------------------*/
unsigned char keyscan(void)  //键盘扫描函数，使用行列反转扫描法
{
unsigned char row,column;//行列值中间变量
Matrix_key=0x0f;            //行线输出全为0
row= Matrix_key&0x0f;     //读入列线值
 if(row!=0x0f)    //先检测有无按键按下
 {
  Delay(10);        //去抖
  if(( Matrix_key&0x0f)!=0x0f)
  {
    row= Matrix_key&0x0f;  //读入列线值
    Matrix_key=row|0xf0;  //输出当前列线值
    column= Matrix_key&0xf0;  //读入行线值

    while(( Matrix_key&0xf0)!=0xf0);//等待松开并输出

    return(row+column);//键盘最后组合码值
   }
  }return(0xff);     //返回该值
}
/*------------------------------------------------
按键值处理函数，返回扫键值
------------------------------------------------*/
unsigned char decode(void)
{
 switch(keyscan())
 {
  case 0x7e:return 0;break;//0 按下相应的键显示相对应的码值
  case 0x7d:return 1;break;//1
  case 0x7b:return 2;break;//2
  case 0x77:return 3;break;//3
  case 0xbe:return 4;break;//4
  case 0xbd:return 5;break;//5
  case 0xbb:return 6;break;//6
  case 0xb7:return 7;break;//7
  case 0xde:return 8;break;//8
  case 0xdd:return 9;break;//9
  case 0xdb:return 't';break;//返回‘t'即表明按下了退位键
  case 0xd7:return 'o';break;//返回‘o'即表明按下了确认键
  case 0xee:return 'c';break;//返回‘c'即表明按下了修改密码键
  default:return 0xff;break;
 }
}
void clear_screen()	//清屏子程序
{ for(j=0;j<8;j++)
  digital_display[j]=0;}    
/*------------------------------------------------
                    主函数
------------------------------------------------*/
void main ()
{
Init_Timer();
n=0;   //记录输入错误密码的次数
m=6;   //位数赋初始值
while (1)         //主循环
{ Display(8);
baojing();
key_value=decode();	 //读取按键输入值
if (key_value!=0xff	)
  {	  
  TR0=0;  TR1=0;      //定时器开关关闭	
   led_wrong=1;
   led_right=1;
 if ((key_value=='c')&&(n<3))   //若key_value=c则说明发生修改密码请求，需要输入正确的密码才能修改密码;  
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
//没按修改密码键则执行以下程序	
else
  {  
if (n<3)
{
 input_password();
 if(key_value=='o')
   {
      if(sign)
	  //如果比较全部相同，标志位置1
	   { sign=0;   //及时将标志位置零，防止判断错误
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
		 TR1=1;        //定时器开关打开			 
		 digital_display[0]=0x6D;
         digital_display[1]=0x7C;
			}
	   }
	  }
	}
   }
  }
 }
 /*设置延时函数的原因是用来防抖*/

void Delay(unsigned char t)
{
 unsigned int x ;  
 while(t--)
 {
   for (x=0;x<500;x++)	;
 }
}
/*------------------------------------------------
 显示函数，用于动态扫描数码管
------------------------------------------------*/
void Display(unsigned char para)
{	 static unsigned char s=0;
       datasend=0;   //清空数据，防止有交替重影
	   duan_select=1;     //段选
       duan_select=0;

	   datasend=display_place[s]; 
	   wei_select=1;		//位选 
       wei_select=0;

	   datasend=digital_display[s]; //取显示数据，段码
	   duan_select=1;     //段选
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
	//回删功能
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
	 for(j=0;j<=i;j++)  //通过一定顺序把把密码数组中的值赋值到显示缓冲区，从右往左输入             
      digital_display[7-i+j]=display_value[temp[j]]; 
	 i++;		   //输入数值累加
   }
 	   	   
	if(key_value=='o') 	 	//key_value=‘o'说明按下了确认键，则执行判断密码的正确性		
 {
	  count=i;		  //i为输入密码的个数
	  i=0;
	  if (count==m)
  {
	   count=0;
	   sign=1;       //先把比较位置1
	   for(j=0;j<m;j++)//循环比较m个数值，如果有一个不相等 则最终标志位sign值为0				      
	   sign=sign&&(temp[j]==password[j]);//比较输入值和已有的密码	 
		  	  }
	  clear_screen();
  }
}
void change_password()
{
if((key_value!='o')&&(key_value!='c'))  //确保按键值为数字键或者退位键
 {  	 
   if(k==0)
       {
      clear_screen();  }
  //按退位键执行密码回删
	if ((key_value=='t')&&(k>=1))
	    {   
	    k--;
		for (j=7;j>=1;j--)
	 {
	    digital_display[j]=digital_display[j-1]; 
		    }
	} 	 
		//否则存入一位密码值
	else if(key_value!='t')
		{
		if (k<6) //最多只能输入六个密码
		{ 
		new_password[k]=key_value;	//将按键值赋到password
		for(j=0;j<=k;j++)         //通过一定顺序把临时数组中
	                  //的值赋值到显示缓冲区，从右往左输入
        digital_display[7-k+j]=display_value[password[j]]; 
		  k++; //密码位数加1
	   }	
	}
 }
   if (key_value=='o')	   //按下确认键密码修改成功
		{siv=0;	  //表示修改密码完毕
		 detect=0;	 
		 digital_display[0]=0x76; 	 //显示H
		if (k!=0)
		{  m=k;   //m存储修改密码的位数
		   k=0;   //索引清零      
	   for (j=0;j<6;j++)
	   password[j]=new_password[j];    }
		}
		}
/*------------------------------------------------
                    定时器初始化子程序
------------------------------------------------*/
void Init_Timer(void)
{
TMOD |= 0x11;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
EA=1;            //总中断打开
ET0=1;           //定时器中断打开
ET1=1;
}
void baojing(void)
 { Delay(1);
  freq++;
  }
/*------------------------------------------------
                 定时器0中断子程序
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
 