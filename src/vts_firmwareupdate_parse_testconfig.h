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
 * @file vts_firmwareupdate_parse_testconfig.h
 *
 */

#ifndef __VTS_FIRMWAREUPDATE_PARSE_TESTCONFIG_H__
#define __VTS_FIRMWAREUPDATE_PARSE_TESTCONFIG_H__

#include <ut.h>
#include <ut_log.h>
#include "ut_kvp_profile.h"
#include "com/rdk/hal/firmwareupdate/IFirmwareUpdate.h"
#include "com/rdk/hal/firmwareupdate/FirmwareUpdateResult.h"

using namespace std;
using namespace com::rdk::hal::firmwareupdate;

typedef struct
{
    // Parameters
    std::string     filename;
    bool            listener{true};
    int        stepPercent{10};
    // EX_NONE(0) means the IPC call returned ok().
    int32_t         expectedException{0};

    // For EX_NONE:
    // - expectedResult == true  → background firmware update started (progress/completed may fire)
    // - expectedResult == false →  (no callbacks must fire)
    bool            expectedResult{true};

    // Whether any listener callbacks are expected for this vector
    bool            callbacksExpected{true};

    // Optional final onCompleted() result when callbacksExpected is true
    FirmwareUpdateResult expectedErrorCode{FirmwareUpdateResult::SUCCESS};
    bool             hasErrorCode{false};

} firmwareUpdateValidationConfiguration_t;

typedef struct
{
    std::vector<firmwareUpdateValidationConfiguration_t> testConfigurations;
    int32_t testMaxFirmwareUpdateWaitDurationMs{30000};
    int32_t testMaxProgressWaitDurationMs{0};
} firmwareUpdateTestConfiguration_t;
#define VTS_FIRMWAREUPDATE_TEST_CONFIG_PATH "./test_configuration/vts_l3_firmwareupdate.yaml"

UT_status_t vts_firmwareupdate_parse_config(const char *configurationFile, const char *module, const char *testGroup,
                                       const char *testName, firmwareUpdateTestConfiguration_t &outConfig);
#endif //__VTS_FIRMWAREUPDATE_PARSE_TESTCONFIG_H__

/** @} */ // End of FIRMWAREUPDATE_HALTEST_PARSE_TESTCONFIG_HEADER
/** @} */ // End of FIRMWAREUPDATE_HALTEST
/** @} */ // End of FIRMWAREUPDATE
/** @} */ // End of HPK