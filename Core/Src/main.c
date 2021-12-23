/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <time.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
struct tm EventDate;
struct tm NowTime;
int utc_timezone = 7;

#define FONT_LENGTH          11
#define FONT_START_CHAR      48
#define FONT_CHAR_WIDTH      8
#define FONT_CHAR_HEIGHT     13
#define FONT_ARRAY_LENGTH    (FONT_LENGTH * FONT_CHAR_WIDTH)

const unsigned short font_f_for16x16[FONT_ARRAY_LENGTH] = { 0b0000111111111110,
		0b0001100000000011, 0b0001000000000001, 0b0001000000000001,
		0b0001000000000001, 0b0001100000000011, 0b0000111111111110,
		0b0000000000000000, // Символ 48  <0>
		0b0000000000000000, 0b0000000000001000, 0b0000000000001100,
		0b0000000000000110, 0b0001111111111111, 0b0000000000000000,
		0b0000000000000000,
		0b0000000000000000, // Символ 49  <1>
		0b0001000000000010, 0b0001110000000011, 0b0001011000000001,
		0b0001001110000001, 0b0001000011000001, 0b0001000001110010,
		0b0001000000011100,
		0b0000000000000000, // Символ 50  <2>
		0b0001000000000001, 0b0001000000100001, 0b0001000000100001,
		0b0001000000100001, 0b0001000000100001, 0b0000100001110011,
		0b0000011110001110,
		0b0000000000000000, // Символ 51  <3>
		0b0000010000000000, 0b0000011111000000, 0b0000010001111100,
		0b0000010000000111, 0b0000010000000000, 0b0001111110000000,
		0b0000010000000000,
		0b0000010000000000, // Символ 52  <4>
		0b0001000000111111, 0b0001000000100001, 0b0001000000100001,
		0b0001000000100001, 0b0001000000100001, 0b0000100001000001,
		0b0000011110000001,
		0b0000000000000000, // Символ 53  <5>
		0b0000111111110000, 0b0001000000111100, 0b0001000000100110,
		0b0001000000100011, 0b0001000000100001, 0b0001000000100000,
		0b0000111111000000,
		0b0000000000000000, // Символ 54  <6>
		0b0000000000000111, 0b0000000000000001, 0b0001100000000001,
		0b0001111100000001, 0b0000000111110001, 0b0000000000011111,
		0b0000000000000011,
		0b0000000000000000, // Символ 55  <7>
		0b0000011110000000, 0b0000100001011110, 0b0001000000110011,
		0b0001000000100001, 0b0001000000100001, 0b0001000000110011,
		0b0000100001011110,
		0b0000011110000000, // Символ 56  <8>
		0b0000000001111110, 0b0001000010000001, 0b0001100010000001,
		0b0000100010000001, 0b0000111010000001, 0b0000011110000001,
		0b0000000111111110,
		0b0000000000000000, // Символ 57  <9>
		0b0000000000000000, 0b0000000000000000, 0b0000000000000000,
		0b0000001100011000, 0b0000001100011000, 0b0000000000000000,
		0b0000000000000000, 0b0000000000000000  // Символ 58  <:>
		};
#define SETTINGS_ADDRESS 0x81E0020 //адрес для сохранения настроек
#define trig_time_err 60 //калибровка каждые trig_time_err секунд
#define digitalWrite(port, pin, st) if(st){port->BSRR = pin;}else{port->BSRR = (uint32_t)pin << 16;}
#define SIZE_BF_SET 28 //размер команды настройки
#define SIZE_BF 600 //размер кольцевого буфера приемника gps nmea сообщений
#define Rx1Buf_SIZE 100 //размер ожидаемой части для добавления в кольцевой буфер

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint16_t TOnLP = 10;

uint8_t RAW_DISPLAY[20][32];
uint8_t ColorMatrix = 0;
const uint8_t ColorMatrix0 = 4;
uint8_t ColorMatrix1 = 7;
int SetColorMatrix = 7;

void delay_us(uint16_t us) //create delay function
{
	for (uint16_t i = 0; i < us; i++) {
		for (uint8_t j = 0; j < 16; j++)
			asm("NOP");
		//Perform no operation //assembly code
	}
}

