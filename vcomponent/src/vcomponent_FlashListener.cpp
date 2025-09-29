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

/** @brief
* vcomponent_Flash.cpp
*
* Implementation of Flash service
*/
#include <iostream>
#include <thread>
#include <chrono>
#include <ut.h>
#include <ut_log.h>

#include "vcomponent_Flash_init.h"
#include "vcomponent_Flash.h"
#include "vcomponent_FlashListener.h"
#include "vcomponent_Flash_parse_config.h"


namespace com
{
    namespace rdk
    {
        namespace hal
        {
            namespace flash
            {
                FlashListener::FlashListener(Flash *parentFlash)
                {                  

                     FlashListener::~FlashListener()
                    {

                    }
                }
                ::android::binder::Status onProgress(int32_t percentComplete)
                {
                    
                }

                ::android::binder::Status onCompleted(::com::rdk::hal::flash::FlashImageResult result, const ::std::string& report)
                {
                    
                }
            }   //namespace flash
        }   //namespace hal
    }   //namespace rdk
}   //namespace com
