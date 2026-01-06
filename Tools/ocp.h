#ifndef OCP_H
#define OCP_H

#include <stdint.h>
#include <stdbool.h>

#define OCP_MAX_STAGES 3 

typedef struct {
    float multiplier;          // [配置] 过流倍数 (例如 1.25, 1.5)
    float thresholdCached;     // [运行时缓存] 计算出的绝对电流阈值 (Amps)，避免ISR中做乘法
    uint32_t tripLimitTicks;   // [预计算] 触发保护所需的计数值 (桶深)
    uint32_t bucketVal;        // [运行时] 当前漏桶累积值 (水位)
    uint32_t fillRate;         // [配置] 注水速率 (通常为1)
    uint32_t leakRate;         // [配置] 漏水速率 (散热速率)
    bool isTriggered;          // [运行时] 是否触发标志
} OcpStage_t;

typedef struct {
    OcpStage_t stages[OCP_MAX_STAGES]; // 保护层级数组
    uint32_t stageCount;                // 实际使用的层级数
    float samplePeriodS;                // 采样周期 (1/Freq)
    float ratedCurrent;                 // 系统当前的额定电流 (基准值)
} OcpController_t;


void ocp_init(OcpController_t*ocp, float samplePeriodS, float ratedCurrent);
bool ocp_config_stage(OcpController_t*ocp, uint8_t index, float multiplier, float delaySeconds, uint32_t leakFactor);
void ocp_current_update(OcpController_t*ocp, float newRatedCurrent);
bool ocp_check(OcpController_t*ocp, float currentFeedback);
void ocp_reset(OcpController_t*ocp);

#endif