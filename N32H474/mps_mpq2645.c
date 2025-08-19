#include "n32h47x_48x.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include "mps_mpq2645.h" // Include balance module header file

#include "mps_delay.h"   // Use custom delay module



/* ===== Global Variable Definitions ===== */
 // Battery data
Battery batterycells[BATTERY_COUNT];
int32_t Active_balance_capacity[BATTERY_COUNT];  // Capacity changes from balancing (mAh)
int32_t Current_capacity[BATTERY_COUNT];  // Current capacity (mAh)

// Power loss parameters
float POWER_LOSS_1A = 0.5;           // Power loss at 1A current (W)
float POWER_LOSS_1_5A = 0.7;         // Power loss at 1.5A current (W)
float POWER_LOSS_2A = 0.9;           // Power loss at 2A current (W)
float POWER_LOSS_3A = 1.5;           // Power loss at 3A current (W)
int32_t delta_time_ms;
 

float GROUP_THRESHOLD  = 6000;   // Group balance threshold mAs
float INNER_THRESHOLD  = 2000;   // Inner balance threshold mAs
// Removed outRangeMax and outRangeMin variables
uint16_t balance_Current_GUI ;
uint16_t balance_Current_half;
uint16_t MP2645_command_num;//MPQ2645 control command
uint16_t ADDR[] = {3, 4, 5, 6, 7, 8, 9, 10};//MPQ2645 chip addresses
float totalSum = 0;         // Sum of all battery capacity offsets
float totalAvgOffset = 0;  // Average capacity offset of all batteries
int maxGroupIndex = 0;       // Index of group with maximum capacity offset
int minGroupIndex = 0;       // Index of group with minimum capacity offset

BatteryGroup groups;
//For applications with different number of cells, modify this configuration
const int groupRanges[5] = {0, 1, 2, 3, 4};

//Initialize the structure
void initBatteryGroup(BatteryGroup *groups)
{
  groups->startIndex = 0;
  groups->endIndex = 0;
  groups->cellnumber = 0;
  groups->avgOffset = 0;
  groups->maxOffset = 0;
  groups->minOffset = 0;
  groups->maxOffsetIndex = 0;
  groups->minOffsetIndex = 0;
  groups->state = Idle;  
  groups->switchNum = SWITCH_1;     
  groups->workMode = BUCK;   
  groups->current = 0;
}

// Calculate the average capacity offset of all batteries above the target battery within the group, excluding the target battery
float calculateUpperAverageForCell(int startIndex, int endIndex, int targetIndex) 
{
  if (targetIndex == endIndex) 
		{
    // If the target battery is the last one in the group, return its capacity offset
    return Current_capacity[targetIndex];
  }

  float sum = 0;
  int count = 0;

  // Iterate from the index after the target battery to the end index of the group
  for (int i = targetIndex + 1; i <= endIndex; i++) {
    sum += Current_capacity[i];
    count++;
  }

  // If no qualifying batteries, return 0
  return count > 0 ? sum / count : 0; 
}

// Calculate the average capacity offset of all batteries below the target battery within the group, excluding the target battery
float calculateLowerAverageForCell(int startIndex, int endIndex, int targetIndex) 
{
  if (targetIndex == startIndex) {
    // If the target battery is the first one in the group, return its capacity offset
    return Current_capacity[targetIndex];
  }

  float sum = 0;
  int count = 0;

  // Iterate from the start index of the group to the index before the target battery
  for (int i = startIndex; i < targetIndex; i++) {
    sum += Current_capacity[i];
    count++;
  }

  // If no qualifying batteries, return 0
  return count > 0 ? sum / count : 0; 
}


// Initialize battery group information
void initializeBatteryGroups(int32_t* Current_capacity, BatteryGroup* groups) 
{
  maxGroupIndex = 0;
  minGroupIndex = 0;
  totalSum = 0;
 
  // Set start and end positions for the group
  groups->startIndex = groupRanges[0];
  groups->endIndex = groupRanges[4]; // Use the last value in groupRanges
    
  float sum = 0;
  groups->cellnumber = groups->endIndex - groups->startIndex + 1;
    
  // Assign the start value to max and min
  groups->maxOffset = Current_capacity[groups->startIndex];
  groups->minOffset = Current_capacity[groups->startIndex];
  groups->maxOffsetIndex = groups->startIndex;
  groups->minOffsetIndex = groups->startIndex;
    
  // Update the max and min values
  for(int j = groups->startIndex; j <= groups->endIndex; j++) 
	 {
    sum += Current_capacity[j];
      
    if(Current_capacity[j] > groups->maxOffset) {
      groups->maxOffset = Current_capacity[j];
      groups->maxOffsetIndex = j;
    }
      
    if(Current_capacity[j] < groups->minOffset) {
      groups->minOffset = Current_capacity[j];
      groups->minOffsetIndex = j;
    }
  }
    
  // Calculate the average
  groups->avgOffset = sum / groups->cellnumber;
  
  // Set the total sum and average
  totalSum = groups->avgOffset;
  totalAvgOffset = totalSum;
				
  // Initialize switch, work state, etc.
  groups->state = Idle;
  groups->switchNum = SWITCH_1;
  groups->workMode = BUCK;
}

