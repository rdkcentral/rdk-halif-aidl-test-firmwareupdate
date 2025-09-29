/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2024 RDK Management
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
* @brief 
* vcomponent_Flash.cpp
* Implementation of the Flash service implementation
*/

#include "vcomponent_Flash.h"
#include "vcomponent_Flash_parse_config.h"
#include <regex>

namespace com
{
    namespace rdk
    {
        namespace hal
        {
            namespace flash
            {

                Flash::Flash()
                {
                }
                Flash::~Flash()
                {
                }

                ::android::binder::Status Flash::flashImageFromFile(const ::std::string& filename, const ::android::sp<::com::rdk::hal::flash::IFlashListener>& listener, bool* _aidl_return)
                {
                    if (!_aidl_return)
                    {
                        return android::binder::Status::fromExceptionCode(android::binder::Status::EX_ILLEGAL_ARGUMENT);
                    }
                    if (!listener || !_aidl_return)
                    {
                        UT_LOG_ERROR("[VCOMPONENT_FLASH]flashImageFromFile: Invalid arguments");
                        *_aidl_return = false;
                        return android::binder::Status::fromExceptionCode(android::binder::Status::EX_NULL_POINTER);
                    }

                    return ::android::binder::Status::ok();
                }

                void Flash::binderDied(const ::android::wp<::android::IBinder> &who)
                {
                    // Handle binder death if necessary
                }

            } // namespace Flash
        } // namespace hal
    } // namespace rdk
} // namespace com