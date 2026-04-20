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
from raft.framework.plugins.ut_raft import utHelperClass
from raft.framework.plugins.ut_raft.utSuiteNavigator import UTSuiteNavigatorClass
from raft.framework.plugins.ut_raft.utBaseUtils import utBaseUtils
from host.tests.vts_flash_helper import flashHelperClass

FLASH_L1_TEST_SUITE_NAME = "VTS_L1_FLASH"

class  VTS_FLASH_L1_Tests(flashHelperClass):
    """
    Runs automated L1 FLASH tests on the target device.
    Handles setup, service restarts, asset management.
    Collects and aggregates test results for pass/fail reporting.
    """
    def __init__(self, log:logModule=None):
        """
        Initializes the test class
        """
        qcId = "1"
        self.testName = "Flash_L1"
        #configureNetwork = False
        super().__init__(self.testName, qcId, log)


    def testFunction(self):
        """
        The main test function to run the L1 and L2 tests.

        Returns:
            bool: Final result of the test.
        """
        self.utilities.change_directory(self.hal_session, self.targetWorkspace)

        self.utMenu.start()

        testsuites = self.testCaseConfiguration.fields.get('test_suites')
        finalresult = True

        for testsuite in testsuites:
            testsuite_name = testsuite.get("name")
            if testsuite_name != FLASH_L1_TEST_SUITE_NAME:
                continue

            test_cases = testsuite.get("test_cases")

            if len(test_cases) == 1 and test_cases[0] == "all":
                self.log.stepStart(f'Test Suit: {testsuite_name} Run all Tests cases')
                # If 'all' test case mentioned in list, run all tests with 'r' option
                result = self.runTest(testsuite)
                self.log.stepResult(result, f'Test Suit: {testsuite_name} Run all Tests cases')
            else:
                for test_case in testsuite.get("test_cases"):
                    self.log.stepStart(f'Test Suit: {testsuite_name} Test Case: {test_case}')
                    result = self.runTest(testsuite, test_case)
                    if not result:
                        finalresult = False
                    self.log.stepResult(result, f'Test Suit: {testsuite_name} Test Case: {test_case}')
        return finalresult

if __name__ == '__main__':
    summaryLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summery"
    summaryLog = logModule(summaryLogName, level=logModule.INFO)
    test = VTS_FLASH_L1_Tests(summaryLog)
    test.run(False)