#include "main.h"

#define RFID_FLAG   0    // �Ƿ�ʹ��rfid
#define IWDG_FLAG   1   // �������Ź�

/*  ���մ��������߳�  */
void Reve_thread_Entry(void *pargem);

/*  �ɼ������߳�  */
void collect_data_thread_entry(void *pargem);



/* ���������߳� */
void key_thread_entry(void *pargem);

/*  �������߳�  */
void beep_thread_entry(void *pargem);

/* ��ʾlcd�� */
void lcd_show_thread_entry(void *pargem);

/* �߼��ж��߳�  */
void logical_thread_entry(void *pargem);


#if RFID_FLAG
// ��rfid
void read_rfid_thread_entry(void *pargem);
#endif


/* ���������߳� */
void Send_data_Entry(void *pargem);
