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
 * @addtogroup FLASH FLASH Module
 * @{
 */

/**
 * @addtogroup FLASH_HALTEST FLASH HAL Tests
 * @{
 */

/**
 * @defgroup FLASH_HALTEST_INIT FLASH HAL Tests helper functions
 * @{
 * @parblock
 *
 * ### Initialization functions for VTS tests :
 *
 * Initialization functions required for the module across all vendors.
 *
 * **Pre-Conditions:**  None @n
 * **Dependencies:** None @n
 *
 * @endparblock
 *
 */

/**
 * @file vts_flash_init.cpp
 *
 */

#include <ut.h>
#include <ut_log.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/Status.h>
#include <utils/String16.h>

#include "com/rdk/hal/flash/BpFlashListener.h"
#include "com/rdk/hal/flash/IFlash.h"
#include "com/rdk/hal/flash/FlashImageResult.h"
#include "com/rdk/hal/flash/IFlashListener.h"

#include "vts_flash_init.h"

using android::sp;
using android::String16;
using android::defaultServiceManager;
using android::binder::Status;

using namespace com::rdk::hal;
using namespace com::rdk::hal::flash;

sp<IFlash> gFLASHHandle = nullptr;

static UT_status_t vts_flash_getService()
{
   gFLASHHandle = android::interface_cast<IFlash>(
            defaultServiceManager()->getService(String16(IFlash::serviceName().c_str())));

    if (gFLASHHandle == nullptr)
    {
        UT_LOG_ERROR("FLASH Service not available");
        return UT_STATUS_FAILURE;
    }
    return UT_STATUS_OK;
}

UT_status_t VTS_FLASH_init()
{
    if(vts_flash_getService() != UT_STATUS_OK)
    {
        UT_LOG_ERROR("Failed to get FLASH Service");
        return UT_STATUS_FAILURE;
    }
    return UT_STATUS_OK;
}

void VTS_FLASH_destroy()
{
    UT_LOG_INFO("Destroying VTS_FLASH...");
    // De-initialization code goes here
}

/** @} */ // End of FLASH_HALTEST_INIT
/** @} */ // End of FLASH_HALTEST
/** @} */ // End of FLASH
/** @} */ // End of HPK