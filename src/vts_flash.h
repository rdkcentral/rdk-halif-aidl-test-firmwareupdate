/**
 *  If not stated otherwise in this file or this component's LICENSE
 *  file the following copyright and licenses apply:
 *
 *  Copyright 2024 RDK Management
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
 * @addtogroup FLASH FLASH Module
 * @{
 */

/**
 * @addtogroup FLASH_HALTEST FLASH HAL Tests
 * @{
 */

/**
 * @defgroup FLASH_HALTEST_HEADER FLASH HAL Tests helper functions Header File
 * @{
 * @parblock
 *
 * ### Common functions/macros required for the flash :
 *
 * Common functions/macros required for the flash .
 *
 * **Pre-Conditions:**  None @n
 * **Dependencies:** None @n
 *
 * @endparblock
 *
 */

/**
 * @file vts_flash.h
 *
 */

#ifndef __VTS_FLASH_H__
#define __VTS_FLASH_H__

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <ut.h>
#include <ut_log.h>
#include <ut_kvp_profile.h>
#include <ut_control_plane.h>
#include <chrono>
#include <thread>

#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/Status.h>
#include <utils/String16.h>

#include "com/rdk/hal/flash/IFlash.h"
#include "com/rdk/hal/flash/BnFlashListener.h"
#include "com/rdk/hal/flash/IFlashListener.h"
#include "vts_flash_helper_functions.h"

using android::sp;
using android::String16;
using android::defaultServiceManager;
using android::binder::Status;
using namespace std;
using namespace std::chrono;

using namespace com::rdk::hal;
using namespace com::rdk::hal::flash;

/* =========================================================
 * SUCCESS MACRO
 * ========================================================= */
#define FLASHIMAGEFROMFILE_SUCCESS(flashHandle, filename, listener, maxFlashwaitDurationMs) \
    do { \
        ::android::binder::Status status; \
        bool _aidl_return = false; \
        UT_ASSERT_NOT_EQUAL_FATAL(listener.get(), nullptr); \
        UT_LOG_DEBUG("Calling flashImageFromFile filename[%s]", filename.c_str()); \
        status = (flashHandle)->flashImageFromFile(filename, listener, &_aidl_return); \
        UT_ASSERT_TRUE(status.isOk()); \
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
        UT_ASSERT_TRUE(_aidl_return); \
        WAIT_FOR_COMPLETION(listener, maxFlashwaitDurationMs); \
        UT_ASSERT_EQUAL(listener->result, FlashImageResult::SUCCESS); \
        UT_ASSERT_TRUE(!listener->progressValues.empty()); \
        UT_ASSERT_TRUE(listener->progressValues.front() == 0); \
        UT_ASSERT_TRUE(listener->progressValues.back() == 100); \
    } while (0)

/* =========================================================
 * NEGATIVE MACRO
 * ========================================================= */
#define FLASHIMAGEFROMFILE_NEGATIVE(flashHandle, filename, listener, expectedException) \
    do { \
        ::android::binder::Status status; \
        bool _aidl_return = false; \
        UT_LOG_DEBUG("Calling flashImageFromFile filename[%s]", filename.c_str()); \
        status = (flashHandle)->flashImageFromFile(filename, listener, &_aidl_return); \
        if (expectedException == Status::EX_NONE) \
        { \
            UT_ASSERT_TRUE(status.isOk()); \
        } \
        else \
        { \
            UT_ASSERT_FALSE(status.isOk()); \
        } \
        UT_LOG_INFO("Received: exceptionCode()[%d]", status.exceptionCode()); \
        UT_LOG_INFO("Expected: exceptionCode()[%d]", expectedException); \
        UT_LOG_INFO("Received AIDL return value: %d", _aidl_return); \
        \
        UT_ASSERT_EQUAL(status.exceptionCode(), expectedException); \
        if (expectedException == Status::EX_NONE) \
        { \
            UT_ASSERT_TRUE(_aidl_return); \
        } \
    } while (0)

#define WAIT_FOR_COMPLETION(listener, maxFlashwaitDurationMs)                          \
    do {                                                                                \
        auto start = std::chrono::steady_clock::now();                                  \
        while (!(listener)->completed) {                                                \
            auto now = std::chrono::steady_clock::now();                                \
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(       \
                now - start).count();                                                   \
                                                                                        \
            if (elapsed > (maxFlashwaitDurationMs)) {                                    \
                UT_LOG_ERROR("WAIT_FOR_COMPLETION timeout after %d ms",                 \
                             (maxFlashwaitDurationMs));                                  \
                break;                                                                  \
            }                                                                           \
                                                                                        \
            std::this_thread::sleep_for(std::chrono::milliseconds(100));                \
        }                                                                               \
        UT_ASSERT_TRUE((listener)->completed);                                          \
    } while(0)

/* =========================================================
 * TEST LISTENER 
 * ========================================================= */
class FlashListener : public BnFlashListener
{
public:
    FlashListener() = default;
    virtual ~FlashListener() = default;

    std::vector<int> progressSequence;
    FlashImageResult finalResult;
    bool completed;

    ::android::binder::Status onProgress(int32_t percentComplete) override;
    ::android::binder::Status onCompleted(::com::rdk::hal::flash::FlashImageResult result, const ::std::string& report) override;

private:
    std::mutex mMutex;
    std::condition_variable mCv;

};
#endif // __VTS_FLASH_H__

/** @} */ // End of FLASH_HALTEST_HEADER
/** @} */ // End of FLASH_HALTEST
/** @} */ // End of FLASH Module
/** @} */ // End of HPK