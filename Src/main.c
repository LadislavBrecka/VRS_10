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
uint8_t mode = 0;

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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  /* Space for your local variables, callback registration ...*/

  USART2_RegisterCallback(proccesDmaData);

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



void sendUsart2Buffer(int state){
//state = 0 ->manual
//state = 1 ->auto
	uint8_t tx_data[] = "Selected mode: %s          \r\n";
	uint8_t str[sizeof(tx_data)];
	if(state == 1){
		sprintf(str,tx_data,"auto");
	}else{
		sprintf(str,tx_data,"manual");
	}

	USART2_PutBuffer(str, sizeof(str));
}


/*
 * Implementation of function processing data received via USART.
 */

void proccesDmaData(const uint8_t* sign,int pos)
{
	  static const char prikaz1[] = "auto";
	  static const char prikaz2[] = "manual";


	  for(uint8_t i = 0; i < pos; i++)
	    {
		  uint8_t a = *(sign+i);
		  if (start == 1 && id >=34 ){
		  		start=0;
		  		id=0;
		  	}else if(start == 1){


		  		if(a=='$'){

		  			if(strcmp(rx_data,prikaz1)==0 && equals(rx_data,prikaz1)){
		  				sendUsart2Buffer(1);
		  				LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
		  			}else if (strcmp(rx_data,prikaz2)==0 && equals(rx_data,prikaz2)){
		  				LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
		  				sendUsart2Buffer(0);
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
