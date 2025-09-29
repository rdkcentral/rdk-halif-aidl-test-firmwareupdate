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

/** @brief
* vcomponent_flash.h
*
* Header file for the Flash service implementation
*/

#ifndef __FLASHLISTENER_H__
#define __FLASHLISTENER_H__

#include <com/rdk/hal/flash/BnFlash.h>
#include <com/rdk/hal/flash/IFlash.h>
#include <com/rdk/hal/flash/BnFlashListener.h>
#include <binder/BinderService.h>
#include <optional>
#include <string>
#include <utils/StrongPointer.h>
#include <vector>
#include <mutex>

using namespace std;
namespace com
{
    namespace rdk
    {
        namespace hal
        {
            namespace flash
            {
                class FlashListener :public android::BinderService<FlashListener>,
                                              public BnFlashListener
                {
                private:
                    Flash *mFlash;
                public:
                    FlashListener(Flash *parentFlash);
                    virtual ~FlashListener();

               public:
                    ::android::binder::Status onProgress(int32_t percentComplete);
                    ::android::binder::Status onCompleted(::com::rdk::hal::flash::FlashImageResult result, const ::std::string& report);
                };
            }   //namespace flash
        }   //namespace hal
    }   //namespace rdk
}   //namespace com

#endif //__FLASHLISTENER_H__
