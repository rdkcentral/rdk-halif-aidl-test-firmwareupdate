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
 * @defgroup FIRMWAREUPDATE_HALTEST_HELPER_HEADER FirmwareUpdate HAL Tests helper functions Header File
 * @{
 * @parblock
 *
 * ### Common functions/macros required for the FirmwareUpdate module :
 *
 * Common functions/macros required for the FirmwareUpdate module.
 *
 * **Pre-Conditions:**  None @n
 * **Dependencies:** None @n
 *
 * @endparblock
 *
 */

/**
 * @file vts_firmwareupdate.cpp
 *
 */

#include "vts_firmwareupdate.h"
#include "vts_firmwareupdate_helper_functions.h"

/* Implementation of FirmwareUpdate listener methods */
android::binder::Status VtsFirmwareUpdateListener::onProgress(int32_t percentComplete)
{
    UT_LOG_INFO("VtsFirmwareUpdateListener::onProgress called with percentComplete[%d]",percentComplete);
    std::lock_guard<std::mutex> lock(mMutex);
    mProgressSequence.push_back(percentComplete);
    UT_LOG_INFO("[L3] onProgress: %d%%\n", percentComplete);
    mCv.notify_all();
    return ::android::binder::Status::ok();
}

android::binder::Status VtsFirmwareUpdateListener::onCompleted(
    FirmwareUpdateResult result,
    const std::string& report)
{
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mFinalResult = result;
        mCompleted = true;
        UT_LOG_INFO("[L3] onCompleted: result=%d report=%s\n", result, report.c_str());
    }
    mCv.notify_all();
    return ::android::binder::Status::ok();
}

bool VtsFirmwareUpdateListener::waitForCompletion(int maxWaitMs)
{
    std::unique_lock<std::mutex> lock(mMutex);

    return mCv.wait_for(lock,
                        std::chrono::milliseconds(maxWaitMs),
                        [this]() { return mCompleted; });
}
/** @} */ // End of FIRMWAREUPDATE_HALTEST_HEADER
/** @} */ // End of FIRMWAREUPDATE_HALTEST
/** @} */ // End of FIRMWAREUPDATE
/** @} */ // End of HPK