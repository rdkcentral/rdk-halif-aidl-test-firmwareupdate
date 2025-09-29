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
* @brief Tests the
*
* This test verifies that the
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 001@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** None@n
* @n
* **Test Procedure:**@n
* |Variation / Step|Description|Test Data|Expected Result|Notes|
* |----------------|-----------|---------|---------------|-----|
* |01||None||Should Pass|

*/

UT_ADD_TEST(VTS_L1_FLASH, flashImageFromFile_pos)
{
    //filename = android::String16("/data/flashfile_1_0.bin");    
    
    sp<IFlashListener> listener = new FlashListener();         

     FLASHIMAGEFROMFILE_SUCCESS(gFLASHHandle,"/data/flashfile_1_0.bin",listener);     
    
}

UT_ADD_TEST(VTS_L1_FLASH, flashImageFromFile_neg)
{
    android::String16 filename("NULL");
    
    sp<IFlashListener> FlashListener = NULL;  

    //FLASHIMAGEFROMFILE_FAILURE(gFLASHHandle, filename,listener);    
    
}

/** @} */ // End of FLASH_HALTEST L1 File
/** @} */ // End of FLASH_HALTEST
/** @} */ // End of FLASH Module
/** @} */ // End of HPK
