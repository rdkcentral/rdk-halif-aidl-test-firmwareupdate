/**
 *  If not stated otherwise in this file or this component's LICENSE
 *  file the following copyright and licenses apply:
 *
 *  Copyright 2026 RDK Management
 *
 *  Licensed under the Apache License, Version 2.0 (the License);
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an AS IS BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 * @addtogroup HPK Hardware Porting Kit
 * @{
 * @par The Hardware Porting Kit
 * HPK is the next evolution of the well-defined Hardware Abstraction Layer
 * (HAL), but augmented with more comprehensive documentation and test suites
 * that OEM or SOC vendors can use to self-certify their ports before taking
 * them to RDKM for validation or to an operator for final integration and
 * deployment. The Hardware Porting Kit effectively enables an OEM and/or SOC
 * vendor to self-certify their own Video Accelerator devices, with minimal RDKM
 * assistance.
 *
 */

/**
 * @addtogroup FIRMWAREUPDATE  Module
 * @{
 */

/**
 * @addtogroup FIRMWAREUPDATE_HALTEST FirmwareUpdate HAL Tests
 * @{
 */

/**
 * @defgroup FIRMWAREUPDATE_HALTEST_PARSE_TESTCONFIG FirmwareUpdate HAL Tests parse test configuration file
 * @{
 * @parblock
 *
 * ### Parse test configuration functions for FirmwareUpdate:
 *
 * Parse configuration functions required for the module across all vendors.
 *
 * **Pre-Conditions:**  None @n
 * **Dependencies:** None @n
 *
 * @endparblock
 *
 */

/**
 * @file vts_firmwareupdate_parse_testconfig.cpp
 *
 */

#include "vts_firmwareupdate_parse_testconfig.h"
#include <binder/Status.h>
#include <cstring>

using android::binder::Status;

/**
 * @brief Create ut_kvp instance
 */
static void* vcomponentkvp_create_instance(const char *fileName)
{
    ut_kvp_instance_t* instance;
    ut_kvp_status_t result;

    instance = ut_kvp_createInstance();

    if(!instance)
    {
        UT_LOG_ERROR("[VTS_FIRMWAREUPDATE] Failed to create ut_kvp instance");
        return nullptr;
    }

    result = ut_kvp_open(instance, (char *)fileName);

    if(result != UT_KVP_STATUS_SUCCESS)
    {
        UT_LOG_ERROR("[VTS_FIRMWAREUPDATE] Failed to open configuration file");
        ut_kvp_destroyInstance(instance);
        return nullptr;
    }

    return (void*)instance;
}

/**
 * @brief Destroy ut_kvp instance
 */
static void vcomponentkvp_destroy_instance(void *instance)
{
    ut_kvp_destroyInstance((ut_kvp_instance_t*)instance);
}

/* Convert YAML exception string to Binder Status exception */
static int32_t convertExceptionCode(const std::string &code)
{
    if(code == "EX_NONE") return Status::EX_NONE;
    if(code == "EX_NULL_POINTER") return Status::EX_NULL_POINTER;
    if(code == "EX_ILLEGAL_ARGUMENT") return Status::EX_ILLEGAL_ARGUMENT;
    if(code == "EX_ILLEGAL_STATE") return Status::EX_ILLEGAL_STATE;
    if(code == "EX_SECURITY") return Status::EX_SECURITY;

    UT_LOG_ERROR("[VTS_FIRMWAREUPDATE] Unknown exceptionCode: %s", code.c_str());
    return Status::EX_ILLEGAL_ARGUMENT;
}

static FirmwareUpdateResult convertErrorCode(const std::string& str)
{
    if (str == "SUCCESS")
        return FirmwareUpdateResult::SUCCESS;
    else if (str == "ERROR_IMAGE_INVALID_TYPE")
        return FirmwareUpdateResult::ERROR_IMAGE_INVALID_TYPE;
    else if (str == "ERROR_IMAGE_INVALID_SIZE")
        return FirmwareUpdateResult::ERROR_IMAGE_INVALID_SIZE;
    else if (str == "ERROR_IMAGE_INVALID_SIGNATURE")
        return FirmwareUpdateResult::ERROR_IMAGE_INVALID_SIGNATURE;
    else if (str == "ERROR_IMAGE_INVALID_PRODUCT")
        return FirmwareUpdateResult::ERROR_IMAGE_INVALID_PRODUCT;
    else if (str == "ERROR_FILE_OPEN_FAIL")
        return FirmwareUpdateResult::ERROR_FILE_OPEN_FAIL;
    else if (str == "ERROR_FW_UPDATE_WRITE_FAILED")
        return FirmwareUpdateResult::ERROR_FW_UPDATE_WRITE_FAILED;
    else if (str == "ERROR_FW_UPDATE_VERIFY_FAILED")
        return FirmwareUpdateResult::ERROR_FW_UPDATE_VERIFY_FAILED;
    else if (str == "ERROR_FW_UPDATE_VERIFY_SIGNATURE_FAILED")
        return FirmwareUpdateResult::ERROR_FW_UPDATE_VERIFY_SIGNATURE_FAILED;
    else
        return FirmwareUpdateResult::ERROR_GENERAL;
}

