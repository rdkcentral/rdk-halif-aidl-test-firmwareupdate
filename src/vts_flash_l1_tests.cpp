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
 * @brief Test the negative and invalid invocation of flashImageFromFile API
 *
 * This test verifies that the flashImageFromFile API correctly handles invalid inputs such as:
 * invalid image type, invalid image size, empty filename, and null listener.
 * It also ensures that appropriate error codes and exceptions are returned,
 * and that listener callbacks behave correctly when applicable.
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 001@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:** Flash image files must be available under "/assets/flashfiles/input"@n
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |@n
 * | :----: | --------- | ---------- |-------------- | ----- |@n
 * | 01 | Parse the YAML configuration for negative test cases | Input: flashImageFromFile_neg section | Configuration is parsed successfully | Should Pass |@n
 * | 02 | Iterate through each negative test scenario from configuration | Input: filename, listener, expectedException | Test cases are executed one by one | Should Pass |@n
 * | 03 | Invoke flashImageFromFile API with invalid inputs | Input: invalid filename / empty filename / null listener | API returns expected failure or exception | Should Fail as expected |@n
 * | 04 | Validate listener behavior when listener is provided | Listener object | Listener does not receive success completion for invalid cases | Should Pass |@n
 * | 05 | Validate returned exception codes | Expected exceptionCode from YAML | Returned exception matches expected exception | Should Pass |@n
 * | 06 | If listener is present and no exception is thrown, wait for completion | Listener, timeout from config | Listener completes within timeout | Should Pass |@n
 * | 07 | Validate error codes from listener when applicable | Expected errorCode from YAML | Listener errorCode matches expected value | Should Pass |@n
 */

UT_ADD_TEST(VTS_L1_FLASH, flashImageFromFile_neg)
{
    flashTestConfiguration_t testConfig; // Single config instance to be filled by parser and used for testing
    UT_LOG_INFO("Starting Testcase: %s\n", "flashImageFromFile_neg");

    UT_ASSERT_TRUE(gFLASHHandle != nullptr);

    UT_status_t status = vts_flash_parse_config(
        VTS_FLASH_TEST_CONFIG_PATH,
        "Flash",
        "L1",
        "flashImageFromFile_neg",
        testConfig);

    UT_ASSERT_TRUE(status == UT_STATUS_OK);

    for(auto &test : testConfig.testConfigurations)
    {
        sp<FlashListener> listener = nullptr;

        // Create listener if it's expected to be non-null
        if(test.listener)
            listener = new FlashListener();
        else
            UT_LOG_INFO("Listener will be null for this test case as per configuration\n");
        FLASHIMAGEFROMFILE_NEGATIVE(
                gFLASHHandle,
                test.filename,
                listener,
                test.expectedException);

        // Check listener behavior based on the expected results
        if (listener != nullptr)
        {
            if (test.expectedResult == Status::EX_NONE)
            {
                WAIT_FOR_COMPLETION(listener, testConfig.testMaxFlashwaitDurationMs);

                UT_LOG_DEBUG("Negative case result: %d", listener->result);

                if (test.hasErrorCode)
                {
                    UT_LOG_DEBUG("Expected errorCode: %d, Actual: %d",
                        test.expectedErrorCode, listener->result);

                    UT_ASSERT_EQUAL(listener->result, test.expectedErrorCode);
                }
                else
                {
                    // fallback (older configs)
                    UT_ASSERT_NOT_EQUAL(listener->result, FlashImageResult::SUCCESS);
                }
            }
            else
            {
                UT_LOG_INFO("Listener not checked due to expected exception: %d\n", test.expectedException);
            }
        }
        else
        {
            UT_LOG_INFO("Listener is null as expected for this test case\n");
        }
    }
    UT_LOG_INFO("Executed Testcase: %s\n", "flashImageFromFile_neg");

} // End of UT_ADD_TEST

/**
 * @brief Test the positive and valid invocation of flashImageFromFile API
 *
 * This test verifies that the flashImageFromFile API successfully processes a valid flash image file.
 * It validates successful completion, progress callbacks from 0% to 100%, and correct final result.
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 002@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:** Valid flash image must be available under "/assets/flashfiles/input"@n
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |@n
 * | :----: | --------- | ---------- |-------------- | ----- |@n
 * | 01 | Parse the YAML configuration for positive test cases | Input: flashImageFromFile_pos section | Configuration is parsed successfully | Should Pass |@n
 * | 02 | Retrieve valid filename and listener from configuration | filename, listener flag | Valid inputs are retrieved | Should Pass |@n
 * | 03 | Invoke flashImageFromFile API with valid inputs | Valid filename and listener | API call returns success (no exception) | Should Pass |@n
 * | 04 | Register listener and monitor progress callbacks | Listener object | Progress callbacks received incrementally from 0% to 100% | Should Pass |@n
 * | 05 | Wait for flash completion using configured timeout | Listener, testMaxFlashwaitDurationMs | Completion is received within timeout | Should Pass |@n
 * | 06 | Validate final result from listener | listener->finalResult | Final result equals SUCCESS | Should Pass |@n
 * | 07 | Verify progress sequence reaches 100% | listener->progressSequence | Last progress value is 100 | Should Pass |@n
 */
UT_ADD_TEST(VTS_L1_FLASH, flashImageFromFile_pos)
{
    flashTestConfiguration_t testConfig;
    UT_LOG_INFO("Starting Testcase: %s\n", "flashImageFromFile_pos");
    UT_ASSERT_TRUE(gFLASHHandle != nullptr);

    UT_status_t status = vts_flash_parse_config(
        VTS_FLASH_TEST_CONFIG_PATH,
        "Flash",
        "L1",
        "flashImageFromFile_pos",
        testConfig);

    UT_ASSERT_TRUE(status == UT_STATUS_OK);

    for(auto &test : testConfig.testConfigurations)
    {
        sp<FlashListener> listener = nullptr;
        if(test.listener)
            listener = new FlashListener();

        FLASHIMAGEFROMFILE_SUCCESS(
            gFLASHHandle,
            test.filename,
            listener,
            testConfig.testMaxFlashwaitDurationMs);
        /* Optional debug */
        UT_LOG_DEBUG("Positive case completed: %s", test.filename.c_str());
    }
    UT_LOG_INFO("Executed Testcase: %s\n", "flashImageFromFile_pos");

}

/** @} */ // End of FLASH_HALTEST L1 File
/** @} */ // End of FLASH_HALTEST
/** @} */ // End of FLASH Module
/** @} */ // End of HPK
