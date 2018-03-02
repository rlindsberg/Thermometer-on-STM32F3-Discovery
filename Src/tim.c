/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
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
#include "tim.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "crc.h"
/* Buffers used for displaying Time and Date */
extern uint8_t aShowTime[50] = {0};
extern uint8_t aShowDate[50] = {0};

/* USER CODE END 0 */

TIM_HandleTypeDef htim2;

/* TIM2 init function */
void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_IC_InitTypeDef sConfigIC;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFF;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchronization(&htim2, &sSlaveConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /**TIM2 GPIO Configuration
    PA15     ------> TIM2_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /**TIM2 GPIO Configuration
    PA15     ------> TIM2_CH1
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
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
    RTC_CalendarShow(aShowTime, aShowDate);
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

      } else {
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

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