void SaveSettings() {
	int bp[8] = { utc_timezone, EventDate.tm_sec, EventDate.tm_min,
			EventDate.tm_hour, EventDate.tm_mday, EventDate.tm_mon,
			EventDate.tm_year, SetColorMatrix };
	HAL_FLASH_Unlock();
	FLASH_Erase_Sector(FLASH_SECTOR_7, FLASH_BANK_2, VOLTAGE_RANGE_1);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, SETTINGS_ADDRESS, *bp);
	HAL_FLASH_Lock();
}

void RestoreSettings() {
	utc_timezone = *(uint32_t*) SETTINGS_ADDRESS;
	EventDate.tm_sec = *(uint32_t*) (SETTINGS_ADDRESS + 4);
	EventDate.tm_min = *(uint32_t*) (SETTINGS_ADDRESS + 8);
	EventDate.tm_hour = *(uint32_t*) (SETTINGS_ADDRESS + 12);
	EventDate.tm_mday = *(uint32_t*) (SETTINGS_ADDRESS + 16);
	EventDate.tm_mon = *(uint32_t*) (SETTINGS_ADDRESS + 20);
	EventDate.tm_year = *(uint32_t*) (SETTINGS_ADDRESS + 24);
	SetColorMatrix = *(uint32_t*) (SETTINGS_ADDRESS + 28);
	if (SetColorMatrix) {
		ColorMatrix1 = SetColorMatrix;
	}
}

void SetPixel(uint8_t Xp, uint8_t Yp, uint8_t N_p, uint16_t ON) {
	uint8_t n_byte = 0;
	uint8_t bit_mask;
	if (Yp >= 8) {
		Yp -= 8;
		n_byte = 16;
	}
	if (Yp < 4) {
		if (Xp < 8) {
			n_byte += (Yp % 4) * 4;
		} else {
			n_byte += 2 + (Yp % 4) * 4;
		}
		bit_mask = 128 >> (Xp % 8);
	} else {
		if (Xp < 8) {
			n_byte += 1 + (Yp % 4) * 4;
		} else {
			n_byte += 3 + (Yp % 4) * 4;
		}
		bit_mask = 1 << (Xp % 8);
	}
	uint8_t SP = RAW_DISPLAY[N_p][n_byte] & bit_mask;
	if (ON) {
		if (!SP) {
			RAW_DISPLAY[N_p][n_byte] |= bit_mask;
		}
	} else {
		if (SP) {
			RAW_DISPLAY[N_p][n_byte] &= ~bit_mask;
		}
	}
}

void SetPixelBigPanel(uint8_t Xp, uint8_t Yp, uint16_t ON) {
	uint8_t N_p_adr;
	switch (Yp / 16) {
	case 0:
		N_p_adr = 4 - (Xp / 16);
		Xp = 79 - Xp;
		break;
	case 1:
		N_p_adr = Xp / 16 + 5;
		break;
	case 2:
		N_p_adr = Xp / 16 + 15;
		break;
	case 3:
		N_p_adr = 14 - (Xp / 16);
		Xp = 79 - Xp;
		break;
	}
	SetPixel(Xp % 16, Yp % 16, N_p_adr, ON);
}

void WriteChar(uint8_t x_s, uint8_t y_s, uint8_t char_id) {
	for (uint8_t x_b = 0; x_b < FONT_CHAR_WIDTH; x_b++) {
		for (uint8_t y_b = 0; y_b < FONT_CHAR_HEIGHT; y_b++) {
			SetPixelBigPanel(x_s + x_b, y_s + y_b,
					font_f_for16x16[char_id * 8 + x_b] & ((uint16_t) 1 << y_b));
		}
	}
}

