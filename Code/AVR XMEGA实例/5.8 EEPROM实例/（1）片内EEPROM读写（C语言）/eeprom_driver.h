#ifndef EEPROM_DRIVER_H
#define EEPROM_DRIVER_H

#include "avr_compiler.h"

//#define MAPPED_EEPROM_START 0x1000EEPROM存储器映射的16位起始地址为0x1000
#define EEPROM_PAGESIZE 32
#define EEPROM(_pageAddr, _byteAddr) \
	((uint8_t *) MAPPED_EEPROM_START)[_pageAddr*EEPROM_PAGESIZE + _byteAddr]

//宏定义

/*! \使能EEPROM功耗减少模式

 *  唤醒需要6个CPU时钟
 */
#define EEPROM_EnablePowerReduction() ( NVM.CTRLB |= NVM_EPRM_bm )

/*! \关闭EEPROM功耗减少模式
 */
#define EEPROM_DisablePowerReduction() ( NVM.CTRLB &= ~NVM_EPRM_bm )

/*! \使能EEPROM映射到内存空间
 *
 *  EEPROM映射到内存空间
 *  起始地址为EEPROM_START = 0x1000,像访问内存一样访问
 *
 *  \注意 页擦除和页写操作仍需要通过IO寄存器来执行
 */
#define EEPROM_EnableMapping() ( NVM.CTRLB |= NVM_EEMAPEN_bm )

/*! \关闭EEPROM映射到内存空间
 */
#define EEPROM_DisableMapping() ( NVM.CTRLB &= ~NVM_EEMAPEN_bm )

/*! \非易失性存储控制器执行命令
 *
 *  \注意 CMDEX 必须在解除CCP保护后的4个时钟周期内置位
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

//函数声明
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
