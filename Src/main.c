/**
******************************************************************************
* File Name          : main.c
* Description        : Main program body
******************************************************************************
** This notice applies to any and all portions of this file
* that are not between comment pairs USER CODE BEGIN and
* USER CODE END. Other portions of this file, whether
* inserted by the user or by software development tools
* are owned by their respective copyright owners.
*
* COPYRIGHT(c) 2018 STMicroelectronics
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"
#include "crc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "display.h"
//These declaration are put here to avoid being flushed when generating new code with CubeMX
ITStatus UartReady = RESET;
/* Size of Transmission buffer */
#define BUFFERSIZE 6
uint8_t Buffer[] = "Hello World interrupt!";
uint8_t RXBuffer[] = "";
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */


/**
* @brief  Tx Transfer completed callback
* @param  UartHandle: UART handle.
* @note   A simple way to report end of IT Tx transfer, and
*         you can add your own implementation.
* @retval None
**/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle){
  /* Set transmission flag: transfer complete*/
  UartReady = SET;
}

/**
* @brief  Rx Transfer completed callback
* @param  UartHandle: UART handle.
* @note   A simple way to report end of IT Tx transfer, and
*         you can add your own implementation.
* @retval None
**/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle){
  /* Set transmission flag: transfer complete*/
  selectRow(0);
  printf("Printing time to display..\n");
  for (size_t i = 0; i < 6; i++) {
    sendDataToDisplay(RXBuffer[i]);
    if (i == 1 || i == 3 ) {
      sendDataToDisplay(58);
    }
  }
  UartReady = SET;
}

/**
* @brief  Checks if it is a short pulse or a long one.
* @param
* @note   None
* @retval None
**/
int interpretPulse(uint16_t ticks){
  if (283 < ticks && ticks < 483) { //short puls, 1. 383µs +-50µs
    //The LED indicates that the board is listening for pulses.
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12);
    return 1;
  } else if (1264 < ticks && ticks < 1464) { //long puls, 0. 1364µs +- 50µs
    //The LED indicates that the board is listening for pulses.
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12);
    return 0;
  } //ticks2 is the same as ticks2 - ticks1 because we reset the timer upon positive edge
  return 3000; //error code 3000.
}

/**
* @brief  Saves pulse to a variable
* @param  int dataBit, uint32_t myVariable
* @note   ...
* @retval None
**/
uint32_t savePulse(int dataBit, uint32_t myVariable){
  return myVariable = (myVariable << 1) | dataBit;
}

