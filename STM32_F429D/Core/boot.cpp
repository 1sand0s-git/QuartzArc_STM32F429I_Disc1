/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F429I Discovery                                                 */
/*                                                                         */
/*   System: Core                                                          */
/*   Role: System Initialization                                           */
/*   Filename: boot.cpp                                                    */
/*   Date: 11th November 2021                                              */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Includes
#include "boot.hpp"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

  //-------------------------------
  //-------------------------------
  //System Initialization Functions

//SystemInitialize
//System Initialization Function
//
//Used to initialize CPU caches, NVIC and SysTick, as well as oscillators, PLLs, system clocks, bus clocks and some peripheral clocks.
//Also enables all GPIOs
//
//Returns QA_OK if successful, or QA_Fail if initialized failed
QA_Result SystemInitialize(void) {
	//-----------------------
  //Enable Cache & Prefetch
	__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
	__HAL_FLASH_DATA_CACHE_ENABLE();
	__HAL_FLASH_PREFETCH_BUFFER_ENABLE();


	//--------------------------
	//Set NVIC Priority Grouping
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);


	//------------
	//Init SysTick
	if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK)
		return QA_Fail;


	//-------------------------
	//Setup Power Configuration
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);


	//Configure Oscillators
	//
	//Configure High Speed External (HSE) oscillator to be used, and setup primary PLL to provide 168MHz clock
	//NOTE: HSE Oscillator provided on STM32F429I-DISC1 board is provided by a 8MHz oscillator (labeled X3 on the PCB)
	RCC_OscInitTypeDef RCC_OscInit = {0};
	RCC_OscInit.OscillatorType = RCC_OSCILLATORTYPE_HSE; //Define which oscillator is to be configured
	RCC_OscInit.HSEState       = RCC_HSE_ON;             //Set High Speed Extenral oscillator as system clock
	RCC_OscInit.PLL.PLLState   = RCC_PLL_ON;             //Enable primary PLL
	RCC_OscInit.PLL.PLLSource  = RCC_PLLSOURCE_HSE;      //Set High Speed External oscillator as PLL input c;pcl

	  //Set PLL Values required for 168MHz system clock
	  //NOTE: Maximum system clock speed for this device is 180MHz, but clock is having to be limited to 168MHz
	  //due to primary PLL also needing to generate 48MHz clock for USB internal PHY
	RCC_OscInit.PLL.PLLM       = 8;
	RCC_OscInit.PLL.PLLN       = 144;
	RCC_OscInit.PLL.PLLP       = RCC_PLLP_DIV2;          //Divider for system clock
	RCC_OscInit.PLL.PLLQ       = 3;                      //Divider for 48MHz USB PHY clock

	HAL_StatusTypeDef eRet = HAL_RCC_OscConfig(&RCC_OscInit);
	if (eRet != HAL_OK) {     //Initialize oscillators using values in init structure
		return QA_Fail;
	}


	//-------------------
	//Enable SysCfg Clock
	__HAL_RCC_SYSCFG_CLK_ENABLE();


	//----------------------------
	//Configure CPU and Bus Clocks
	RCC_ClkInitTypeDef RCC_ClkInit = {0};
	RCC_ClkInit.ClockType       = RCC_CLOCKTYPE_HCLK |      //Define which clocks are to be configured (HCLK, SYSCLK, PCLK1, PCLK2)
			                          RCC_CLOCKTYPE_SYSCLK |
																RCC_CLOCKTYPE_PCLK1 |
																RCC_CLOCKTYPE_PCLK2;

	RCC_ClkInit.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;  //Set primary PLL as system clock source

	RCC_ClkInit.AHBCLKDivider   = RCC_SYSCLK_DIV1;          //Set clock divider for host bus (AHB), DIV1 provides an AHB frequency of 168MHz

	RCC_ClkInit.APB1CLKDivider  = RCC_HCLK_DIV4;            //Set clock divider for peripheral bus 1 (APB1), DIV4 provides an APB1 frequency of 42MHz
	                                                        //NOTE: APB1 timer clocks are clock doubled, providing APB1 timers with 84MHz clocks

	RCC_ClkInit.APB2CLKDivider  = RCC_HCLK_DIV2;            //Set clock divider for peripheral bus 2 (APB2), DIV2 provides an APB2 frequency of 84MHz
	                                                        //NOTE: APB2 timer clocks are clock doubled, providing APB2 timers with 168MHz clocks

	if (HAL_RCC_ClockConfig(&RCC_ClkInit, FLASH_LATENCY_5) != HAL_OK) {  //Initialize system clocks using required values,
		                                                                   //and setting Flash Latency to 5 cycles
		return QA_Fail;
	}


  //----------------------------
  //Initialize Peripheral Clocks
  RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit = {0};
  RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC |
  		                                     RCC_PERIPHCLK_LTDC;

    //Define settings for PLLSAI, as this PLL is also used as the clock source for the LTDC peripheral which we require to run the LCD screen
	RCC_PeriphClkInit.PLLSAI.PLLSAIN       = 192;
	RCC_PeriphClkInit.PLLSAI.PLLSAIR       = 4;
	RCC_PeriphClkInit.PLLSAI.PLLSAIQ       = 4;
	RCC_PeriphClkInit.PLLSAIDivQ           = 1;
	RCC_PeriphClkInit.PLLSAIDivR           = RCC_PLLSAIDIVR_8;

  RCC_PeriphClkInit.RTCClockSelection    = RCC_RTCCLKSOURCE_LSI; //Set the LSI oscillator as the clock source for the Real Time Clock

  if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit) != HAL_OK) { //Initialize the peripheral clocks used the required values
  	return QA_Fail;
  }


	//---------------------------
	//Enable all GPIO Port clocks
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();


  //---------------------
  //Enable all DMA Clocks
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();


	//Return
	return QA_OK;
}






