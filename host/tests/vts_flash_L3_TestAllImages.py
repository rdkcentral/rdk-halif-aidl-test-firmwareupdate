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

class VTS_FLASH_L3_TestAllImages(flashHelperClass):
    """
    Validate all supported Flash image types
    """
    def __init__(self, log:logModule=None,moduleName:str=None):
        """
        Initializes the test class
        """
        self.testName = "flash_imageTypes"
        qcId = "1"
        
        super().__init__(self.testName, qcId, log, moduleName)

    def testFunction(self):
        """
        Runs L3 Flash Test for all supported image types.

        Returns:
            bool: True if all tests pass, False otherwise.
        """
        self.utilities.change_directory(self.hal_session, self.targetWorkspace)
        result = False
        overallResult = True
        
        profileConfiguration = ConfigRead(self.paths["moduleHFPFile"], self.profile_moduleName)
        validImageMap = self.getValidImageList()

        for imageType in profileConfiguration.supportedImages:
            imageFile = validImageMap.get(imageType)
            stepPercent = profileConfiguration.progress.step_percent
            self.updateFlashTestConfig(imageFile, "SUCCESS", stepPercent)
            self.log.stepStart(f'Test Suit: {FLASH_L3_TEST_SUITE_NAME} Test Case: {self.testName}')
            self.log.step(f'Supported Image type: {imageType}, imageFile: {imageFile}')

            # Copy YAML to DUT
            self.utilities.sftpCopy(
                self.hal_session,
                self.paths["l3TestConfigFile"],
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
            if not result:
                overallResult = False

        return overallResult


if __name__ == '__main__':
    summaryLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summary"
    summaryLog  = logModule(summaryLogName, level=logModule.INFO)
    test = VTS_FLASH_L3_TestAllImages(summaryLog)
    test.run(False)