void handleInnerBalance(BatteryGroup* group) 
{
	  double maxUpperAvg,maxLowerAvg,minUpperAvg,minLowerAvg;
	  group->state = INNER_BALANCE;
	 // Calculate which deviates further from the average: the maximum or minimum capacity offset battery
  float maxDeviation = group->maxOffset - group->avgOffset;
  float minDeviation = group->avgOffset - group->minOffset;

  // Calculate the relative position of the target battery within its group
  int maxPositionInGroup = group->maxOffsetIndex - group->startIndex;
  int minPositionInGroup = group->minOffsetIndex - group->startIndex;
		// If the maximum capacity offset battery deviates further from the average, handle the maximum capacity offset battery
  if (maxDeviation >= minDeviation) 
			{      
  maxUpperAvg = calculateUpperAverageForCell(group->startIndex, group->endIndex, group->maxOffsetIndex);					
  maxLowerAvg = calculateLowerAverageForCell(group->startIndex, group->endIndex, group->maxOffsetIndex);
			
			if (maxUpperAvg > maxLowerAvg) 
  {
      group->workMode = BUCK;
      group->switchNum = maxPositionInGroup; // Upper average is greater than lower, switch number is the target battery's position in the group-1
		
    } else {
      group->workMode = BOOST;
      group->switchNum = maxPositionInGroup + 1; // Lower average is greater than upper, switch number is the target battery's position in the group+1
		
    }
  } 
			else {
				
  // If the minimum capacity offset battery deviates further from the average, handle the minimum capacity offset battery
  minUpperAvg = calculateUpperAverageForCell(group->startIndex, group->endIndex, group->minOffsetIndex);
  minLowerAvg = calculateLowerAverageForCell(group->startIndex, group->endIndex, group->minOffsetIndex);

    if (minUpperAvg > minLowerAvg) {
      group->workMode = BUCK;
      group->switchNum = minPositionInGroup + 1; // Upper average is greater than lower, switch number is the target battery's position in the group-1
			
    } else {
      group->workMode = BOOST;
      group->switchNum = minPositionInGroup; // Lower average is greater than upper, switch number is the target battery's position in the group+1
		
    }
  }
}


