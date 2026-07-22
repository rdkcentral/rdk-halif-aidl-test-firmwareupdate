/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2026 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/**
 * @addtogroup HPK Hardware Porting Kit
 * @{
 *
 */

/**
 * @addtogroup FIRMWAREUPDATE Module
 * @{
 *
 */

/**
 * @defgroup FIRMWAREUPDATE_HALTEST FirmwareUpdate HAL Tests
 * @{
 *
 */

/**
 * @defgroup FIRMWAREUPDATE_HALTEST_L1 FirmwareUpdate HAL Tests L1 File
 * @{
 * @parblock
 *
 * ### L1 Test Cases for FirmwareUpdate HAL:
 *
 * This module includes Level 1 functional tests (success and failure scenarios).
 * This is to ensure that the API meets the operational requirements of the module across all vendors.
 *
 * **Pre-Conditions:** @n
 * **Dependencies:** None@n
 *
 * @endparblock
 *
 */

/**
 * @file vts_firmwareupdate_l1_tests.cpp
 *
 */
#include "vts_firmwareupdate.h"
#include "vts_firmwareupdate_parse_testconfig.h"

extern sp<IFirmwareUpdate> gFirmwareUpdateHandle;

/* Test class registration with UT_Core */
UT_ADD_TEST_TO_GROUP(VTS_L1_FIRMWAREUPDATE, UT_TESTS_L1)

class VTS_L1_FIRMWAREUPDATE : public UTCore
{
public:
    VTS_L1_FIRMWAREUPDATE() : UTCore() {}

    ~VTS_L1_FIRMWAREUPDATE() override = default;

    void SetUp() override
    {
        UT_LOG_INFO("====================L1 Test Start====================\n");
        UT_LOG_INFO(
            "\t\tTest name: %s\n",
            ::testing::UnitTest::GetInstance()->current_test_info()->name()
        );
        UT_LOG_INFO("===================================================\n");

        android::ProcessState::self()->startThreadPool();
    }

    void TearDown() override
    {
        UT_LOG_INFO("====================L1 Test End====================\n");
        UT_LOG_INFO(
            "\t\tTest name: %s\n",
            ::testing::UnitTest::GetInstance()->current_test_info()->name()
        );
        UT_LOG_INFO("===================================================\n");
    }
};

/**
 * @brief Test invalid parameter handling of updateFirmwareFromFile API (L1 scope)
 *
 * This test verifies that the updateFirmwareFromFile API correctly handles
 * invalid input parameters and adheres to the AIDL contract for exception handling.
 *
 * L1 scope is limited to validating:
 * - Null listener handling
 * - Empty filename
 * - Non-existent file path
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 001@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:** None@n
 * **Dependencies:** IFirmwareUpdate service must be initialized@n
 * **User Interaction:** None@n
 *
 * **Test Procedure:**@n
 * | Step | Description | Input | Expected Result |@n
 * | :--: | ----------- | ----- | --------------- |@n
 * | 01 | Invoke API with null listener | Valid filename, null listener | Returns EX_NULL_POINTER |@n
 * | 02 | Invoke API with empty filename | "", valid listener | Returns EX_NONE, listener gets failure |@n
 * | 03 | Invoke API with non-existent file | Invalid path, valid listener | Returns EX_NONE, listener gets ERROR_FILE_OPEN_FAIL |@n
 * | 04 | Validate AIDL return value | _aidl_return | TRUE when no exception |@n
 * | 05 | Validate listener completion (if applicable) | listener->result | Not SUCCESS |@n
 *
 * **Expected Outcome:**@n
 * - API rejects invalid parameters with correct exception codes@n
 * - Listener is invoked only when applicable@n
 * - No successful firmware update operation occurs for invalid inputs@n
 */

 UT_ADD_TEST(VTS_L1_FIRMWAREUPDATE, updateFirmwareFromFile_neg)
{
    UT_ASSERT_TRUE(gFirmwareUpdateHandle != nullptr);

    // 1. Null listener
    FIRMWAREUPDATEFROMFILE_NEGATIVE(
        gFirmwareUpdateHandle,
        std::string("valid_image.bin"),
        nullptr,
        Status::EX_NULL_POINTER
    );

    // 2. Empty filename
    {
        sp<VtsFirmwareUpdateListener> listener = new VtsFirmwareUpdateListener();

        FIRMWAREUPDATEFROMFILE_NEGATIVE(
            gFirmwareUpdateHandle,
            std::string(""),
            listener,
            Status::EX_NONE
        );

        WAIT_FOR_COMPLETION(listener, 5000);

        UT_ASSERT_EQUAL(listener->mFinalResult, FirmwareUpdateResult::ERROR_FILE_OPEN_FAIL);
    }

    // 3. Non-existent file
    {
        sp<VtsFirmwareUpdateListener> listener = new VtsFirmwareUpdateListener();

        FIRMWAREUPDATEFROMFILE_NEGATIVE(
            gFirmwareUpdateHandle,
            std::string("non_existent.bin"),
            listener,
            Status::EX_NONE
        );

        WAIT_FOR_COMPLETION(listener, 5000);

        UT_ASSERT_EQUAL(listener->mFinalResult, FirmwareUpdateResult::ERROR_FILE_OPEN_FAIL);
    }
}


/** @} */ // End of FIRMWAREUPDATE_HALTEST_L1
/** @} */ // End of FIRMWAREUPDATE_HALTEST
/** @} */ // End of FIRMWAREUPDATE Module
/** @} */ // End of HPK
