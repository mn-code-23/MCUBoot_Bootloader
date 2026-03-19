/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iwdg.h"
#include "usart.h"
#include "gpio.h"
#include "../../MCUBoot/bootutil/include/bootutil/bootutil.h"  /* boot_go() */
#include "../../MCUBoot/flash_map_backend/boot_hal.h"            /* boot_hal_jump_to_app() */
#include "../../MCUBoot/mcuboot_config/mcuboot_config.h"
#include <stdio.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* ================================================================
 * REDIRECTION printf → UART4 (pour les logs MCUBoot)
 * ================================================================ */
int _write(int file, char *ptr, int len)
{
    (void)file;
    HAL_UART_Transmit(&huart4, (uint8_t *)ptr, (uint16_t)len, 100);
    return len;
}
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

void mcuboot_watchdog_feed(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}

int main(void)
{
	  struct boot_rsp boot_response;
	  /*fih_ret         boot_result = FIH_FAILURE; */
	  int             boot_result;
	  uint32_t        app_start;
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_IWDG_Init();
  MX_USART2_UART_Init();
  MX_UART4_Init();

  /* ----------------------------------------------------------
    * 2. Bannière de démarrage
    * ---------------------------------------------------------- */
   MCUBOOT_LOG_INF("========================================");
   MCUBOOT_LOG_INF(" Secure Bootloader — STM32L4R5VGT6");
   MCUBOOT_LOG_INF(" MCUBoot v2.1.0 / ECDSA-P256");
   MCUBOOT_LOG_INF("========================================");

   HAL_Delay(500);

   /* Vérification image */
   MCUBOOT_LOG_INF("Verification de l image en cours...");

   /* ----------------------------------------------------------
    * 3. Nourrir le watchdog avant une opération longue
    * ---------------------------------------------------------- */
   HAL_IWDG_Refresh(&hiwdg);

   /*
      * mcuboot_watchdog_feed()
      * Appelée par MCUBoot pendant les opérations longues (swap, erase).
      * Permet à hiwdg de rester dans main.c sans export global.
      */

   /* ----------------------------------------------------------
       * 4. Appel de la fonction principale MCUBoot
       *
       *    boot_go() effectue dans l'ordre :
       *      a) Vérification de la signature du Slot 0 (ECDSA-P256)
       *      b) Vérification du hash SHA-256 de l'image
       *      c) Si une image en attente est dans le Slot 1 :
       *           → Swap Slot 1 → Slot 0 (via SWAP_USING_MOVE)
       *           → Redémarre depuis le Slot 0 mis à jour
       *      d) Remplit boot_response avec l'adresse de démarrage
   * ---------------------------------------------------------- */
   MCUBOOT_LOG_INF("Vérification de l'image en cours...");

   boot_result = boot_go(&boot_response);
     /* ----------------------------------------------------------
      * 5. Analyse du résultat
      * ---------------------------------------------------------- */
   if (boot_result != 0) {
	    while (1) {
	        MCUBOOT_LOG_ERR("boot_go() ECHEC code=%d - Signature invalide ?",
	                        boot_result);
	        HAL_Delay(1000);
	    }
//       MCUBOOT_LOG_ERR("boot_go() ÉCHEC (code=%d)", boot_result);
        /* MCUBOOT_LOG_ERR("Aucune image valide trouvée — système bloqué");
         MCUBOOT_LOG_ERR("Attente du reset watchdog..."); */

         /* Bloquer — le IWDG va resetter dans ~5 secondes */
//       while (1) {
//           __NOP();
//       }
   }

   /* ----------------------------------------------------------
        * 6. Image valide — calcul de l'adresse de saut
        *
        *    boot_response.br_image_off : offset du début de l'image
        *    boot_response.br_hdr       : pointeur vers le header MCUBoot
        *    boot_response.br_hdr->ih_hdr_size : taille du header (0x400)
        *
        *    app_start = base Flash + offset image + taille header
    * ---------------------------------------------------------- */
   app_start = FLASH_BASE_ADDRESS
             + boot_response.br_image_off
             + boot_response.br_hdr->ih_hdr_size;

   MCUBOOT_LOG_INF("Image valide — version %d.%d.%d",
                   boot_response.br_hdr->ih_ver.iv_major,
                   boot_response.br_hdr->ih_ver.iv_minor,
                   boot_response.br_hdr->ih_ver.iv_revision);
   MCUBOOT_LOG_INF("Adresse de saut : 0x%08X", app_start);

       /* ----------------------------------------------------------
        * 7. Nourrir le watchdog une dernière fois avant le saut
        * ---------------------------------------------------------- */
   HAL_IWDG_Refresh(&hiwdg);

       /* ----------------------------------------------------------
        * 8. SAUT VERS L'APPLICATION — aucun retour après cette ligne
        * ---------------------------------------------------------- */
   boot_hal_jump_to_app(app_start);

       /* Ne doit jamais être atteint */
   while (1) { __NOP(); }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/* Implement MMIO setup functions in the auto-generated peripheral files
 * (gpio.c, iwdg.c, usart.c) to avoid duplicate symbol definitions. */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  MCUBOOT_LOG_ERR("Error_Handler — reset imminent");
  while (1)
  {
	  __NOP();
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
