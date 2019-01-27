/** 
****************************************************************************** 
@brief   Pingpong test program  
@file    main.c  
@author  Roderick Karlemstrand, Junyu Chen
@version 0.1 
@date    18-Januari-2018 
@brief   Testing functions and structures for program Pingpong 
****************************************************************************** 
*/ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"
#include "gpio.h"
#include "Pingpong_functions.h"
#include  "clockinit.h"
#include "stdbool.h"
states State, NextState;

int main(void) {
  bool ButtonPressed; // To remember that button is pressed
  uint32_t Varv, Speed; // Ball speed
  uint8_t Led; // LED nr
  State = Start; // Initiate State to Start NextState= Start;
  Speed = 500000; // Number of loops
  
  uint8_t LeftPts = 0;
  uint8_t RightPts = 0;

  /* MCU Configuration----------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* Infinite loop */
  
  while (1) {
    State = NextState;
    switch (State) // State machine
    {
    case Start:
      Led_on(0); // Turn off all LEDs
      if (L_hit() == true) // L serve
      {
        Led = 1;
        NextState = MoveRight;
        
        while (L_hit() == true); // wait until button is released
      }
      
      else if (R_hit() == true) // R serve
      {
        
        Led = 8;
        NextState = MoveLeft;
        while (R_hit() == true); // wait until button is released
      } else
        NextState = Start; // Stay in Start state
      break;
      
    case MoveRight:
      {
        Led_on(Led);
        Varv = Speed;
        while (Varv != 0) {
          if (R_hit()) ButtonPressed = true; // R hit
          Varv--;
          
        }
        
        if (ButtonPressed) // R pressed
        {
          if (Led == 8) //andLED8activa
          {
            NextState = MoveLeft; // return ball
            Speed /= 1.2;
            Led = 7;
          } else NextState = Start; // hit to early
        } else {
          if (Led == 9) //R player miss
          {LeftPts = LeftPts * 2 + 1; 
          NextState = ShowPoints;}
          else NextState = MoveRight; // ball continues to move right
          
        }
        if (!ButtonPressed) Led++; // prepare to turn next LED on
        ButtonPressed = false;
      }
      
      break;
    case MoveLeft:
      {
        Led_on(Led);
        Varv = Speed;
        while (Varv != 0) {
          if (L_hit()) ButtonPressed = true; // L hit
          Varv--;
        }
        if (ButtonPressed) // L pressed
        {
          if (Led == 1) //andLED1active
          {
            NextState = MoveRight; // return ball
            Speed /= 1.2;
            Led = 2;
          } else NextState = Start; // hit to early
        } else {
          
          if (Led == 0) //L player miss
          {RightPts = RightPts * 2 + 1; 
          NextState = ShowPoints;}
          else NextState = MoveLeft; // ball continues to move left
          
        }
        if (!ButtonPressed) Led--; // prepare to turn next LED on
        ButtonPressed = false;
      }
      break;
      
    case ShowPoints:

      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(LED8_GPIO_Port, LED8_Pin, GPIO_PIN_SET);
      HAL_Delay(100);
      
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED8_GPIO_Port, LED8_Pin, GPIO_PIN_RESET);
      
      
      
      
      
      Show_points(LeftPts, RightPts);
      HAL_Delay(1000);
      if(LeftPts & 0xF | RightPts & 0xF) // 1111, 4 pts
      {
        NextState= GameOver;
      }
      NextState= Start;
      break;
      
    case GameOver:
      LeftPts = 0;
      RightPts = 0;
      NextState= Start;
      break;
      
    default:
      break;
    }
  }
}




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
