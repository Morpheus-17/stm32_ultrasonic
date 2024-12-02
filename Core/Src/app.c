/*
 * app.c
 *
 *  Created on: Nov 25, 2024
 *      Author: blueb
 */

#include "app.h"
#include "uart.h"
#include "delay.h"

// 장치 재선언
extern TIM_HandleTypeDef htim3;		// echo 파형의 길이 측정
extern TIM_HandleTypeDef htim11;	// us 지연함수
extern UART_HandleTypeDef huart2;

// 전역 변수
uint32_t inputCapture_Val1 	= 0; 	// Rising Edge  위치 저장
uint32_t inputCapture_Val2 	= 0;	// Falling Edge 위치 저장
uint32_t difference 				= 0;	// Rising Edge - Falling Edge
uint8_t  is_first_captured 	= 0; 	// 현재 캡처 단계
uint8_t distance 						= 0;	// 계산된 거리 (cm), (센서의 성능상 200cm 이상은 나오지도 않아 1byte로 선택했다)

// 인터럽트 서비스 루틴(ISR)
// Global Interrup Enable 하면 여기에 대응하는 함수를 만들어줘야 한다 (오타가 있어서는 안된다 : stm32f4xx_it.h)
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	// 인터럽트가 발생된 타이머의 채널 확인
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
		// 현재 캡처 상승 단계
		if(is_first_captured == 0){ // Rising Edge
			// 타이머에서 Rising Edge 에서 캡처된 카운트 값을 저장해준다
			inputCapture_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			// 다음 인터럽트 발생을 위한 설정
			is_first_captured = 1;
			// 타이머가 기본적으로 Rising Edge 에서동작하도록 되어 있기 때문에 Falling Edge 에서 인터럽트가 발생하도록 타이머 설정을 변경한다
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);

		}
		else if(is_first_captured == 1) {// Falling Edge
			// 타이머에서 Falling Edge 에서 캡처된 카운트 값을 저장해준다
			inputCapture_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);


			//거리 계산 (소요 시간)
			distance = difference * 0.034 / 2;


			// 다음 인터럽트 발생을 위한 설정
			is_first_captured = 0;
			// 타이머가 기본적으로 Falling Edge 에서동작하도록 되어 있기 때문에 Rising Edge 에서 인터럽트가 발생하도록 타이머 설정을 변경한다
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_CC2);
		}
	}
}

uint32_t getDistance(){


//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); 아래와 동일
	HAL_GPIO_WritePin(Trigger_GPIO_Port, Trigger_Pin, GPIO_PIN_SET);
	delayUS(10);
	HAL_GPIO_WritePin(Trigger_GPIO_Port, Trigger_Pin, GPIO_PIN_SET);

	// 인터럽트 설정
	__HAL_TIM_ENABLE_IT(&htim3,TIM_IT_CC2);


	return distance;
}

void app(){


	// Uart 장치 초기화
	initUart(&huart2);

	// Timer3 시작 (Input Capture 모드 Interrupt 방식으로 사용된다)
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
	// Timer11 시작 (Timer 가 기본 기능 카운트로만 사용된다)
	HAL_TIM_Base_Start(&htim11);

	while(1){
		printf("distance = %d\n", getDistance());
		HAL_Delay(100);
	}



}
;
