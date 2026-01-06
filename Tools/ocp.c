#include "ocp.h"


/**
 * @brief initialize the OCP controller instance
 * @param ocp conntroller instance pointer
 * @param samplePeriodS OCP check cycle period in seconds
 * @param ratedCurrent standard rated max current in Amps
 */
void ocp_init(OcpController_t *ocp, float samplePeriodS, float ratedCurrent) {
    ocp->stageCount = 0;
    ocp->ratedCurrent = ratedCurrent;
    ocp->samplePeriodS = samplePeriodS;

    uint32_t i;
    for (i = 0; i < OCP_MAX_STAGES; i++) {
        ocp->stages[i].bucketVal = 0;
        ocp->stages[i].isTriggered = false;
        ocp->stages[i].thresholdCached = 0.0f;
    }
}

/**
 * @brief 配置一个过流保护层级
 * @param multiplier 过流倍数 (例如 1.2f 表示 1.2倍额定电流)
 * @param delaySeconds 持续时间 (秒)
 * @param leakFactor 漏水因子 (散热速率，1为线性时间恢复)
 */
bool ocp_config_stage(OcpController_t *ocp, uint8_t index, float multiplier, float delaySeconds, uint32_t leakFactor) {
    if (index >= OCP_MAX_STAGES) return false;

    OcpStage_t *stage = &ocp->stages[index];

    stage->multiplier = multiplier;
    stage->thresholdCached = multiplier * ocp->ratedCurrent;

    stage->fillRate = 1;
    stage->leakRate = leakFactor; 
    stage->bucketVal = 0;
    stage->isTriggered = false;

    if (delaySeconds <= ocp->samplePeriodS) {
        stage->tripLimitTicks = 0;
    } else {
        stage->tripLimitTicks = (uint32_t)(delaySeconds / ocp->samplePeriodS);
    }

    if (index + 1 > ocp->stageCount) {
        ocp->stageCount = index + 1;
    }

    return true;
}


/**
 * @brief update the rated current used for OCP calculations
 * @details change the rated current, and update all stage thresholds,
 *          keep bucket values and triggered states unchanged.
 */
void ocp_current_update(OcpController_t *ocp, float newRatedCurrent) {
    if (newRatedCurrent < 0.0f) return;

    ocp->ratedCurrent = newRatedCurrent;
    uint32_t i;
    for (i = 0; i < ocp->stageCount; i++) {
        ocp->stages[i].thresholdCached = ocp->stages[i].multiplier * newRatedCurrent;
    }
}


/**
 * @brief cyclically check for over-current condition
 * @return true means fault detected
 */
bool ocp_check(OcpController_t *ocp, float currentFeedback) {
    bool faultDetected = false;
    float absCurrent = (currentFeedback < 0.0f) ? -currentFeedback : currentFeedback;
    uint32_t i;
    for (i = 0; i < ocp->stageCount; i++) {
        OcpStage_t *stage = &ocp->stages[i];

        if (absCurrent > stage->thresholdCached) {
            if (stage->tripLimitTicks == 0) {
                stage->isTriggered = true;
                faultDetected = true;
                return faultDetected;
            }

            // accumulate bucket
            if (stage->bucketVal < stage->tripLimitTicks) {
                stage->bucketVal += stage->fillRate;
            } else {
                stage->isTriggered = true;
                faultDetected = true;
            }
        } else {
            // leak bucket
            if (stage->bucketVal > 0 && !stage->isTriggered) {
                if (stage->bucketVal >= stage->leakRate) {
                    stage->bucketVal -= stage->leakRate;
                } else {
                    stage->bucketVal = 0;
                }
            }
        }
    }

    return faultDetected;
}


/**
 * @brief reset all OCP stages
 */
void ocp_reset(OcpController_t *ocp) {
    uint32_t i;
    for (i = 0; i < OCP_MAX_STAGES; i++) {
        ocp->stages[i].bucketVal = 0;
        ocp->stages[i].isTriggered = false;
    }
}