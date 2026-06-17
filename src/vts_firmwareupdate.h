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
 * @addtogroup FIRMWAREUPDATE FirmwareUpdate Module
 * @{
 */

/**
 * @addtogroup FIRMWAREUPDATE_HALTEST FirmwareUpdate HAL Tests
 * @{
 */

/**
 * @defgroup FIRMWAREUPDATE_HALTEST_HEADER FirmwareUpdate HAL Tests helper functions Header File
 * @{
 * @parblock
 *
 * ### Common functions/macros required for the firmware update:
 *
 * Common functions/macros required for the firmware update.
 *
 * **Pre-Conditions:**  None @n
 * **Dependencies:** None @n
 *
 * @endparblock
 *
 */

/**
 * @file vts_firmwareupdate.h
 *
 */

#ifndef __VTS_FIRMWAREUPDATE_H__
#define __VTS_FIRMWAREUPDATE_H__

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
#include <mutex>
#include <condition_variable>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/Status.h>
#include <utils/String16.h>

#include "com/rdk/hal/firmwareupdate/IFirmwareUpdate.h"
#include "com/rdk/hal/firmwareupdate/BnFirmwareUpdateListener.h"
#include "com/rdk/hal/firmwareupdate/IFirmwareUpdateListener.h"
#include "com/rdk/hal/firmwareupdate/FirmwareUpdateResult.h"
#include "vts_firmwareupdate_helper_functions.h"

using android::sp;
using android::String16;
using android::defaultServiceManager;
using android::binder::Status;
using namespace std;
using namespace std::chrono;

using namespace com::rdk::hal;
using namespace com::rdk::hal::firmwareupdate;

/* =========================================================
 * SUCCESS MACRO
 * ========================================================= */
#define FIRMWAREUPDATEFROMFILE_SUCCESS(firmwareUpdateHandle, filename, listener, maxFirmwareUpdateWaitDurationMs) \
    do { \
        ::android::binder::Status status; \
        bool _aidl_return = false; \
        UT_ASSERT_NOT_EQUAL_FATAL(listener.get(), nullptr); \
        UT_LOG_DEBUG("Calling updateFirmwareFromFile filename[%s]", filename.c_str()); \
        status = (firmwareUpdateHandle)->updateFirmwareFromFile(filename, listener, &_aidl_return); \
        UT_ASSERT_TRUE(status.isOk()); \
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
        UT_ASSERT_TRUE(_aidl_return); \
        WAIT_FOR_COMPLETION(listener, maxFirmwareUpdateWaitDurationMs); \
        UT_ASSERT_EQUAL(listener->mFinalResult, FirmwareUpdateResult::SUCCESS); \
        UT_ASSERT_TRUE(!listener->mProgressSequence.empty()); \
        UT_ASSERT_TRUE(listener->mProgressSequence.front() == 0); \
        UT_ASSERT_TRUE(listener->mProgressSequence.back() == 100); \
    } while (0)

/* =========================================================
 * NEGATIVE MACRO
 * ========================================================= */
#define FIRMWAREUPDATEFROMFILE_NEGATIVE(firmwareUpdateHandle, filename, listener, expectedException) \
    do { \
        ::android::binder::Status status; \
        bool _aidl_return = false; \
        UT_LOG_DEBUG("Calling updateFirmwareFromFile filename[%s]", filename.c_str()); \
        status = (firmwareUpdateHandle)->updateFirmwareFromFile(filename, listener, &_aidl_return); \
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

/* TODO: WAIT_FOR_COMPLETION
 *       Switch fully to condition_variable when glib versions
 *       are aligned across VTS, vDevice/vendor image, and AIDL builds
 *       Current implementation uses condition_variable but may need
 *       fallback depending on platform glib differences.
 */
#define WAIT_FOR_COMPLETION(listener, maxFirmwareUpdateWaitDurationMs)         \
    do {                                                                       \
                                                                      \
        bool completed = (listener)->waitForCompletion(maxFirmwareUpdateWaitDurationMs);\
                                                                               \
        if (!completed) {                                                      \
            UT_LOG_ERROR("WAIT_FOR_COMPLETION timeout after %d ms",           \
                         (maxFirmwareUpdateWaitDurationMs));                   \
        }                                                                      \
                                                                               \
        UT_ASSERT_TRUE((listener)->mCompleted);                                 \
    } while(0)

#define FIRMWAREUPDATEFROMFILE_VALIDATE_ERROR_RESULT(firmwareUpdateHandle, filename, listener, expectedError, waitMs) \
    do { \
        ::android::binder::Status status; \
        bool _aidl_return = false; \
        UT_ASSERT_NOT_EQUAL_FATAL(listener.get(), nullptr); \
        UT_LOG_DEBUG("Calling updateFirmwareFromFile filename[%s]", filename.c_str()); \
        status = (firmwareUpdateHandle)->updateFirmwareFromFile(filename, listener, &_aidl_return); \
        UT_ASSERT_TRUE(status.isOk()); \
        UT_ASSERT_EQUAL(status.exceptionCode(), Status::EX_NONE); \
        UT_ASSERT_TRUE(_aidl_return); \
        WAIT_FOR_COMPLETION(listener, waitMs); \
        UT_ASSERT_EQUAL(listener->mFinalResult, expectedError); \
        UT_ASSERT_NOT_EQUAL(listener->mFinalResult, FirmwareUpdateResult::SUCCESS); \
    } while (0)

/* =========================================================
 * TEST LISTENER 
 * ========================================================= */
class VtsFirmwareUpdateListener : public BnFirmwareUpdateListener
{
public:
    VtsFirmwareUpdateListener() = default;
    virtual ~VtsFirmwareUpdateListener() = default;

    std::vector<int> mProgressSequence;
    FirmwareUpdateResult mFinalResult = FirmwareUpdateResult::ERROR_GENERAL;
    bool mCompleted = false;

    ::android::binder::Status onProgress(int32_t percentComplete) override;
    ::android::binder::Status onCompleted(::com::rdk::hal::firmwareupdate::FirmwareUpdateResult result, const ::std::string& report) override;
    bool waitForCompletion(int maxWaitMs);
private:
    std::mutex mMutex;
    std::condition_variable mCv;

};
#endif // __VTS_FIRMWAREUPDATE_H__

/** @} */ // End of FIRMWAREUPDATE_HALTEST_HEADER
/** @} */ // End of FIRMWAREUPDATE_HALTEST
/** @} */ // End of FIRMWAREUPDATE Module
/** @} */ // End of HPK