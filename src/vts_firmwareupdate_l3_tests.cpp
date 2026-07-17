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
 * @defgroup FIRMWAREUPDATE_HALTEST_L3 FirmwareUpdate HAL Tests L3 File
 * @{
 * @parblock
 *
 * ### L3 Test Cases for FirmwareUpdate HAL:
 *
 * This module includes Level 3 functional tests (success and failure scenarios).
 * This is to ensure that the API meets the operational requirements of the module across all vendors.
 *
 * **Pre-Conditions:** @n
 * **Dependencies:** None@n
 *
 * @endparblock
 *
 */

/**
 * @file vts_firmwareupdate_l3_tests.cpp
 *
 */

#include "vts_firmwareupdate.h"
#include <atomic>
#include <condition_variable>
#include "vts_firmwareupdate_parse_testconfig.h"

extern sp<IFirmwareUpdate> gFirmwareUpdateHandle;

/* -------------------------
* Test Group
* ------------------------- */
UT_ADD_TEST_TO_GROUP(VTS_L3_FIRMWAREUPDATE, UT_TESTS_L3)
class VTS_L3_FIRMWAREUPDATE : public UTCore
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
 * @brief Test successful execution of updateFirmwareFromFile API with valid inputs
 *
 * This test verifies that the updateFirmwareFromFile API successfully updates from a valid image file,
 * receives progress callbacks from 0% to 100%, and completes with a SUCCESS result.
 *
 * **Test Group ID:** Advanced: 02@n
 * **Test Case ID:** 001@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:** Valid firmware update image must be available under "/assets/firmwareupdatefiles/input"@n
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |@n
 * | :----: | --------- | ---------- |-------------- | ----- |@n
 * | 01 | Parse YAML configuration for firmware update test | FirmwareUpdate.L3.Test | Configuration is parsed successfully | Should Pass |@n
 * | 02 | Retrieve valid filename and listener flag from configuration | filename, listener | Valid inputs are retrieved | Should Pass |@n
 * | 03 | Invoke updateFirmwareFromFile API with valid inputs | filename, listener | API returns success status (isOk() == true) and result flag true | Should Pass |@n
 * | 04 | Register listener and monitor progress callbacks | VtsFirmwareUpdateListener | Progress callbacks received incrementally from 0% to 100% | Should Pass |@n
 * | 05 | Wait for completion using configured timeout | listener, testMaxFirmwareUpdateWaitDurationMs | Completion callback is received within timeout | Should Pass |@n
 * | 06 | Validate progress sequence | listener->mProgressSequence | First value is 0 and last value is 100 | Should Pass |@n
 * | 07 | Validate final result | listener->mFinalResult | Final result equals SUCCESS | Should Pass |
 */
UT_ADD_TEST(VTS_L3_FIRMWAREUPDATE, firmwareupdate_imageTypes)
{
    firmwareUpdateTestConfiguration_t testConfig;

    UT_status_t cfgStatus = vts_firmwareupdate_parse_config(
        VTS_FIRMWAREUPDATE_TEST_CONFIG_PATH,
        "FirmwareUpdate",
        "L3",
        "Test",
        testConfig);

    UT_ASSERT_EQUAL(cfgStatus, UT_STATUS_OK);
    UT_ASSERT_FALSE(testConfig.testConfigurations.empty());

    auto &test = testConfig.testConfigurations[0];
    sp<VtsFirmwareUpdateListener> listener = new VtsFirmwareUpdateListener();

    UT_LOG_DEBUG("Updating firmware from image: %s", test.filename.c_str());

    FIRMWAREUPDATEFROMFILE_SUCCESS(
        gFirmwareUpdateHandle,
        test.filename,
        listener,
        testConfig.testMaxFirmwareUpdateWaitDurationMs);
}
/**
 * @brief Test concurrent rejection behavior of updateFirmwareFromFile
 *
 * This test verifies that a second update request is rejected while one firmware update is already active.
 *
 * **Test Group ID:** Advanced: 02@n
 * **Test Case ID:** 001@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:** Valid firmware update image must be available under "/assets/firmwareupdatefiles/input"@n
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |@n
 * | :----: | --------- | ---------- |-------------- | ----- |@n
 * | 01 | Parse YAML configuration for firmware update test | FirmwareUpdate.L3.Test | Configuration is parsed successfully | Should Pass |@n
 * | 02 | Retrieve valid filename and listener flag from configuration | filename, listener | Valid inputs are retrieved | Should Pass |@n
 * | 03 | Invoke updateFirmwareFromFile API with valid inputs | filename, listener | API returns success status (isOk() == true) and result flag true | Should Pass |@n
 * | 04 | Register listener and monitor progress callbacks | VtsFirmwareUpdateListener | Progress callbacks received incrementally from 0% to 100% | Should Pass |@n
 * | 05 | Wait for completion using configured timeout | listener, testMaxFirmwareUpdateWaitDurationMs | Completion callback is received within timeout | Should Pass |@n
 * | 06 | Validate progress sequence | listener->mProgressSequence | First value is 0 and last value is 100 | Should Pass |@n
 * | 07 | Validate final result | listener->mFinalResult | Final result equals SUCCESS | Should Pass |
 */
UT_ADD_TEST(VTS_L3_FIRMWAREUPDATE, firmwareupdate_concurrentReject)
{
    firmwareUpdateTestConfiguration_t testConfig;

    UT_status_t cfgStatus = vts_firmwareupdate_parse_config(
        VTS_FIRMWAREUPDATE_TEST_CONFIG_PATH,
        "FirmwareUpdate",
        "L3",
        "Test",
        testConfig);

    UT_ASSERT_EQUAL(cfgStatus, UT_STATUS_OK);
    UT_ASSERT_FALSE(testConfig.testConfigurations.empty());

    auto &test = testConfig.testConfigurations[0];
    sp<VtsFirmwareUpdateListener> listener1 = new VtsFirmwareUpdateListener();
    sp<VtsFirmwareUpdateListener> listener2 = new VtsFirmwareUpdateListener();
    bool ret1 = false, ret2 = false;

    // Start first firmware update
    Status s1 = gFirmwareUpdateHandle->updateFirmwareFromFile(
        test.filename,
        listener1,
           &ret1);

    UT_ASSERT_TRUE(s1.isOk());
    UT_ASSERT_TRUE(ret1);

    // Attempt second firmware update immediately
    Status s2 = gFirmwareUpdateHandle->updateFirmwareFromFile(
        test.filename,
        listener2,
        &ret2);
    UT_ASSERT_TRUE(s2.isOk());
    UT_ASSERT_FALSE(ret2); // must reject

    // Only first should complete
    WAIT_FOR_COMPLETION(listener1, testConfig.testMaxFirmwareUpdateWaitDurationMs);

    UT_ASSERT_EQUAL(listener1->mFinalResult, FirmwareUpdateResult::SUCCESS);
}

/**
 * @brief Test handling of firmware update write failure scenario
 *
 * This test verifies that the updateFirmwareFromFile API correctly handles write failures
 * and returns the appropriate error code ERROR_FW_UPDATE_WRITE_FAILED.
 *
 * **Test Group ID:** Advanced: 02@n
 * **Test Case ID:** 003@n
 * **Priority:** High@n
 *
 * **Pre-Conditions:** Fault injection must be enabled to simulate firmware update write failure@n
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |@n
 * | :----: | --------- | ---------- |-------------- | ----- |@n
 * | 01 | Parse YAML configuration for write failure test | FirmwareUpdate.L3.Test | Configuration is parsed successfully | Should Pass |@n
 * | 02 | Retrieve filename and listener from configuration | filename, listener | Inputs are valid | Should Pass |@n
 * | 03 | Invoke updateFirmwareFromFile API with failure-injected image | filename | API call returns success status but operation fails internally | Should Pass |@n
 * | 04 | Wait for completion callback | listener, timeout | Completion callback is received within timeout | Should Pass |@n
 * | 05 | Validate final result | listener->mFinalResult | Final result equals ERROR_FW_UPDATE_WRITE_FAILED | Should Pass |
 */
UT_ADD_TEST(VTS_L3_FIRMWAREUPDATE, firmwareupdate_validateAllErrorcodes)
{
    firmwareUpdateTestConfiguration_t testConfig;

    vts_firmwareupdate_parse_config(VTS_FIRMWAREUPDATE_TEST_CONFIG_PATH,
                           "FirmwareUpdate", "L3",
                           "Test",
                           testConfig);

    UT_ASSERT_FALSE(testConfig.testConfigurations.empty());
    auto &test = testConfig.testConfigurations[0];

    sp<VtsFirmwareUpdateListener> listener = new VtsFirmwareUpdateListener();

    FIRMWAREUPDATEFROMFILE_VALIDATE_ERROR_RESULT(
        gFirmwareUpdateHandle,
        test.filename,
        listener,
        test.expectedErrorCode,
        testConfig.testMaxFirmwareUpdateWaitDurationMs);
}
    /** @} */ // End of FIRMWAREUPDATE_HALTEST_L3
/** @} */ // End of FIRMWAREUPDATE_HALTEST
/** @} */ // End of FIRMWAREUPDATE Module
/** @} */ // End of HPK