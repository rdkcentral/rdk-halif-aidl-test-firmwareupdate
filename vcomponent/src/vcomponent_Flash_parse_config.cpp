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
* vcomponent_Flash_parse_config.cpp
*
* Implementation of vcomponent Flash driver
*/

#include "vcomponent_Flash_parse_config.h"
//#include "ut_kvp.h"

static std::string prefix;
static std::string key;
static std::string postfix;

ut_kvp_instance_t* ginstance;
static ut_controlPlane_instance_t *gCPInstance = NULL;
flashConfig_t gFlashConfig = {};

static void UT_ControlPlane_Sigint_Handler(int sig)
{
    UT_LOG("Signal Handler invoked\n");
    UT_ControlPlane_Exit(gCPInstance);
}

void vcomponent_Flash_initialize_ControlPlane()
{
    if(gCPInstance == NULL)
    {
        gCPInstance = UT_ControlPlane_Init(8080); // Example port number
        if(!gCPInstance)
        {
            UT_LOG_ERROR("[VCOMPONENT_FLASH]Failed to create control plane instance");
            return;
        }
        signal(SIGINT, UT_ControlPlane_Sigint_Handler);
        UT_LOG_INFO("[VCOMPONENT_FLASH]Control plane instance created");
    }
}

static void* vcomponent_Flash_kvp_create_instance(char *fileName)
{
    ut_kvp_status_t result;

    ginstance = ut_kvp_createInstance();

    if(!ginstance)
    {
        UT_LOG_ERROR("[VCOMPONENT_FLASH]Failed to create ut kvp instance");
        return nullptr;
    }

    result = ut_kvp_open(ginstance, fileName);

    if(result != UT_KVP_STATUS_SUCCESS)
    {
        UT_LOG_ERROR("[VCOMPONENT_Flash]Failed to open test configuration file");
        ut_kvp_destroyInstance(ginstance);
        return nullptr;
    }

    return (ut_kvp_instance_t*)ginstance;
}

static void vcomponent_Flash_kvp_destroy_instance(void *instance)
{
    if(!instance)
    {
        UT_LOG_ERROR("[VCOMPONENT_FLASH]Instance is null, cannot destroy");
        return;
    }
    UT_LOG_INFO("[VCOMPONENT_FLASH]Destroying kvp instance");
    ut_kvp_destroyInstance((ut_kvp_instance_t*)instance);
}

void vcomponent_Flash_destroy_ControlPlane()
{
    if(gCPInstance)
    {
        UT_ControlPlane_Exit(gCPInstance);
        UT_LOG_INFO("[VCOMPONENT_FLASH]Control plane instance destroyed");
        gCPInstance = NULL;
    }
}

// ut_kvp_instance_t* vcomponent_Flash_get_kvp_instance()
// {
//     return ginstance;
// }
void testYAMLCallback(char *key, ut_kvp_instance_t *instance, void* userData)
{
    char* kvpData;
    UT_LOG("*******************************Inside testYAMLCallback************************\n");
    kvpData = ut_kvp_getData(instance);
    const char *data = static_cast<const char *>(userData);

    if (kvpData != NULL)
    {
        // Print the emitted KVP string
        UT_LOG("%s\n", kvpData);

        // Free the emitted KVP string
        free(kvpData);
    }

    if (data != NULL)
    {
        UT_LOG("Original user data \n%s", data);
    }
    UT_LOG("*******************************************************************************\n");
}

void vcomponent_Flash_register_and_start_ControlPlane()
{
    const char* a = "Test Data";
    UT_ControlPlane_RegisterCallbackOnMessage(gCPInstance, "test/yamlData", &testYAMLCallback, (void*)a);
    UT_ControlPlane_Start(gCPInstance);
    UT_LOG_INFO("[VCOMPONENT_FLASH]Control plane started");
}

void vcomponent_Flash_parse_config(char *configurationFile)
{
    ut_kvp_instance_t* instance = (ut_kvp_instance_t*)vcomponent_Flash_kvp_create_instance(configurationFile);
    if(!instance)
    {
        UT_LOG_ERROR("[VCOMPONENT_FLASH]Failed to create kvp instance");
        return;
    }

    flashConfig_t devConfig = {};

    // Example of reading a string value from the configuration file
    prefix = "flash/SupportedFileProperties/";
    char value[256];
    memset(value, 0, sizeof(value));

    devConfig.filetype = ut_kvp_getStringField(instance, (prefix + "FILETYPE").c_str(), value, sizeof(value));
    devConfig.filepath = ut_kvp_getStringField(instance, (prefix + "FILEPATH").c_str(), value, sizeof(value));
    devConfig.filesignature = ut_kvp_getStringField(instance, (prefix + "FILESIGNATURE").c_str(), value, sizeof(value));
    devConfig.filesize = ut_kvp_getStringField(instance, (prefix + "FILESIZE").c_str(), value, sizeof(value));
    
    gFlashConfig = devConfig;
    // if(ut_kvp_getStringField(instance, fullKey.c_str(), value, sizeof(value)) == UT_KVP_STATUS_SUCCESS)
    // {
    //     UT_LOG("[VCOMPONENT_FLASH] %s: %s", fullKey.c_str(), value);
    // }
    // else
    // {
    //     UT_LOG_ERROR("[VCOMPONENT_FLASH] Failed to read %s from configuration file", fullKey.c_str());
    // }
    // UT_LOG("[VCOMPONENT_FLASH]Reading configuration for key: %s, value:  %s", fullKey.c_str(), value);

    // Add more keys as needed following the above pattern

    vcomponent_Flash_kvp_destroy_instance(instance);
}