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
 * @defgroup FIRMWAREUPDATE_HALTEST_INIT FirmwareUpdate HAL Tests helper functions
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
 * @file vts_firmwareupdate_init.cpp
 *
 */

#include <ut.h>
#include <ut_log.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/Status.h>
#include <utils/String16.h>

#include "com/rdk/hal/firmwareupdate/BpFirmwareUpdateListener.h"
#include "com/rdk/hal/firmwareupdate/IFirmwareUpdate.h"
#include "com/rdk/hal/firmwareupdate/FirmwareUpdateResult.h"
#include "com/rdk/hal/firmwareupdate/IFirmwareUpdateListener.h"

#include "vts_firmwareupdate_init.h"

using android::sp;
using android::String16;
using android::defaultServiceManager;
using android::binder::Status;

using namespace com::rdk::hal;
using namespace com::rdk::hal::firmwareupdate;

sp<IFirmwareUpdate> gFirmwareUpdateHandle = nullptr;

static UT_status_t vts_firmwareupdate_getService()
{
    gFirmwareUpdateHandle = android::interface_cast<IFirmwareUpdate>(
                defaultServiceManager()->getService(String16(IFirmwareUpdate::serviceName().c_str())));

    if (gFirmwareUpdateHandle == nullptr)
    {
        UT_LOG_ERROR("FIRMWAREUPDATE Service not available");
        return UT_STATUS_FAILURE;
    }
    return UT_STATUS_OK;
}

UT_status_t VTS_FIRMWAREUPDATE_init()
{
    if(vts_firmwareupdate_getService() != UT_STATUS_OK)
    {
        UT_LOG_ERROR("Failed to get FIRMWAREUPDATE Service");
        return UT_STATUS_FAILURE;
    }
    return UT_STATUS_OK;
}

void VTS_FIRMWAREUPDATE_destroy()
{
    UT_LOG_INFO("Destroying VTS_FIRMWAREUPDATE...");
    // De-initialization code goes here
}

/** @} */ // End of FIRMWAREUPDATE_HALTEST_INIT
/** @} */ // End of FIRMWAREUPDATE_HALTEST
/** @} */ // End of FIRMWAREUPDATE
/** @} */ // End of HPK