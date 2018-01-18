/*
Были обнаружены ошибки в функциях FIFO_IS_FULL, FIFO_SPACE и др, т.к. индекс реально не корректно переходил через 
границу 256. В любом случае в буфере на 8 есть только 7 мест! А в базовых функциях - 8!
В общем я переделал, как мне кажется правильным.
*/
#ifndef FIFO1__H
#define FIFO1__H
 
//размер должен быть степенью двойки: 4,8,16,32...128
#define FIFO( size )\
  struct {\
    unsigned char buf[size];\
    unsigned int tail;\
    unsigned int head;\
  } 
 
//fifo заполнено?
#define FIFO_IS_FULL(fifo)   (((fifo.head-fifo.tail) & (sizeof(fifo.buf)-1)) == (sizeof(fifo.buf)-1))
 
//fifo пусто?
#define FIFO_IS_EMPTY(fifo)  (fifo.tail==fifo.head)
 
//количество элементов в очереди
#define FIFO_COUNT(fifo)     ((fifo.head-fifo.tail) & (sizeof(fifo.buf)-1))
 
//размер fifo
#define FIFO_SIZE(fifo)      ( (sizeof(fifo.buf)))
 
//количество свободного места в fifo
#define FIFO_SPACE(fifo)     (FIFO_SIZE(fifo)-FIFO_COUNT(fifo) -1)
 
//поместить элемент в fifo
#define FIFO_PUSH(fifo, byte) \
  {\
    fifo.buf[fifo.head]=byte;\
    fifo.head++;\
	fifo.head &= (sizeof(fifo.buf)-1);\
  }

// в очередь не помещается элемент, который там уже есть!
/*
#define FIFO_PUSH(fifo, byte) \
  {\
  	char flag=0;\
	char i=fifo.tail; \
	while(i != fifo.head){ \
		if(fifo.buf[i]==byte)\
		{\
			flag=1;\
			break;\
		}\
		i++; \
		i &= (sizeof(fifo.buf)-1);\
	}\
	if(!flag)\
	{\
	    fifo.buf[fifo.head]=byte;\
	    fifo.head++;\
		fifo.head &= (sizeof(fifo.buf)-1);\
	}\
  }
*/
 
//взять первый элемент из fifo
#define FIFO_FRONT(fifo) (fifo.buf[(fifo).tail])
 
//уменьшить количество элементов в очереди
#define FIFO_POP(fifo)   \
  {\
      fifo.tail++; \
	  fifo.tail &= (sizeof(fifo.buf)-1);\
  }
 
//очистить fifo
#define FIFO_FLUSH(fifo)   \
  {\
    fifo.tail=0;\
    fifo.head=0;\
  } 
 
#endif //FIFO__H 

/*
#ifndef FIFO__H
#define FIFO__H
 
//размер должен быть степенью двойки: 4,8,16,32...128
#define FIFO( size )\
  struct {\
    unsigned char buf[size];\
    unsigned char tail;\
    unsigned char head;\
  } 
 
//fifo заполнено?
#define FIFO_IS_FULL(fifo)   ((fifo.head-fifo.tail)==sizeof(fifo.buf))
 
//fifo пусто?
#define FIFO_IS_EMPTY(fifo)  (fifo.tail==fifo.head)
 
//количество элементов в очереди
#define FIFO_COUNT(fifo)     (fifo.head-fifo.tail)
 
//размер fifo
#define FIFO_SIZE(fifo)      ( (sizeof(fifo.buf)))
 
//количество свободного места в fifo
#define FIFO_SPACE(fifo)     (FIFO_SIZE(fifo)-FIFO_COUNT(fifo))
 
//поместить элемент в fifo
#define FIFO_PUSH(fifo, byte) \
  {\
    fifo.buf[fifo.head & (sizeof(fifo.buf)-1)]=byte;\
    fifo.head++;\
  }
 
//взять первый элемент из fifo
#define FIFO_FRONT(fifo) (fifo.buf[(fifo).tail & (sizeof(fifo.buf)-1)])
 
//уменьшить количество элементов в очереди
#define FIFO_POP(fifo)   \
  {\
      fifo.tail++; \
  }
 
//очистить fifo
#define FIFO_FLUSH(fifo)   \
  {\
    fifo.tail=0;\
    fifo.head=0;\
  } 
 
#endif //FIFO1__H 

*/