void Clk_data(uint8_t n_p, uint8_t x_b, uint8_t y_b, uint8_t bit_mask,
		uint8_t CM1, uint8_t CM2, uint8_t n_line) {
	uint16_t CLK_P;
	if (n_line) {
		CLK_P = CLK1_Pin;
		n_p += 10;
		ColorMatrix = ColorMatrix1;
	} else {
		CLK_P = CLK_Pin;
		ColorMatrix = ColorMatrix0;
	}
	digitalWrite(GPIOD, CLK_P, 0);

	if (RAW_DISPLAY[n_p][x_b / 8 + y_b * 4] & bit_mask) {
		CM1 = ColorMatrix;
	}
	if (RAW_DISPLAY[n_p][x_b / 8 + y_b * 4 + 16] & bit_mask) {
		CM2 = ColorMatrix;
	}
	digitalWrite(GPIOA, R1_Pin, CM1 & 0b100);
	digitalWrite(GPIOA, G1_Pin, CM1 & 0b10);
	digitalWrite(GPIOC, B1_Pin, CM1 & 0b1);
	digitalWrite(GPIOA, R2_Pin, CM2 & 0b100);
	digitalWrite(GPIOC, G2_Pin, CM2 & 0b10);
	digitalWrite(GPIOC, B2_Pin, CM2 & 0b1);
	digitalWrite(GPIOD, CLK_P, 1);
}

void LoadAndShowBufOnPanel(void) {
	uint8_t bit_mask;
	uint8_t y_b;
	uint8_t x_b;
	uint8_t n_p;
	uint8_t CM1;
	uint8_t CM2;
	digitalWrite(GPIOD, OE_Pin, 1);
	for (y_b = 0; y_b < 4; y_b++) {
		for (n_p = 0; n_p < 10; n_p++) {
			for (x_b = 0; x_b < 32; x_b++) {
				CM1 = CM2 = 0;
				bit_mask = 1 << (x_b % 8);

				Clk_data(n_p, x_b, y_b, bit_mask, CM1, CM2, 0);
				Clk_data(n_p, x_b, y_b, bit_mask, CM1, CM2, 1);

			}
		}
		digitalWrite(GPIOD, STB_Pin, 1);
		digitalWrite(GPIOD, STB_Pin, 0);

		digitalWrite(GPIOD, A_Pin, (y_b & 0b1) > 0);
		digitalWrite(GPIOC, B_Pin, (y_b & 0b10) > 0);

		digitalWrite(GPIOD, OE_Pin, 0);
		delay_us(TOnLP);
	}
}

char* S_Parser(char *string) {
	static char *source = NULL;
	char *p, *next_data = 0;
	if (string != NULL) {
		source = string;
	}
	if (source == NULL) {
		return NULL;
	}
	if ((p = strpbrk(source, ",")) != NULL) {
		*p = 0;
		next_data = source;
		source = ++p;
	}
	return next_data;
}
uint64_t EventTimeSec;
uint64_t NowTimeSec;
uint64_t NowTimeSec_new;
char SET_data[SIZE_BF_SET] = { 0 };
const char str_SET_data[4] = "SET";
//формат: SET,+3,170222,120000,0,END\n
//+3 - utc_timezone
//170222 - дата в формате DDMMYY
//120000 - время в формате HHMMSS
//0 - цвет, 0 - перебор цветов, 1-7 - 3х-битный RGB-цвет
void GetSetting() {
	char *pstr = strstr(SET_data, str_SET_data);  //Поиск строки
	char *istr = S_Parser(pstr);  //Парсим значения из SET
	uint8_t i_d_SET = 0;
	int e_mon;
	while (istr != NULL)  // Выделение последующих частей
	{
		switch (i_d_SET) {
		case 1:
			sscanf(istr, "%2d", &utc_timezone);
			break;
		case 2:
			sscanf(istr, "%2d%2d%2d", &EventDate.tm_mday, &e_mon,
					&EventDate.tm_year);
			EventDate.tm_mon = e_mon - 1;
			break;
		case 3:
			sscanf(istr, "%2d%2d%2d", &EventDate.tm_hour, &EventDate.tm_min,
					&EventDate.tm_sec);
			EventDate.tm_year += 100;
			break;
		case 4:
			sscanf(istr, "%1d", &SetColorMatrix);
			if (SetColorMatrix) {
				ColorMatrix1 = SetColorMatrix;
			}
			break;
		}
		istr = S_Parser(NULL);  // Выделение очередной части строки
		i_d_SET++;
	}
	EventTimeSec = mktime(&EventDate);
	SaveSettings();
}

