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
 * @file vts_flash_l1_tests.cpp
 *
 */

#include "vts_flash.h"
#include <atomic>
#include <condition_variable>
#include "vts_flash_parse_testconfig.h"

extern sp<IFlash> gFLASHHandle;

/* -------------------------
* Test Group
* ------------------------- */
UT_ADD_TEST_TO_GROUP(VTS_L3_FLASH, UT_TESTS_L3)
class VTS_L3_FLASH : public UTCore
{
public:

    void SetUp() override
    {

        UT_LOG_INFO("====================L3 Test Start====================\n");
        UT_LOG_INFO(
            "\t\tTest name: %s\n", 
            ::testing::UnitTest::GetInstance()->current_test_info()->name()
        );
        UT_LOG_INFO("===================================================\n");

        android::ProcessState::self()->startThreadPool();
    }
    void TearDown() override
    {
        UT_LOG_INFO("====================L3 Test End====================\n");
        UT_LOG_INFO(
            "\t\tTest name: %s\n", 
            ::testing::UnitTest::GetInstance()->current_test_info()->name()
        );
        UT_LOG_INFO("===================================================\n");
    }
};

/**
 * @brief Test successful execution of flashImageFromFile API with valid inputs
 *
 * This test verifies that the flashImageFromFile API successfully flashes a valid image file,
 * receives progress callbacks from 0% to 100%, and completes with a SUCCESS result.
 *
 * **Test Group ID:** Advanced: 02@n
 * **Test Case ID:** 001@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:** Valid flash image must be available under "/assets/flashfiles/input"@n
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |@n
 * | :----: | --------- | ---------- |-------------- | ----- |@n
 * | 01 | Parse YAML configuration for flash_success test | Flash.L3.flash_success | Configuration is parsed successfully | Should Pass |@n
 * | 02 | Retrieve valid filename and listener flag from configuration | filename, listener | Valid inputs are retrieved | Should Pass |@n
 * | 03 | Invoke flashImageFromFile API with valid inputs | filename, listener | API returns success status (isOk() == true) and result flag true | Should Pass |@n
 * | 04 | Register listener and monitor progress callbacks | FlashListener | Progress callbacks received incrementally from 0% to 100% | Should Pass |@n
 * | 05 | Wait for completion using configured timeout | listener, testMaxFlashwaitDurationMs | Completion callback is received within timeout | Should Pass |@n
 * | 06 | Validate progress sequence | listener->mProgressSequence | First value is 0 and last value is 100 | Should Pass |@n
 * | 07 | Validate final result | listener->mFinalResult | Final result equals SUCCESS | Should Pass |
 */
UT_ADD_TEST(VTS_L3_FLASH, flash_imageTypes)
{
    flashTestConfiguration_t testConfig;

    UT_status_t cfgStatus = vts_flash_parse_config(
        VTS_FLASH_TEST_CONFIG_PATH,
        "Flash",
        "L3",
        "Test",
        testConfig);

    UT_ASSERT_EQUAL(cfgStatus, UT_STATUS_OK);
    UT_ASSERT_FALSE(testConfig.testConfigurations.empty());

    auto &test = testConfig.testConfigurations[0];
    sp<FlashListener> listener = new FlashListener();

    UT_LOG_DEBUG("Flashing image: %s", test.filename.c_str());

    FLASHIMAGEFROMFILE_SUCCESS(
        gFLASHHandle,
        test.filename,
        listener,
        testConfig.testMaxFlashwaitDurationMs);
}
/**
 * @brief Test successful execution of flashImageFromFile API with valid inputs
 *
 * This test verifies that the flashImageFromFile API successfully flashes a valid image file,
 * receives progress callbacks from 0% to 100%, and completes with a SUCCESS result.
 *
 * **Test Group ID:** Advanced: 02@n
 * **Test Case ID:** 001@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:** Valid flash image must be available under "/assets/flashfiles/input"@n
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |@n
 * | :----: | --------- | ---------- |-------------- | ----- |@n
 * | 01 | Parse YAML configuration for flash_success test | Flash.L3.flash_success | Configuration is parsed successfully | Should Pass |@n
 * | 02 | Retrieve valid filename and listener flag from configuration | filename, listener | Valid inputs are retrieved | Should Pass |@n
 * | 03 | Invoke flashImageFromFile API with valid inputs | filename, listener | API returns success status (isOk() == true) and result flag true | Should Pass |@n
 * | 04 | Register listener and monitor progress callbacks | FlashListener | Progress callbacks received incrementally from 0% to 100% | Should Pass |@n
 * | 05 | Wait for completion using configured timeout | listener, testMaxFlashwaitDurationMs | Completion callback is received within timeout | Should Pass |@n
 * | 06 | Validate progress sequence | listener->mProgressSequence | First value is 0 and last value is 100 | Should Pass |@n
 * | 07 | Validate final result | listener->mFinalResult | Final result equals SUCCESS | Should Pass |
 */
