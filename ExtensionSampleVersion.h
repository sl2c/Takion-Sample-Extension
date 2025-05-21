#pragma once

#include "TakionVersionBase.h"

#define EXTENSIONSAMPLE_MAJORNUMBER          1
#define EXTENSIONSAMPLE_MINORNUMBER          0
#define EXTENSIONSAMPLE_BUILDNUMBER          8
#define EXTENSIONSAMPLE_MODIFICATIONNUMBER   3

#define ExtensionSampleVersion EXTENSIONSAMPLE_MAJORNUMBER,EXTENSIONSAMPLE_MINORNUMBER,EXTENSIONSAMPLE_BUILDNUMBER,EXTENSIONSAMPLE_MODIFICATIONNUMBER
#define ExtensionSampleVersionDot EXTENSIONSAMPLE_MAJORNUMBER.EXTENSIONSAMPLE_MINORNUMBER.EXTENSIONSAMPLE_BUILDNUMBER.EXTENSIONSAMPLE_MODIFICATIONNUMBER
#define ExtensionSampleVersionStr T_STRINGIZE(ExtensionSampleVersionDot)


#define ExtensionSampleProductName TakionExtensionSample
#define ExtensionSampleProductNameStr T_STRINGIZE(ExtensionSampleProductName)
#define ExtensionSampleFileNameStr T_STRINGIZE(ExtensionSampleProductName.dll)

#ifdef TAKION_NO_OPTIONS
#define ExtensionSamplePlusVersionDot ExtensionSampleProductName NoOption ExtensionSampleVersionDot TakionPlatform TakionConfiguration
#else
#define ExtensionSamplePlusVersionDot ExtensionSampleProductName ExtensionSampleVersionDot TakionPlatform TakionConfiguration
#endif
#define ExtensionSampleDescriptionStr T_STRINGIZE(ExtensionSamplePlusVersionDot)
