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
#define SETTINGS_ADDRESS 0x81E0020
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint16_t TOnLP = 50;

uint8_t RAW_DISPLAY[10][32];/* = { 1, 32, 224, 8, 3, 48, 32, 24, 6, 16, 48, 16, 4,
 16, 16, 16, 8, 128, 8, 7, 8, 0, 8, 0, 12, 0, 16, 0, 7, 0, 48, 0, 0, 128,
 0, 1, 0, 192, 0, 1, 0, 248, 96, 1, 0, 128, 192, 0, 1, 128, 0, 0, 1, 192,
 0, 0, 1, 64, 0, 0, 1, 0, 0, 0, 0, 16, 0, 16, 0, 0, 0, 24, 7, 0, 224, 8,
 12, 0, 56, 12, 0, 48, 96, 0, 0, 252, 192, 31, 3, 0, 0, 0, 6, 0, 0, 0, 0,
 0, 0, 2, 0, 0, 0, 2, 3, 128, 192, 3, 6, 192, 64, 3, 0, 192, 96, 0, 0, 0,
 96, 0, 8, 0, 64, 0, 13, 0, 192, 0, 0, 48, 0, 2, 0, 16, 0, 2, 4, 16, 0,
 2, 4, 16, 0, 2, 8, 0, 192, 1, 15, 0, 192, 1, 0, 0, 128, 0, 0, 0, 128, 0,
 0, 32, 0, 0, 0, 32, 0, 0, 0, 32, 0, 0, 7, 224, 224, 3, 0, 16, 96, 3, 0,
 240, 32, 1, 0, 0, 32, 0, 0, 0, 96, 0, 0, 224, 0, 3, 0, 16, 0, 0, 0, 8,
 0, 0, 0, 12, 0, 0, 32, 56, 0, 16, 39, 240, 248, 8, 36, 128, 8, 7, 52, 0,
 8, 0, 0, 48, 0, 4, 0, 0, 0, 4, 3, 0, 144, 2, 6, 0, 240, 3, 1, 0, 240, 1,
 0, 0, 128, 0, 0, 0, 128, 0, 0, 0, 128, 0, 0, 64, 0, 8, 0, 64, 0, 12, 0,
 192, 0, 7, 3, 240, 240, 1, 24, 24, 192, 2, 16, 240, 64, 3, 16, 0, 64, 0,
 16, 0, 64, 0, 0, 48, 0, 3, 0, 16, 0, 6, 0, 16, 0, 4, 7, 48, 0, 4, 6,
 240, 32, 3, 3, 0, 224, 0, 0, 0, 32, 0, 0, 0, 96, 0 };*/
uint8_t ColorMatrix = 1;
int SetColorMatrix = 0;

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
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, SETTINGS_ADDRESS, &bp);
	HAL_FLASH_Lock();
}

void RestoreSettings() {
	int RSetting[8] = { 0 };
	memcpy(RSetting, SETTINGS_ADDRESS, 32);
	utc_timezone = RSetting[0];
	EventDate.tm_sec = RSetting[1];
	EventDate.tm_min = RSetting[2];
	EventDate.tm_hour = RSetting[3];
	EventDate.tm_mday = RSetting[4];
	EventDate.tm_mon = RSetting[5];
	EventDate.tm_year = RSetting[6];
	SetColorMatrix = RSetting[7];
	if (SetColorMatrix) {
		ColorMatrix = SetColorMatrix;
	}
}