const char strfind[4] = "RMC";
char nmea_data[SIZE_BF] = { 0 };
char Rx1Buf[Rx1Buf_SIZE] = { 0 };
void Get_Date_and_Time() {
	char *pstr = strstr(nmea_data, strfind);  //Поиск строки
	char *istr = S_Parser(pstr);  //Парсим значения из RMC
	uint8_t i_d_nmea = 0;
	while (istr != NULL)  // Выделение последующих частей
	{
		if (i_d_nmea == 1) {
			sscanf(istr, "%2d%2d%2d.", &NowTime.tm_hour, &NowTime.tm_min,
					&NowTime.tm_sec);
		}
		if (i_d_nmea == 9) {
			int n_mon;
			sscanf(istr, "%2d%2d%2d", &NowTime.tm_mday, &n_mon,
					&NowTime.tm_year);
			NowTime.tm_mon = n_mon - 1;
			NowTime.tm_year += 100;
			NowTimeSec_new = mktime(&NowTime) + utc_timezone * 3600;
			return;
		}
		i_d_nmea++;
		istr = S_Parser(NULL);  // Выделение очередной части строки
	}
}

uint16_t oldPos = 0;
uint16_t newPos = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &huart1) {
		uint16_t Size = Rx1Buf_SIZE;
		oldPos = newPos; // Обновляем последнюю позицию перед копированием новых данных
		/* Если данные слишком большие и они вот-вот превысят размер буфера, мы должны направить их в начало буфера
		 * Это необходимо для поддержания циклического буфера
		 * Старые данные в основном буфере будут перекрываться
		 */
		if (oldPos + Size > SIZE_BF) // Если текущая позиция + новый размер данных больше, чем основной буфер
		{
			uint16_t datatocopy = SIZE_BF - oldPos; // узнаем, сколько места осталось в основном буфере
			memcpy((uint8_t*) nmea_data + oldPos, Rx1Buf, datatocopy); // копируем данные в оставшееся пространство

			oldPos = 0;   // указываем на начало буфера
			memcpy((uint8_t*) nmea_data, (uint8_t*) Rx1Buf + datatocopy,
					(Size - datatocopy));   // копируем оставшиеся данные
			newPos = (Size - datatocopy);   // обновляем позицию
		}
		/* если текущая позиция + новый размер данных меньше, чем основной буфер
		 * мы просто скопируем данные в буфер и обновим позицию
		 */
		else {
			memcpy((uint8_t*) nmea_data + oldPos, Rx1Buf, Size);
			newPos = Size + oldPos;
		}
		HAL_UART_Receive_DMA(&huart1, (uint8_t*) &Rx1Buf, Rx1Buf_SIZE);
	}
	if (huart == &huart3) {
		GetSetting();
	}
}

uint16_t time_err = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM1) //check if the interrupt comes from TIM1
	{
		Get_Date_and_Time();
		NowTimeSec++;
		if (NowTimeSec != NowTimeSec_new) {
			time_err++;
			if (time_err > trig_time_err) {
				NowTimeSec = NowTimeSec_new;
				time_err = 0;
			}
		} else {
			time_err = 0;
		}

		HAL_UART_Receive_IT(&huart3, (uint8_t*) &SET_data, SIZE_BF_SET - 1);
	}
}

void print_two_dig(uint8_t x_start, uint8_t y_start, uint8_t two_dig) {
	WriteChar(x_start, y_start, two_dig / 10);
	WriteChar(x_start + FONT_CHAR_WIDTH + 2, y_start, two_dig % 10);
}

