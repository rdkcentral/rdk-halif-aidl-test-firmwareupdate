#!/usr/bin/env python3
#** *****************************************************************************
# *
# * If not stated otherwise in this file or this component's LICENSE file the
# * following copyright and licenses apply:
# *
# * Copyright 2026 RDK Management
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# * http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *
#* ******************************************************************************

import os
import sys

dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(dir_path, "../"))

from raft.framework.plugins.ut_raft.configRead import ConfigRead
from raft.framework.core.logModule import logModule
from vts_flash_helper import flashHelperClass

FLASH_L3_TEST_SUITE_NAME = "VTS_L3_FLASH"

class VTS_FLASH_L3_ConcurrentImage(flashHelperClass):
    """
    Validate all supported Flash image types
    """
    def __init__(self, log:logModule=None):
        """
        Initializes the test class
        """
        self.testName = "flash_concurrentReject"
        qcId = "1"
        super().__init__(self.testName, qcId, log)

    def testFunction(self):
        """
        Runs L3 Flash Test for all supported image types.

        Returns:
            bool: True if all tests pass, False otherwise.
        """
        self.utilities.change_directory(self.hal_session, self.targetWorkspace)
        result = False

        profileConfiguration = ConfigRead(self.moduleHFPFile, self.profile_moduleName)

        if profileConfiguration is None:
            raise ValueError("Failed to load HFP profile file: "
                            f"{self.moduleHFPFile}")

        # Validate supportedImages exists
        if not hasattr(profileConfiguration, "supportedImages"):
            raise ValueError("HFP profile does not contain 'supportedImages' field.")
        if not profileConfiguration.supportedImages:
            raise ValueError("No supported images found in HFP!")

        validImageMap = self.getValidImageList()
        imageType = profileConfiguration.supportedImages[0]
        imageFile = validImageMap.get(imageType)
        
        self.updateFlashTestConfig(imageFile, "SUCCESS")
        self.log.stepStart(f'Test Suit: {FLASH_L3_TEST_SUITE_NAME} Test Case: {self.testName}')
        self.log.step(f'Supported Image type: {imageType}, imageFile: {imageFile}')

        # Copy YAML to DUT
        self.utilities.sftpCopy(
            self.hal_session,
            self.l3TestConfigFile,
            self.l3TestConfigFolder
        )

        self.utMenu.start()
        output = self.utMenu.select(
            FLASH_L3_TEST_SUITE_NAME,
            self.testName,
            None,
            self.waitTime,
            False
        )

        result = self.utMenu.collect_results(output, True)
        self.log.stepResult(result, f'Test Suit: {FLASH_L3_TEST_SUITE_NAME} Test Case: {self.testName}')
        return result

if __name__ == '__main__':
    summaryLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summary"
    summaryLog  = logModule(summaryLogName, level=logModule.INFO)
    test = VTS_FLASH_L3_ConcurrentImage(summaryLog)
    test.run(False)