void digitalWrite(GPIO_TypeDef *Port, uint16_t NumP, uint8_t stat) {
	if (stat) {
		HAL_GPIO_WritePin(Port, NumP, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(Port, NumP, GPIO_PIN_RESET);
	}
}
void SetColor(uint8_t Color) {
	ColorMatrix = Color;
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
	uint8_t N_p_adr = Xp / 16 + 5 * (Yp / 16);
	SetPixel(Xp % 16, Yp % 16, N_p_adr, ON);
}

void WriteChar(uint8_t x_s, uint8_t y_s, uint8_t char_id) {
	for (uint8_t x_b = 0; x_b < FONT_CHAR_WIDTH; x_b++) {
		for (uint8_t y_b = 0; y_b < FONT_CHAR_HEIGHT; y_b++) {
			SetPixelBigPanel(x_s + x_b, y_s + y_b,
					font_f_for16x16[(char_id - FONT_START_CHAR) * 8 + x_b]
							& ((uint16_t) 1 << y_b));
		}
	}
}

void Clk_data(uint8_t n_p, uint8_t x_b, uint8_t y_b, uint8_t bit_mask,
		uint8_t CM1, uint8_t CM2, uint8_t n_line) {
	uint16_t CLK_P;
	if (n_line) {
		CLK_P = CLK1_Pin;
		n_p += 5;
	} else {
		CLK_P = CLK_Pin;
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
		for (n_p = 0; n_p < 5; n_p++) {
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
#define SIZE_BF_SET 28
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
				ColorMatrix = SetColorMatrix;
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
#define SIZE_BF 600
char nmea_data[SIZE_BF] = { 0 };
#define Rx1Buf_SIZE 20
char Rx1Buf[Rx1Buf_SIZE] = { 0 };

void Set_Date_and_Time(int h, int m, int s, int dd, int mm, int yy) {
	NowTime.tm_sec = s; //секунды
	NowTime.tm_min = m; //минуты
	NowTime.tm_hour = h; //час
	NowTime.tm_mday = dd; //день
	NowTime.tm_mon = mm; //месяц
	NowTime.tm_year = yy; //год с 1900 года
}

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

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	if (huart->Instance == USART1) {

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
			if (time_err > 5) {
				NowTimeSec = NowTimeSec_new;
				time_err = 0;
			}
		} else {
			time_err = 0;
		}

		HAL_UART_Receive_IT(&huart3, (uint8_t*) &SET_data, SIZE_BF_SET - 1);
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

	//SetColor(4); //R
	//SetColor(2); //G
	//SetColor(1); //B
	//WriteChar(64, 0,'1');
	uint16_t timer_i = 0;
	uint8_t i_c = 0;
	char d_time[4];
	char s_time[9];
	while (1) {
		LoadAndShowBufOnPanel();

		timer_i++;
		if (SetColorMatrix == 0) {
			if (timer_i % 100 == 0) {
				if (i_c < 8) {
					SetColor(i_c);
				} else {
					i_c = 0;
				}
				i_c++;
			}
		}
		if (timer_i % 1 == 0) {

			if (EventTimeSec > NowTimeSec) {
				EventTimer = EventTimeSec - NowTimeSec;
				sprintf(d_time, "%03d", (int) (EventTimer / 86400));
				for (uint8_t sm_t = 0; sm_t < 3; sm_t++) {
					WriteChar(23 + sm_t * (FONT_CHAR_WIDTH + 5), 1,
							d_time[sm_t]);
				}
				sprintf(s_time, "%02d:%02d:%02d",
						(int) (EventTimer / 3600 % 24),
						(int) (EventTimer / 60 % 60), (int) (EventTimer % 60));
				for (uint8_t sm_t = 0; sm_t < 8; sm_t++) {
					WriteChar(1 + sm_t * (FONT_CHAR_WIDTH + 2), 17,
							s_time[sm_t]);
				}
			} else {
				//Выводим на экран сообщение что счетчик оттикал
				for (uint8_t sm_t = 0; sm_t < 3; sm_t++) {
					WriteChar(23 + sm_t * (FONT_CHAR_WIDTH + 5), 1, ':');
				}
				for (uint8_t sm_t = 0; sm_t < 8; sm_t++) {
					WriteChar(1 + sm_t * (FONT_CHAR_WIDTH + 2), 17, ':');
				}
			}

			/*
			 //Для отладки выводи время NowTime
			 sprintf(d_time, "%03d", (int) (NowTime.tm_year));
			 for (uint8_t sm_t = 0; sm_t < 3; sm_t++) {
			 WriteChar(64+sm_t * 4, 2, d_time[sm_t]);
			 }

			 if (1) {
			 sprintf(s_time, "%02d:%02d:%02d", (int) (NowTime.tm_hour) + 7,
			 (int) (NowTime.tm_min), (int) (NowTime.tm_sec));
			 for (uint8_t sm_t = 0; sm_t < 8; sm_t++) {
			 WriteChar(1 + sm_t * (FONT_CHAR_WIDTH + 2), 17,
			 s_time[sm_t]);
			 }
			 } else {
			 sprintf(s_time, "%02d:%02d:%02d", (int) (NowTime.tm_mday),
			 (int) (NowTime.tm_mon) + 1, (int) (NowTime.tm_year));
			 for (uint8_t sm_t = 0; sm_t < 8; sm_t++) {
			 WriteChar(1 + sm_t * (FONT_CHAR_WIDTH + 2), 17,
			 s_time[sm_t]);
			 }
			 }
			 */
			//NowTimeSec = mktime(&NowTime) + utc_timezone * 3600;
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
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
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
