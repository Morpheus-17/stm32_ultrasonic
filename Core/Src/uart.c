/*
 * uart.c
 *
 *  Created on: Nov 19, 2024
 *      Author: blueb
 */

#include "uart.h"

extern UART_HandleTypeDef huart2;

UART_HandleTypeDef* myHuart; 		// Uart Handler
uint8_t rxChar; 								// receive character
#define rxBufferMax 255
int rxBufferWrite;							// write receivebuffer pointer
int rxBufferRead;								// read receivebuffer pointer
uint8_t rxBuffer[rxBufferMax];	// receivebuffer

int _write(int file, char* p, int len){
	HAL_UART_Transmit(myHuart, (uint8_t*)p, len, 10);
	return len;
}


//initialize uart device
void initUart(UART_HandleTypeDef* inHuart){
	//initialize uart device name
	myHuart = inHuart;

	//receive interrupt setting
	HAL_UART_Receive_IT(myHuart, &rxChar, 1);
	rxBufferRead = rxBufferWrite = 0;
}

// 지정되어 있는 함수로 스펠링도 틀리면 안된다
// 문자수신 처리 함수 구현
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart){
	// 수신된 문자를 버퍼에 저장하고 쓰기 포인터의 값을 1 증가
	rxBuffer[rxBufferWrite++] = rxChar;
	// 쓰기 포인터의 값이 최대치에 도달하면 다시 0으로 초기화
	rxBufferWrite %= rxBufferMax;
	// 다음 문자 수신 인터럽트를 위하여 재설정 (아래를 선언하여 재설정을 안해주면 한번만 되고 그다음 버퍼 저장을 안해버림)
	HAL_UART_Receive_IT(myHuart, &rxChar, 1);
}

// 버퍼에서 문자 한개 꺼내오기
uint8_t getUart(){
	uint8_t result;
	// 쓰기 포인터와 읽기 포인터가 없으면 수신된 문자 없음
	if(rxBufferWrite == rxBufferRead) return 0;
	else {
		//읽기 포인터가 가르키는 위치의 버퍼문자를 꺼내고, 읽기 포인터를 1 증가
		result = rxBuffer[rxBufferRead++];
		//읽기 포인터가 최대치에 도달하면 다시 0으로 초기화
		rxBufferRead %= rxBufferMax;
	}
	return result;
}
