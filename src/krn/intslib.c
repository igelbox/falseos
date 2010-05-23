/*
  intslib.c
*/

#define IDT_TABLE 0x100000
#define IDT_REG 0x100800
#define SYS_CODE_SELECTOR 0x8

//Функция i_install() устанавливает в качестве обработчика vector функцию func.
//Тип шлюза (прерывания [0x8e] или ловушки [0x8f]) указывается параметром type.

//Фактически, эта функция создает (или изменяет) соответствующий дескриптор в таблице IDT

void i_install(unsigned char vector, void (*func)(), unsigned char type)
{
  char * idt_table=(char*)IDT_TABLE;

  unsigned char i;
  unsigned char b[8];

  b[0]=  (unsigned int)func & 0x000000FF;
  b[1]=( (unsigned int)func & 0x0000FF00) >> 8;
  b[2]=SYS_CODE_SELECTOR;
  b[3]=0;
  b[4]=0;
  b[5]=type;
  b[6]=( (unsigned int)func & 0x00FF0000) >> 16;
  b[7]=( (unsigned int)func & 0xFF000000) >> 24;



  for(i=0;i<8;i++){
    *(idt_table+vector*8+i)=b[i];
  }


}

//Функция i_setup() загружает регистр IDTR
void i_setup()
{
  unsigned short *table_limit = (unsigned short*)IDT_REG;
  unsigned int *table_address = (unsigned int*)(IDT_REG+2);

  *table_limit = 256*8 - 1;
  *table_address = IDT_TABLE;

  asm("lidt 0(,%0,)"::"a"(IDT_REG));
  asm("sti");
}


//Включение обработки прерываний
void i_enable()
{
  asm("sti");
}

//Отключение обработки прерываний
void i_disable()
{
  asm("cli");
}
