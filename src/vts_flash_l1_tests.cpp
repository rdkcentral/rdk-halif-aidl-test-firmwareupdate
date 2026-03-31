/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
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
 * @addtogroup FLASH Module
 * @{
 *
 */

/**
 * @defgroup FLASH_HALTEST FLASH HAL Tests
 * @{
 *
 */

/**
 * @defgroup FLAH_HALTEST_L1 FLASH HAL Tests L1 File
 * @{
 * @parblock
 *
 * ### L1 Test Cases for FLASH HAL :
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
 * @file vts_flash_l1_tests.c
 *
 */
#include "vts_flash.h"
#include "vts_flash_parse_testconfig.h"

extern sp<IFlash> gFLASHHandle;

/* Test class registration with UT_Core */
UT_ADD_TEST_TO_GROUP(VTS_L1_FLASH, UT_TESTS_L1)

class VTS_L1_FLASH : public UTCore
{
public:
    VTS_L1_FLASH() : UTCore() {}

    ~VTS_L1_FLASH() override = default;

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
 * @brief Test invalid parameter handling of flashImageFromFile API (L1 scope)
 *
 * This test verifies that the flashImageFromFile API correctly handles
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
 * **Dependencies:** IFlash service must be initialized@n
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
 * - No successful flash operation occurs for invalid inputs@n
 */

 UT_ADD_TEST(VTS_L1_FLASH, flashImageFromFile_invalid_params)
{
    UT_ASSERT_TRUE(gFLASHHandle != nullptr);

    // 1. Null listener
    FLASHIMAGEFROMFILE_NEGATIVE(
        gFLASHHandle,
        std::string("valid_image.bin"),
        nullptr,
        Status::EX_NULL_POINTER
    );

    // 2. Empty filename
    {
        sp<FlashListener> listener = new FlashListener();

        FLASHIMAGEFROMFILE_NEGATIVE(
            gFLASHHandle,
            std::string(""),
            listener,
            Status::EX_NONE
        );

        WAIT_FOR_COMPLETION(listener, 5000);

        UT_ASSERT_EQUAL(listener->mFinalResult, FlashImageResult::ERROR_FILE_OPEN_FAIL);
    }

    // 3. Non-existent file
    {
        sp<FlashListener> listener = new FlashListener();

        FLASHIMAGEFROMFILE_NEGATIVE(
            gFLASHHandle,
            std::string("non_existent.bin"),
            listener,
            Status::EX_NONE
        );

        WAIT_FOR_COMPLETION(listener, 5000);

        UT_ASSERT_EQUAL(listener->mFinalResult, FlashImageResult::ERROR_FILE_OPEN_FAIL);
    }
}


/** @} */ // End of FLASH_HALTEST L1 File
/** @} */ // End of FLASH_HALTEST
/** @} */ // End of FLASH Module
/** @} */ // End of HPK
