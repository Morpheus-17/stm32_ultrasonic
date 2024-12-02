/*
 * delay.c
 *
 *  Created on: Dec 2, 2024
 *      Author: blueb
 */

#include "main.h"
#include "delay.h"

// 장치 재선언
extern TIM_HandleTypeDef htim3;		// echo 파형의 길이 측정
extern TIM_HandleTypeDef htim11;	// us 지연함수
extern UART_HandleTypeDef huart2;

//16bit Timer
void delayUS(uint16_t us){

	htim11.Instance->CNT = 0;

	// Up-Counter 의 값이 us와 같으면 종료
	while(htim11.Instance->CNT < us);



}

