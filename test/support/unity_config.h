#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

// Define SOC_CPU_INTR_NUM to avoid redefinition issues
#define SOC_CPU_INTR_NUM 32

// Define this to avoid redefinition warnings from sdkconfig.h
#ifndef CONFIG_FREERTOS_HZ
#define CONFIG_FREERTOS_HZ 1000
#endif

// Define this to avoid redefinition warnings from unity_config.h
#ifndef UNITY_EXCLUDE_FLOAT
#define UNITY_EXCLUDE_FLOAT 1
#endif

// Any other Unity configuration settings you need

#endif // UNITY_CONFIG_H