UT_ADD_TEST(VTS_L3_FLASH, flash_concurrentReject)
{
    flashTestConfiguration_t testConfig;

    UT_status_t cfgStatus = vts_flash_parse_config(
        VTS_FLASH_TEST_CONFIG_PATH,
        "Flash",
        "L3",
        "Test",
        testConfig);

    UT_ASSERT_EQUAL(cfgStatus, UT_STATUS_OK);
    UT_ASSERT_FALSE(testConfig.testConfigurations.empty());

    auto &test = testConfig.testConfigurations[0];
    sp<FlashListener> listener1 = new FlashListener();
    sp<FlashListener> listener2 = new FlashListener();
    bool ret1 = false, ret2 = false;

    // Start first flash
    Status s1 = gFLASHHandle->flashImageFromFile(
        test.filename,
        listener1,
           &ret1);

    UT_ASSERT_TRUE(s1.isOk());
    UT_ASSERT_TRUE(ret1);

    // Attempt second flash immediately
    Status s2 = gFLASHHandle->flashImageFromFile(
        test.filename,
        listener2,
        &ret2);
    UT_ASSERT_TRUE(s2.isOk());
    UT_ASSERT_FALSE(ret2); // must reject

    // Only first should complete
    WAIT_FOR_COMPLETION(listener1, testConfig.testMaxFlashwaitDurationMs);

    UT_ASSERT_EQUAL(listener1->mFinalResult, FlashImageResult::SUCCESS);
}

/**
 * @brief Test handling of flash write failure scenario
 *
 * This test verifies that the flashImageFromFile API correctly handles write failures
 * and returns the appropriate error code ERROR_FLASH_WRITE_FAILED.
 *
 * **Test Group ID:** Advanced: 02@n
 * **Test Case ID:** 003@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:** Fault injection must be enabled to simulate flash write failure@n
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |@n
 * | :----: | --------- | ---------- |-------------- | ----- |@n
 * | 01 | Parse YAML configuration for write failure test | Flash.L3.flash_write_fail | Configuration is parsed successfully | Should Pass |@n
 * | 02 | Retrieve filename and listener from configuration | filename, listener | Inputs are valid | Should Pass |@n
 * | 03 | Invoke flashImageFromFile API with failure-injected image | filename | API call returns success status but operation fails internally | Should Pass |@n
 * | 04 | Wait for completion callback | listener, timeout | Completion callback is received within timeout | Should Pass |@n
 * | 05 | Validate final result | listener->mFinalResult | Final result equals ERROR_FLASH_WRITE_FAILED | Should Pass |
 */
UT_ADD_TEST(VTS_L3_FLASH, flash_validateAllErrorcodes)
{
    flashTestConfiguration_t testConfig;

    vts_flash_parse_config(VTS_FLASH_TEST_CONFIG_PATH,
                           "Flash", "L3",
                           "Test",
                           testConfig);

    UT_ASSERT_FALSE(testConfig.testConfigurations.empty());
    auto &test = testConfig.testConfigurations[0];

    sp<FlashListener> listener = new FlashListener();

    FLASHIMAGEFROMFILE_VALIDATE_ERROR_RESULT(
        gFLASHHandle,
        test.filename,
        listener,
        test.expectedErrorCode,
        testConfig.testMaxFlashwaitDurationMs);
}
/** @} */ // End of FLAH_HALTEST_L1
/** @} */ // End of FLASH_HALTEST
/** @} */ // End of FLASH Module
/** @} */ // End of HPK