void print_three_dig(uint8_t x_start, uint8_t y_start, uint16_t three_dig) {
	uint8_t dig1 = (uint16_t) (three_dig / 100);
	uint8_t dig2 = (uint16_t) (three_dig % 100 / 10);
	uint8_t dig3 = (uint16_t) (three_dig % 10);
	if (three_dig < 10) {
		WriteChar(x_start, y_start, 10);
		WriteChar(x_start + FONT_CHAR_WIDTH + 5, y_start, dig3);
		WriteChar(x_start + FONT_CHAR_WIDTH + 18, y_start, 10);
	} else {
		WriteChar(x_start, y_start, dig1);
		WriteChar(x_start + FONT_CHAR_WIDTH + 5, y_start, dig2);
		WriteChar(x_start + FONT_CHAR_WIDTH + 18, y_start, dig3);
	}
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	EventDate.tm_sec = 49; //секунды
	EventDate.tm_min = 30; //минуты
	EventDate.tm_hour = 13; //час
	EventDate.tm_mday = 12; //день
	EventDate.tm_mon = 2; //месяц - 1, т.е. тут указан март
	EventDate.tm_year = 122; //год с 1900 года
	EventDate.tm_isdst = 0; //флаг перехода на летнее время

	uint64_t EventTimer;
	EventTimeSec = mktime(&EventDate);
	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM1_Init();
	MX_USART3_UART_Init();
	MX_DMA_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim1);

	HAL_UART_Receive_DMA(&huart1, (uint8_t*) &Rx1Buf, Rx1Buf_SIZE);

	HAL_UART_Receive_IT(&huart3, (uint8_t*) &SET_data, SIZE_BF_SET - 1);

	//вкл. выход упр. на панели
	HAL_GPIO_WritePin(GPIOD, VCC0_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, VCC1_Pin, GPIO_PIN_SET);

	RestoreSettings();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	//SetPixel(2, 3, 0, 1);
	/*
	 SetPixelBigPanel(64, 0, 1);
	 SetPixelBigPanel(65, 1, 1);
	 SetPixelBigPanel(66, 2, 1);
	 SetPixelBigPanel(67, 3, 1);
	 SetPixelBigPanel(68, 4, 1);
	 SetPixelBigPanel(69, 5, 1);
	 SetPixelBigPanel(70, 6, 1);
	 SetPixelBigPanel(71, 7, 1);
	 SetPixelBigPanel(72, 8, 1);
	 SetPixelBigPanel(73, 9, 1);*/

	//WriteChar(64, 0, 1);
	uint16_t timer_i = 0;
	uint8_t i_c = 0;
	while (1) {

		LoadAndShowBufOnPanel();

		if (HAL_UART_GetState(&huart1) != HAL_UART_STATE_BUSY_RX) {
			HAL_UART_Receive_DMA(&huart1, (uint8_t*) &Rx1Buf, Rx1Buf_SIZE);
		}

		timer_i++;
		if (SetColorMatrix == 0) {
			if (timer_i % 300 == 0) {
				if (i_c < 8) {
					ColorMatrix1 = i_c;
				} else {
					i_c = 0;
				}
				i_c++;
			}
		}
		if (timer_i % 10 == 0) {

			if (EventTimeSec > NowTimeSec) {
				EventTimer = EventTimeSec - NowTimeSec;
				print_three_dig(23, 12, (uint16_t) (EventTimer / 86400));
				print_two_dig(5, 36, (uint8_t) (EventTimer / 3600 % 24));
				WriteChar(23, 36, 10);
				print_two_dig(31, 36, (uint8_t) (EventTimer / 60 % 60));
				WriteChar(49, 36, 10);
				print_two_dig(57, 36, (uint8_t) (EventTimer % 60));
			}
		} else {
			//Выводим на экран сообщение что счетчик оттикал
			WriteChar(23, 12, 10);
			WriteChar(36, 12, 10);
			WriteChar(49, 12, 10);

			WriteChar(05, 36, 10);
			WriteChar(15, 36, 10);
			WriteChar(23, 36, 10);
			WriteChar(31, 36, 10);
			WriteChar(41, 36, 10);
			WriteChar(49, 36, 10);
			WriteChar(57, 36, 10);
			WriteChar(67, 36, 10);
		}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Supply configuration update enable
	 */
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
	}
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1
			| RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void) {

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 39999;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 1599;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 9600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void) {

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart3) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD,
	VCC1_Pin | VCC0_Pin | CLK1_Pin | OE_Pin | CLK_Pin | STB_Pin | A_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, B_Pin | G2_Pin | B2_Pin | B1_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, R2_Pin | R1_Pin | G1_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : VCC1_Pin VCC0_Pin CLK1_Pin OE_Pin
	 CLK_Pin STB_Pin A_Pin */
	GPIO_InitStruct.Pin = VCC1_Pin | VCC0_Pin | CLK1_Pin | OE_Pin | CLK_Pin
			| STB_Pin | A_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pins : B_Pin G2_Pin B2_Pin B1_Pin */
	GPIO_InitStruct.Pin = B_Pin | G2_Pin | B2_Pin | B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : R2_Pin R1_Pin G1_Pin */
	GPIO_InitStruct.Pin = R2_Pin | R1_Pin | G1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