/* Parse single test configuration instance */
static UT_status_t vts_firmwareupdate_parse_testconfig_instance(
        void *instance,
        const std::string &firmwareUpdateKey,
        firmwareUpdateValidationConfiguration_t &testConfiguration)
{
    ut_kvp_instance_t* kvpInstance = (ut_kvp_instance_t*)instance;
    const std::string parametersKey = firmwareUpdateKey + ".Parameters";

    char buf[UT_KVP_MAX_ELEMENT_SIZE] = {0};

    /* filename */
    if(ut_kvp_getStringField(kvpInstance, (parametersKey + ".filename").c_str(), buf, sizeof(buf)) != UT_KVP_STATUS_SUCCESS)
    {
        UT_LOG_ERROR("[VTS_FIRMWAREUPDATE] Failed to read filename for key %s", (parametersKey + ".filename").c_str());
        return UT_STATUS_FAILURE;
    }
    testConfiguration.filename = std::string(buf);

    /* listener */
    testConfiguration.listener = ut_kvp_getBoolField(kvpInstance, (parametersKey + ".listener").c_str());

    /* step_percent */
    testConfiguration.stepPercent = ut_kvp_getUInt32Field(kvpInstance, (parametersKey + ".step_percent").c_str());

    /* Expected results */
    const std::string expectedKey = firmwareUpdateKey + ".Expected_Results";

    if(ut_kvp_getStringField(kvpInstance, (expectedKey + ".exceptionCode").c_str(), buf, sizeof(buf)) != UT_KVP_STATUS_SUCCESS)
    {
        UT_LOG_ERROR("[VTS_FIRMWAREUPDATE] Failed to read exceptionCode for key %s", (expectedKey + ".exceptionCode").c_str());
        return UT_STATUS_FAILURE;
    }
    testConfiguration.expectedException = convertExceptionCode(std::string(buf));

    testConfiguration.expectedResult = ut_kvp_getBoolField(kvpInstance, (expectedKey + ".result").c_str());
    /* errorCode (optional) */
    memset(buf, 0, sizeof(buf));
    if (ut_kvp_getStringField(
            kvpInstance,
            (expectedKey + ".errorCode").c_str(),
            buf,
            sizeof(buf)) == UT_KVP_STATUS_SUCCESS)
    {
        testConfiguration.expectedErrorCode = convertErrorCode(std::string(buf));
        testConfiguration.hasErrorCode = true;

        UT_LOG_DEBUG("[VTS_FIRMWAREUPDATE] Parsed errorCode: %s[%d]", buf, testConfiguration.expectedErrorCode);
    }
    else
    {
        testConfiguration.hasErrorCode = false;
    }
    return UT_STATUS_OK;
}

/* Parse FirmwareUpdate test configuration file */
UT_status_t vts_firmwareupdate_parse_config(
        const char *configurationFile,
        const char *module,
        const char *testGroup,
        const char *testName,
        firmwareUpdateTestConfiguration_t &outConfig)
{
    void *testconfig = vcomponentkvp_create_instance(configurationFile);
    if(!testconfig)
    {
        UT_LOG_ERROR("[VTS_FIRMWAREUPDATE] Failed to create test configuration instance");
        return UT_STATUS_FAILURE;
    }

    ut_kvp_instance_t* kvpInstance = (ut_kvp_instance_t*)testconfig;
    outConfig.testMaxFirmwareUpdateWaitDurationMs = ut_kvp_getUInt32Field(
        kvpInstance,
        "FirmwareUpdate.overall.maxFirmwareUpdateWaitDurationMs");
    if (outConfig.testMaxFirmwareUpdateWaitDurationMs == 0)
    {
        UT_LOG_WARNING("[VTS_FIRMWAREUPDATE] maxFirmwareUpdateWaitDurationMs not set in config, using default 30000 ms");
        outConfig.testMaxFirmwareUpdateWaitDurationMs = 30000; // default to 30 seconds
    }
    outConfig.testMaxProgressWaitDurationMs = ut_kvp_getUInt32Field(
        kvpInstance,
        "FirmwareUpdate.overall.maxProgressWaitDurationMs");
    if (outConfig.testMaxProgressWaitDurationMs == 0)
    {
        UT_LOG_WARNING("[VTS_FIRMWAREUPDATE] maxProgressWaitDurationMs not set in config, using default 300 ms");
        outConfig.testMaxProgressWaitDurationMs = 300; // default to 300 ms
    }

    {
        firmwareUpdateValidationConfiguration_t testConfiguration;
        std::string baseKey = "FirmwareUpdate.L3.Test";

        UT_status_t status = vts_firmwareupdate_parse_testconfig_instance(kvpInstance, baseKey, testConfiguration);
        if(status != UT_STATUS_OK)
        {
            UT_LOG_ERROR("[VTS_FIRMWAREUPDATE] Failed to parse config: %s", baseKey.c_str());
            vcomponentkvp_destroy_instance(testconfig);
            return status;
        }
        outConfig.testConfigurations.push_back(testConfiguration);
    }
    vcomponentkvp_destroy_instance(testconfig);
    return UT_STATUS_OK;
}

/** @} */ // End of FIRMWAREUPDATE_HALTEST_PARSE_TESTCONFIG
/** @} */ // End of FIRMWAREUPDATE_HALTEST
/** @} */ // End of FIRMWAREUPDATE
/** @} */ // End of HPK