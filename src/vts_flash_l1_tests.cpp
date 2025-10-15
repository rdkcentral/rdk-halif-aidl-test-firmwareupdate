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
        //android::ProcessState::self()->startThreadPool();
    }

    void TearDown() override
    {
    }
};

/**
* @brief Verify that flashImageFromFile correctly processes valid flash image file paths
*
* This test validates the flashImageFromFile API by passing valid filename and valid listener.
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 001@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** flash_image should be placed under predefined path "/asserts/flashfiles/input"
* **Dependencies:** None@n
* **User Interaction:** None@n
* @n
* **Test Procedure:**@n
* |Variation / Step|Description|Test Data|Expected Result|Notes|
* |----------------|-----------|---------|---------------|-----|
* |01||Define filenames with valid, invalid and empty string values|call FLASHIMAGEFROMFILE_FAILURE|Should fail|
  |02||Create NULL flash listener |call FLASHIMAGEFROMFILE_FAILURE|Should fail|

*/

UT_ADD_TEST(VTS_L1_FLASH, flashImageFromFile_neg)
{
    std::string fileName1 = {"/asserts/input/flash_valid_image2.DRI"};
    std::string fileName2 = {" "};
    std::string fileName3 = {"/asserts/flashfiles/input/flash_valid_image2.DRI"};
    sp<IFlashListener> listener1 = new FlashListener();
    sp<IFlashListener> listener2 = NULL;

   // Test case 1: Pass file with wrong file path 

    FLASHIMAGEFROMFILE_FAILURE(gFLASHHandle,fileName1,listener1,Status::EX_ILLEGAL_ARGUMENT);

    // Test case 2: Pass filename as empty string 

    FLASHIMAGEFROMFILE_FAILURE(gFLASHHandle,fileName2,listener1,Status::EX_ILLEGAL_ARGUMENT);

   //Test case 2: Send NULL listener

    FLASHIMAGEFROMFILE_FAILURE(gFLASHHandle,fileName3,listener2,Status::EX_NULL_POINTER);
}


/** @} */ // End of FLASH_HALTEST L1 File
/** @} */ // End of FLASH_HALTEST
/** @} */ // End of FLASH Module
/** @} */ // End of HPK