// Main function: balance battery capacity offsets
void balanceBatteries(int32_t* Current_capacity) 
{
	
//Initialize the state of battery group
  initBatteryGroup(&groups);
	
  //Calculate the maximum, minimum, etc. of battery group based on capacity offset
  initializeBatteryGroups(Current_capacity, &groups);

  //Execute inner balance if not participating in group balance
  // Check if the difference between maximum and minimum capacity offsets within the group is greater than INNER_THRESHOLD
  if((groups.maxOffset - groups.minOffset) > 2*INNER_THRESHOLD) 
  {
  handleInnerBalance(&groups);
  }
  
  // Check if group needs balancing
  if(groups.state!=Idle)
  {
  CTRL_L; //Pull CTRL pin low during execution
  systick_delay_us(250); // MP2645 delay time
  
  //1. Send corresponding current setting to MPQ2645
  if(groups.current==balance_Current_GUI)
  {
    Send_ctrl_pulse(ADDR[0],21+balance_Current_GUI); // Set to 2A
  }
  else if(groups.current==balance_Current_half)
  {
    Send_ctrl_pulse(ADDR[0],21+balance_Current_half); // Set to 1A
  }		
  
  //2. Send corresponding switch number to MPQ2645
  if(groups.switchNum==1)MP2645_command_num=13;
  else if(groups.switchNum==2)MP2645_command_num=14;
  else if(groups.switchNum==3)MP2645_command_num=15;
  else if(groups.switchNum==4)MP2645_command_num=16;
  Send_ctrl_pulse(ADDR[0],MP2645_command_num);
			 		 
  //3. Send work mode to MPQ2645
  if(groups.workMode==1)MP2645_command_num=18;
  else if(groups.workMode==2)MP2645_command_num=19;
  else MP2645_command_num =0;	
  Send_ctrl_pulse(ADDR[0],MP2645_command_num);
	}
	
	
  systick_delay_ms(1000); // Execution time
	Send_ctrl_pulse(0,2); // Turn off all balancing
	CTRL_H;	// Pull CTRL high, stop execution, prepare for next MCU sampling
}
 
 /**
  * Update capacity changes from active balancing
  * Consistently use mAs as the unit for balancing capacity changes
  */
 void update_active_balance_capacity(uint16_t *cells) 
{
  memset(Active_balance_capacity, 0, sizeof(Active_balance_capacity)); 
  float balance_current = 2;
  // Update the capacity change for the battery group
  if (groups.state == Idle) 
  {
    // Skip if group is idle
  }
  else 
  {
    float i_Switch_Up = 0, i_Switch_Down = 0; //i_Switch_Up is the current for batteries above the switch boundary, i_Switch_Down is for batteries below
    float v_Switch_Up = 0, v_Switch_Down = 0; //v_Switch_Up is the voltage for batteries above the switch boundary, v_Switch_Down is for batteries below
    int bottom_end, top_start;
    int switch_pos = groups.switchNum;
    float power_loss;
     
    // Choose power loss based on current size
    if (groups.current == balance_Current_GUI) {//Is this effective?
      balance_current = 2; // A
      power_loss = POWER_LOSS_2A; // Use 2A power loss
    } else {
      balance_current = 1; // A
      power_loss = POWER_LOSS_1A; // Use 1A power loss
    }
     
    // Determine top and bottom battery groups based on switch position
    // Switch position indicates the switch is connected between cell(switch-1) and cell(switch)
    // Batteries before the switch belong to the bottom group, batteries after the switch belong to the top group
    bottom_end = groups.startIndex + switch_pos - 1;  // Last bottom battery
    top_start = groups.startIndex + switch_pos;       // First top battery
     
    // Battery voltage below the switch boundary
    for (int j = groups.startIndex; j <= bottom_end; j++) 
    {
      v_Switch_Down += cells[j] * ADC_LSB / 1000.0f; // Convert to V
    }
    // Battery voltage above the switch boundary
    for (int j = top_start; j <= groups.endIndex; j++) 
    {
      v_Switch_Up += cells[j] * ADC_LSB / 1000.0f; // Convert to V
    }
     
    // Calculate current based on different modes 
    if (groups.workMode == BUCK) 
    {
      // Buck mode formula:
      // i_Switch_Up + i_Switch_Down = Ibalance
      // Vbottom × i_Switch_Down + Ploss = Vtop × i_Switch_Up
      i_Switch_Down = (v_Switch_Up * balance_current - power_loss) / (v_Switch_Up + v_Switch_Down);
      i_Switch_Up = balance_current - i_Switch_Down;
       
      // Update battery charge/discharge amount (mAs)
      // Buck mode: top batteries discharge(-), bottom batteries charge(+)
       
      for (int j = groups.startIndex; j <= bottom_end; j++) {
        // Down area batteries charge(+)
        Active_balance_capacity[j] += (int32_t)(i_Switch_Down * BALANCE_EXECUTE_TIME);
      }
      for (int j = top_start; j <= groups.endIndex; j++) {
        // Up area batteries discharge(-)
        Active_balance_capacity[j] -= (int32_t)(i_Switch_Up * BALANCE_EXECUTE_TIME);
      }
    } 
    else if (groups.workMode == BOOST)
    {
      // Boost mode formula:
      // i_Switch_Up + i_Switch_Down = Ibalance
      // Vbottom × i_Switch_Down - Ploss = Vtop × i_Switch_Up
      i_Switch_Down = (v_Switch_Up * balance_current + power_loss) / (v_Switch_Up + v_Switch_Down);
      i_Switch_Up = balance_current - i_Switch_Down;
       
      // Update battery charge/discharge amount (mAs)
      // Boost mode: top batteries charge(+), bottom batteries discharge(-)             
      for (int j = groups.startIndex; j <= bottom_end; j++) {
        // bottom area batteries discharge(-)
        Active_balance_capacity[j] -= (int32_t)(i_Switch_Down * BALANCE_EXECUTE_TIME);
      }
      for (int j = top_start; j <= groups.endIndex; j++) {
        // top area batteries charge(+)
        Active_balance_capacity[j] += (int32_t)(i_Switch_Up * BALANCE_EXECUTE_TIME);
      }
    }		 
  }
}
	
	

void Send_ctrl_pulse(uint16_t ADDR_num, uint16_t Commond_num)
{
	systick_delay_us(5); 

	for(char i=0;i<ADDR_num;i++)// sent address
	{
		CTRL_H;
		systick_delay_us(5);
		CTRL_L;
		systick_delay_us(5);
	}
	systick_delay_us(50); //t_store=50us
	
	for(char i=0;i<Commond_num;i++)// send command
	{
		CTRL_H;
		systick_delay_us(5);
		CTRL_L;
		systick_delay_us(5);
	}
	systick_delay_us(50); //t_store=50us
}