/**
* @brief  Rising/falling edge captured callback
* @param  TIM_HandleTypeDef: htim2
* @note   A simple way to report capture of an edge
* @retval None
**/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim2){
  static uint16_t ticks1, ticks2;
  static uint32_t preamble = 0;
  static uint32_t temperaturData = 0;
  static uint32_t receivedCRC = NULL;
  static uint32_t calculatedCRC = NULL;
  static int bitCounter = 0;
  static int clearForTemp = 0;
  static int clearForCRC = 0;

  if (htim2->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
    //Gets input capture value upon negative edge
    ticks2 = HAL_TIM_ReadCapturedValue(htim2, TIM_CHANNEL_2);
    //Gets input capture value upon positive edge
    ticks1 = HAL_TIM_ReadCapturedValue(htim2, TIM_CHANNEL_1);


    if (preamble == 0xFF) { //preamble is valid, 1111 1111
      //Now receives data from the censor
      // temperaturData = savePulse(0, temperaturData);
      // temperaturData = savePulse(1, temperaturData); //Just to verify that the program DOES write data..
      if (bitCounter < 8) {
        temperaturData = savePulse(interpretPulse(ticks2), temperaturData); //Saves data to temperaturData
        bitCounter ++;
      }

      //When 8 bits saved to temperaturData
      if (bitCounter == 8) {
        if (temperaturData == 0x48 && clearForTemp == 0) { //0100 1000
          //preamble complete, first 8 bits of temperaturData correct
          //All clear to fill temperatureData with more bits
          clearForTemp = 1;
          return;
        } else if (clearForTemp == 0){ //not ready to move on, first 8 bits of data are not correct, reset
          temperaturData = 0;
          bitCounter = 0;
          preamble = 0;
          clearForTemp = 0;
          clearForCRC = 0;
          return;
        }
      }

    } else { //preamble not complete
      //Checks preamble
      if (interpretPulse(ticks2) == 1) {
        int dataBit = 1;
        preamble = savePulse(dataBit, preamble);
      } else {
        preamble = 0; //reset
        return;
      }

    }

    //preamble complete, first 8 bits of temperaturData correct, cotinues..
    if (clearForTemp == 1 && bitCounter < 32) {
      temperaturData = savePulse(interpretPulse(ticks2), temperaturData);
      bitCounter ++;
    } else if (clearForTemp == 1 && bitCounter == 32) {
      clearForCRC = 1;
    }

    //temperaturData completed, begins to receive CRC
    if (clearForCRC == 1 && bitCounter < 40) {
      receivedCRC = savePulse(interpretPulse(ticks2), receivedCRC);
      bitCounter ++;
      // printf("bitCounter is %i\n", bitCounter);
    }
    if (bitCounter == 40) { //Reception complete, calculates and verifies CRC
      calculatedCRC = HAL_CRC_Calculate(&hcrc, &temperaturData, 1);

      //Received data is correct, prints temp to terminal.
      if (receivedCRC == calculatedCRC) {
        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13); //data correct
        double tempToPrint = (double)((temperaturData >> 8) & 0x3FF) / 10; //00 0000 0000 0000 11 1111 1111
        printf("The current temperature is %0.1f Celsius, \n", tempToPrint);
        int humToPrint = (temperaturData & 0x7F); //0....0111 1111
        printf("the humidity is %d%%.\n", humToPrint);
        //Creates a buffer for line 1 on the display
        char buffer[10];
        sprintf(buffer, "Temp:%0.1fC", tempToPrint);
        selectRow(1);
        printf("Printing temp to display..\n");
        sendCharToDisplay(buffer);
        selectRow(2);
        printf("Pringting hum to display..\n");
        char buffer2[10]; //buffer2 for line 2
        // sprintf(buffer2, "Temp:%0.1fC", tempToPrint);
        sprintf(buffer2, "Hum:   %d%%", humToPrint);
        sendCharToDisplay(buffer2);

        buffer[0]='\0';
        buffer2[0]='\0';
        temperaturData = 0;
        bitCounter = 0;
        preamble = 0;
        clearForTemp = 0;
        clearForCRC = 0;
        calculatedCRC = 0;
        receivedCRC = 0;
        return;

      }
    }

  }

}//callback ends

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_CRC_Init();
  MX_SPI2_Init();

  /* USER CODE BEGIN 2 */
  initDisplay();
  selectRow(0);
  // sprintf

  //Start TIM2
  if (HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    /* Put UART peripheral in reception process */
    if(HAL_UART_Receive_IT(&huart3, (uint8_t *)RXBuffer, BUFFERSIZE) != HAL_OK)
    {
      Error_Handler();
    }
    /* Wait for the end of the transfer */
    while (UartReady != SET)
    {
    }
    /* Reset transmission flag */
    UartReady = RESET;

    /* Start the transmission process */
    if(HAL_UART_Transmit_IT(&huart3, (uint8_t *)RXBuffer, BUFFERSIZE)!= HAL_OK)
    {
      Error_Handler();
    }
    /* Wait for the end of the transfer */
    while (UartReady != SET)
    {
    }
    /* Reset transmission flag */
    UartReady = RESET;


  }

  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Configure the Systick interrupt time
  */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  /**Configure the Systick
  */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
* @brief  This function is executed in case of error occurrence.
* @param  None
* @retval None
*/
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT

/**
* @brief Reports the name of the source file and the source line number
* where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
