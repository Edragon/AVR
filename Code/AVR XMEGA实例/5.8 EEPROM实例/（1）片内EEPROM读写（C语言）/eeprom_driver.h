#ifndef EEPROM_DRIVER_H
#define EEPROM_DRIVER_H

#include "avr_compiler.h"

//#define MAPPED_EEPROM_START 0x1000EEPROM�洢��ӳ���16λ��ʼ��ַΪ0x1000
#define EEPROM_PAGESIZE 32
#define EEPROM(_pageAddr, _byteAddr) \
	((uint8_t *) MAPPED_EEPROM_START)[_pageAddr*EEPROM_PAGESIZE + _byteAddr]

//�궨��

/*! \ʹ��EEPROM���ļ���ģʽ

 *  ������Ҫ6��CPUʱ��
 */
#define EEPROM_EnablePowerReduction() ( NVM.CTRLB |= NVM_EPRM_bm )

/*! \�ر�EEPROM���ļ���ģʽ
 */
#define EEPROM_DisablePowerReduction() ( NVM.CTRLB &= ~NVM_EPRM_bm )

/*! \ʹ��EEPROMӳ�䵽�ڴ�ռ�
 *
 *  EEPROMӳ�䵽�ڴ�ռ�
 *  ��ʼ��ַΪEEPROM_START = 0x1000,������ڴ�һ������
 *
 *  \ע�� ҳ������ҳд��������Ҫͨ��IO�Ĵ�����ִ��
 */
#define EEPROM_EnableMapping() ( NVM.CTRLB |= NVM_EEMAPEN_bm )

/*! \�ر�EEPROMӳ�䵽�ڴ�ռ�
 */
#define EEPROM_DisableMapping() ( NVM.CTRLB &= ~NVM_EEMAPEN_bm )

/*! \����ʧ�Դ洢������ִ������
 *
 *  \ע�� CMDEX �����ڽ��CCP�������4��ʱ����������λ
 */
#define NVM_EXEC()	asm("push r30"      "\n\t"	\
			    "push r31"      "\n\t"	\
    			    "push r16"      "\n\t"	\
    			    "push r18"      "\n\t"	\
			    "ldi r30, 0xCB" "\n\t"	\
			    "ldi r31, 0x01" "\n\t"	\
			    "ldi r16, 0xD8" "\n\t"	\
			    "ldi r18, 0x01" "\n\t"	\
			    "out 0x34, r16" "\n\t"	\
			    "st Z, r18"	    "\n\t"	\
    			    "pop r18"       "\n\t"	\
			    "pop r16"       "\n\t"	\
			    "pop r31"       "\n\t"	\
			    "pop r30"       "\n\t"	\
			    )

//��������
void EEPROM_WriteByte( uint8_t pageAddr, uint8_t byteAddr, uint8_t value );
uint8_t EEPROM_ReadByte( uint8_t pageAddr, uint8_t byteAddr );
void EEPROM_WaitForNVM( void );
void EEPROM_FlushBuffer( void );
void EEPROM_LoadByte( uint8_t byteAddr, uint8_t value );
void EEPROM_LoadPage( const uint8_t * values );
void EEPROM_AtomicWritePage( uint8_t pageAddr );
void EEPROM_ErasePage( uint8_t pageAddress );
void EEPROM_SplitWritePage( uint8_t pageAddr );
void EEPROM_EraseAll( void );

#endif
