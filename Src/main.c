/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE END Includes */

/* Function processing DMA Rx data. Counts how many capital and small letters are in sentence.
 * Result is supposed to be stored in global variable of type "letter_count_" that is defined in "main.h"
 *
 * @param1 - received sign
 */
void proccesDmaData(const uint8_t* sign, int pos);


/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

uint8_t rx_data[35];
uint8_t id = 0;
uint8_t start = 0;
//external variables
uint8_t mode = 1;
uint8_t manualValue = 0;
uint8_t internalmode = 1;


static const char prikaz1[] = "auto";
static const char prikaz2[] = "manual";

int main(void)
{

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  USART2_RegisterCallback(proccesDmaData);
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  /* Space for your local variables, callback registration ...*/



  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

//	#if POLLING
//		//Polling for new data, no interrupts
//		USART2_CheckDmaReception();
//		LL_mDelay(10);
//	#else
//
//		USART2_PutBuffer(0,0);
//		LL_mDelay(200);
//	#endif

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}

int equals(char firstStr[],char secondStr[])
{
    int i,result=1;
    for(i=0; firstStr[i]!='\0' || secondStr[i]!='\0'; i++) {
        if(firstStr[i] != secondStr[i]) {
            result=0;
            break;
        }
    }
    return result;
}

int equalsPWM(char str[])
{
	uint8_t prikaz[] = "PWMxx";
	uint8_t i,result=1;
    uint8_t num[3] ;
    for(i=0; prikaz[i]!='\0' || str[i]!='\0'; i++) {

        if(i<=2 && prikaz[i] != str[i]) {
            result=0;
            break;
        }else if(i>2 && result==1){
        	num[i-3]=str[i];
        }
    }


    manualValue = atoi(num);
    if(manualValue>99){
    	manualValue = 99;
    }else if(manualValue<0) {
    	manualValue = 0;
    }

    return result;
}



void sendUsart2Buffer(int state){
//state = 0 ->manual
//state = 1 ->auto
	uint8_t tx_data[] = "Selected mode: %s\r\n";
	uint8_t length = 0;
	if(state == 1){
		length = sizeof(tx_data)+sizeof(prikaz1);
	}else{
		length = sizeof(tx_data)+sizeof(prikaz2);
	}
	uint8_t str[length];
	if(state == 1){
		sprintf(str,tx_data,prikaz1);
	}else{
		sprintf(str,tx_data,prikaz2);
	}

	USART2_PutBuffer(str, sizeof(str));
}


/*
 * Implementation of function processing data received via USART.
 */

void proccesDmaData(const uint8_t* sign,int pos)
{

	  for(uint8_t i = 0; i < pos; i++)
	    {
		  uint8_t a = *(sign+i);
		  if (start == 1 && id >=34 ){
		  		start=0;
		  		id=0;
		  	}else if(start == 1){


		  		if(a=='$'){

		  			if(strcmp(rx_data,prikaz1)==0 && equals(rx_data,prikaz1)){
		  				mode = 1;
		  				internalmode = mode;
		  				sendUsart2Buffer(mode);
		  			}else if (strcmp(rx_data,prikaz2)==0 && equals(rx_data,prikaz2)){
		  				internalmode = 0;
		  				sendUsart2Buffer(internalmode);
		  			}else if(internalmode==0 && equalsPWM(rx_data)){
		  				mode = 0;
		  			}
		  			memset(&rx_data[0], 0, sizeof(rx_data));
		  			start=0;
		  			id=0;
		  		}else{
		  			rx_data[id]= a;
		  			id +=1;
		  		}

			}

			if(a == '$')
			{
				start=1;
			}

	    }
}

//void writeCCR() {
//	if (mode){
//		setDutyCycle(value);
//
//
//	} else {
//		value = 0;
//		state = 0;
//		setDutyCycle(value);
//	}
//}

void setDutyCycle(uint8_t D) {
	LL_TIM_OC_SetCompareCH1(TIM2, D);
}


void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
