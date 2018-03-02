/**
******************************************************************************
* File Name          : display.c
* Description        : Function library for display
******************************************************************************
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
#include "display.h"

/**
* @brief  Init display
* @param  None
* @note   ..
* @retval None
**/
void initDisplay(void){
  //Set CS to 0, Reset SPI2_NSS
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
  HAL_Delay(10);
  // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  // HAL_Delay(10);
  //Initialization commands
  uint8_t functionSet[3] = {0x1f, 0x0a, 0x03};
  sendCommandToSPI(functionSet);
  uint8_t extendedFunctionSet[3] = {0x1f, 0x09, 0x0};
  sendCommandToSPI(extendedFunctionSet);
  uint8_t entryModeSet[3] = {0x1f, 0x06, 0x0};
  sendCommandToSPI(entryModeSet);
  uint8_t biasSetting[3] = {0x1f,0x0e,0x01};
  sendCommandToSPI(biasSetting);
  uint8_t functionSet2[3] = {0x1f,0x09,0x03};
  sendCommandToSPI(functionSet2);
  uint8_t internalOSC[3] = {0x1f,0x0b,0x01};
  sendCommandToSPI(internalOSC);
  uint8_t followerControl[3] = {0x1f,0x0e,0x06};
  sendCommandToSPI(followerControl);
  uint8_t powerControl[3] = {0x1f,0x06,0x05};
  sendCommandToSPI(powerControl);
  uint8_t contrastSet[3] = {0x1f,0x0a,0x07};
  sendCommandToSPI(contrastSet);
  uint8_t functionSet3[3] = {0x1f,0x08,0x03};
  sendCommandToSPI(functionSet3);
  uint8_t displayOn[3] = {0x1f,0x0f,0x00};
  sendCommandToSPI(displayOn);
  uint8_t clearDisplay[3] = {0x1f,0x01,0x0};
  sendCommandToSPI(clearDisplay);

}

/**
* @brief  Init display
* @param  uint8_t commandToBeSent[]
* @note   ..
* @retval None
**/
void sendCommandToSPI(uint8_t commandToBeSent[]){
  for (int i = 0; i < 3; i++) {
    HAL_SPI_Transmit(&hspi2, &commandToBeSent[i], 1, 50);
  }
}

/**
* @brief  Put cursor onto the desired row
* @param  uint8_t rowNr
* @note   ..
* @retval None
**/
void selectRow(uint8_t rowNr){
  uint8_t rowAddress = 0x80 + rowNr * 0x20;
  printf("Selecting row with addr %x\n", rowAddress);
  uint8_t displayBuffer[3];
  displayBuffer[0] = 0x1f;
  displayBuffer[1] = rowAddress & 0x0f;
  displayBuffer[2] = (rowAddress >> 4) & 0x0f;
  HAL_SPI_Transmit(&hspi2, displayBuffer, 3, 1000);
}

/**
* @brief  Sends an ASCII code to display
* @param  uint8_t asciiData
* @note   ..
* @retval None
**/
void sendDataToDisplay(uint8_t asciiData) {  // Send one character to display
  uint8_t displayBuffer[3];
  displayBuffer[0] = 0x5f;
  displayBuffer[1] = asciiData & 0x0f;
  displayBuffer[2] = (asciiData >> 4) & 0x0f;
  HAL_SPI_Transmit(&hspi2, displayBuffer, 3, 500);
  // sendCommandToSPI(displayBuffer); //doesn't work..
}

/**
* @brief  Sends a string to display
* @param  char charBuffer[]
* @note   ..
* @retval None
**/
void sendCharToDisplay(char charBuffer[]) {  // Send one character to display
  uint8_t displayBuffer[3];
  displayBuffer[0] = 0x5f;
  for (size_t i = 0; i < 10; i++) {
    displayBuffer[1] = charBuffer[i] & 0x0f;
    displayBuffer[2] = charBuffer[i] >> 4 & 0x0f;
    HAL_SPI_Transmit(&hspi2, displayBuffer, 3, 500);
  }
}
