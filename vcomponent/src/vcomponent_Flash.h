/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
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

 /*
 @brief Header file for the flash service implementation.
 */

#pragma once // Include guard

#include <com/rdk/hal/flash/IFlash.h>
#include <com/rdk/hal/flash/BnFlash.h>
#include <binder/BinderService.h>
#include <binder/IBinder.h>
#include <ut_kvp.h>

namespace com
{
    namespace rdk
    {
        namespace hal
        {
            namespace flash
            {

                class Flash : public android::BinderService<Flash>, public android::IBinder::DeathRecipient, public BnFlash
                {
                public:
                    static char const* getServiceName() { return IFlash::serviceName().c_str(); }

                    Flash();
                    virtual ~Flash();

                ::android::binder::Status flashImageFromFile(const ::std::string& filename, const ::android::sp<::com::rdk::hal::flash::IFlashListener>& listener, bool* _aidl_return) override;
                private:
                    void binderDied(const ::android::wp<::android::IBinder> &who) override;
                };

            } // namespace flash
        } // namespace hal
    } // namespace rdk
} // namespace com
extern ut_kvp_instance_t* ginstance;