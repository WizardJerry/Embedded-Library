#ifndef MP2645_H_
#define MP2645_H_
#include "n32h47x_48x.h"
#include "n32h47x_48x_gpio.h"
#include <stdint.h>

/* CTRL Pin Definition */
#define CTRL_PIN                 GPIO_PIN_1
#define CTRL_GPIO                GPIOA
#define CTRL_GPIO_RCC            RCC_AHB_PERIPHEN_GPIOA

/* CTRL Pin Control Macros */
#define CTRL_H                   GPIO_SetBits(CTRL_GPIO, CTRL_PIN)
#define CTRL_L                   GPIO_ResetBits(CTRL_GPIO, CTRL_PIN)


#define BATTERY_COUNT      16
#define GROUP_COUNT         4    // Number of battery groups
#define ADC_LSB            0.15259f   // ADC minimum resolution, unit mV
#define BALANCE_EXECUTE_TIME  1000    // Balance execution time, unit ms

// Battery structure definition
typedef struct {
    uint16_t voltage;       // Battery voltage
    uint8_t temperature;    // Battery temperature
} Battery;

extern int32_t Current_capacity[BATTERY_COUNT]; // Store capacity offset values for all batteries

extern uint16_t balance_Current_GUI;
extern uint16_t balance_Current_half;

// Work mode enumeration: BUCK and BOOST
typedef enum {
    BUCK = 1,
    BOOST =2
} WorkMode;

// Switch type enumeration: four different switch states
typedef enum {
    SWITCH_1 = 1,
    SWITCH_2 = 2,
    SWITCH_3 = 3,
    SWITCH_4 = 4
} SwitchType;

// Balance state enumeration: group balance and inner balance
typedef enum {
   	Idle,
    GROUP_BALANCE,  // Balance between groups
    INNER_BALANCE   // Balance within group
} BalanceState;

// Battery group structure definition, including start/end battery indices, average capacity offset, max/min capacity offset and their indices, etc.
typedef struct {
    int startIndex;         // Group start battery index
    int endIndex;          // Group end battery index
	  int cellnumber;        // Number of cells in each group
    float avgOffset;        // Group average capacity offset
    float maxOffset;        // Maximum capacity offset within group
    float minOffset;        // Minimum capacity offset within group
    int maxOffsetIndex;     // Index of battery with maximum capacity offset
    int minOffsetIndex;     // Index of battery with minimum capacity offset
    BalanceState state;    // Balance state
    SwitchType switchNum;  // Switch number used
    WorkMode workMode;     // Work mode
	  uint16_t current;
} BatteryGroup;


void balanceBatteries(int32_t* Current_capacity);
void initializeBatteryGroups(int32_t* Current_capacity, BatteryGroup* groups);
void handleInnerBalance(BatteryGroup* group);
float calculateUpperAverageForCell(int startIndex, int endIndex, int targetIndex);
float calculateLowerAverageForCell(int startIndex, int endIndex, int targetIndex);
void initBatteryGroup(BatteryGroup *groups);
void update_active_balance_capacity(uint16_t *cells);//cell is battery voltage(hexadecimal read from register)
void Send_ctrl_pulse(uint16_t ADDR_num, uint16_t Commond_num);
#endif