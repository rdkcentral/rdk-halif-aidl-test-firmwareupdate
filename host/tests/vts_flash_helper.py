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
import tarfile
import yaml
import time

dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(dir_path, "../"))

from raft.framework.plugins.ut_raft.configRead import ConfigRead
from raft.framework.plugins.ut_raft import utHelperClass
from raft.framework.core.logModule import logModule
from raft.framework.plugins.ut_raft.utSuiteNavigator import UTSuiteNavigatorClass
from raft.framework.plugins.ut_raft.utBaseUtils import utBaseUtils
from raft.framework.plugins.ut_raft.utUserResponse import utUserResponse

FLASH_TEST_MENU_CONFIGURATION_FILE = os.path.join(dir_path, "vts_flash_testMenuConfiguration.yaml")
FLASH_TESTCASES_CONFIGURATION_FILE = os.path.join(os.path.join(dir_path, "../configs"), "vts_flash_testcase_configuration.yaml")
FLASH_IMAGES_CONFIG_FILE = os.path.join(os.path.join(dir_path, "../../test_configuration"), "vts_flash_image_config.yaml")
FLASH_L3_TEST_CONFIG_FILE = os.path.join(os.path.join(dir_path, "../../test_configuration"), "vts_l3_flash.yaml")

class flashHelperClass(utHelperClass):

    def __init__(self, testName:str, qcId:str, log:logModule=None, moduleName:str=None):
        """
        Initializes the test class
        """
        self.moduleName = "Flash" if moduleName is None else moduleName
        isUnifiedTest = False if moduleName is None else True
        self.profile_moduleName = "flash"
        super().__init__(testName, qcId, log)

        self.log = log
        if log is None:
            self.log = logModule(self.__class__.__name__)
            self.log.setLevel(self.log.INFO)

        # Parse device configuration first (used by path selection below)
        self.deviceTestSetup = self.cpe.get("test")
        self.waitTime = self.cpe.get("wait_time")
        self.device_type = self.cpe.get("device_type")
        self.platform = self.cpe.get("platform")
        self.targetWorkspace = os.path.join(self.cpe.get("target_directory"), self.moduleName)

        # Resolve local component and unified repo roots
        component_root = os.path.normpath(os.path.join(dir_path, "../.."))
        unified_root = os.path.normpath(os.path.join(dir_path, "../../../.."))

        # Resolve all config paths based on run mode
        if not isUnifiedTest:
            self.log.info(f"Initialized flashHelperClass for module: {self.moduleName}")
            self.testMenuConfigFile = FLASH_TEST_MENU_CONFIGURATION_FILE
            self.testCasesConfigFile = FLASH_TESTCASES_CONFIGURATION_FILE
            self.imagesConfigFile = FLASH_IMAGES_CONFIG_FILE
            self.l3TestConfigFile = FLASH_L3_TEST_CONFIG_FILE
            self.moduleHFPFile = os.path.join(dir_path, self.deviceTestSetup.get("profile"))
            artifactBaseDir = dir_path
            targetCfgBase = self.targetWorkspace
            self.testCaseConfiguration = ConfigRead(self.testCasesConfigFile, self.moduleName)

        else:
            self.testMenuConfigFile = os.path.join(unified_root, "host/tests/TestCases/vts_l3_test_setup.yaml")
            self.testCasesConfigFile = os.path.join(component_root, "configs/vts_flash_testcase_configuration.yaml")
            self.imagesConfigFile = os.path.join(component_root, "test_configuration/vts_flash_image_config.yaml")
            self.l3TestConfigFile = os.path.join(component_root, "test_configuration/vts_l3_flash.yaml")
            self.moduleHFPFile = os.path.join(component_root, "profiles/hfp-flash-vcomponent.yaml")
            artifactBaseDir = os.path.join(component_root, "host")
            targetCfgBase = os.path.join(self.targetWorkspace, "bin").replace("\\", "/")

        self.moduleHFPFile = os.path.normpath(self.moduleHFPFile).replace("\\", "/")
        self.testMenuConfigFile = os.path.normpath(self.testMenuConfigFile)
        self.testCasesConfigFile = os.path.normpath(self.testCasesConfigFile)
        self.imagesConfigFile = os.path.normpath(self.imagesConfigFile)
        self.l3TestConfigFile = os.path.normpath(self.l3TestConfigFile)

        profileOnTarget = os.path.join(
            targetCfgBase,
            "profiles",
            os.path.basename(self.moduleHFPFile)
        ).replace("\\", "/")

        self.l3TestConfigFolder = os.path.join(targetCfgBase, "test_configuration").replace("\\", "/")

        # Load test menu and test case configuration files
        self.testMenuConfiguration = ConfigRead(self.testMenuConfigFile, self.moduleName)
        self.testImageList = ConfigRead(self.imagesConfigFile, "Flash")
        self.testMenuConfiguration.test.execute = self.testMenuConfiguration.test.execute + f" -p {profileOnTarget}"

        # Establish HAL session
        self.hal_session = self.dut.getConsoleSession("ssh_hal_test_0")
        self.hal_session_1 = self.dut.getConsoleSession("ssh_hal_test_1")

        # Initialize UT menu navigator and utilities
        self.utMenu = UTSuiteNavigatorClass(self.testMenuConfiguration, None, self.hal_session)
        self.utilities = utBaseUtils()

        # Create user response Class
        self.testUserResponse = utUserResponse()

        # Copy required artifacts to target device
        self.utilities.change_directory(self.hal_session, self.targetWorkspace)
        for artifact in self.testMenuConfiguration.test.artifacts:
            filesPath = os.path.normpath(os.path.join(artifactBaseDir, artifact))
            self.utilities.sftpCopy(self.hal_session, filesPath, self.targetWorkspace)

            if tarfile.is_tarfile(filesPath):
                self.utilities.change_directory(self.hal_session, self.targetWorkspace)
                target_tarfile = os.path.join(self.targetWorkspace, os.path.basename(artifact))
                target_tarfile = target_tarfile.replace("\\", "/")
                self.utilities.untar(self.hal_session, target_tarfile, self.targetWorkspace)

        self.targetWorkspace = targetCfgBase
        self.utilities.change_directory(self.hal_session, self.targetWorkspace)
        self.utilities.change_directory(self.hal_session_1, self.targetWorkspace)
        time.sleep(2)

    def runTest(self, testSuite, test_case:str=None):
        """
        Runs the test case passed to this funtion
        Args:
            test_case (str, optional): test case name to run, default runs all test
        Returns:
            bool: True - test pass, False - test fails
        """
        testsuite_name = testSuite.get("name")

        output = self.utMenu.select(testsuite_name, test_case, None, self.waitTime, False)
        results = self.utMenu.collect_results(output, True)
        if results is None:
            results = False
        return results

    def flashReadYaml(self, yaml_path: str) -> dict:
        """
        Reads a YAML file and returns its content as a dictionary.
        Args:
            yaml_path (str): Path to the YAML file
        Returns:
            dict: Content of the YAML file as a dictionary
        """
        with open(yaml_path, "r") as f:
            return yaml.safe_load(f)


    def flashWriteYaml(self, yaml_path: str, data: dict):
        """
        Writes a dictionary to a YAML file.
        Args:
            yaml_path (str): Path to the YAML file
            data (dict): Content to write to the YAML file
        """
        with open(yaml_path, "w") as f:
            yaml.dump(data, f, sort_keys=False, default_flow_style=False)

    def getValidImageList(self):
        """
        Convert ConfigRead.image_types to a real Python dictionary.
        """
        image_obj = self.testImageList.image_types
        image_dict = {}
        for key in dir(image_obj):
            if key.startswith("_"):
                continue
            value = getattr(image_obj, key)
            if isinstance(value, str):
                image_dict[key] = value
        return image_dict

    def getNegativeImageList(self) -> dict:
        """Return negative_images as a clean dictionary: {errorCode: filename}."""
        try:
            # ConfigRead maps YAML to an object; convert to dict
            return dict(self.testImageList.negative_images.__dict__)
        except Exception:
            # fallback for safety
            obj = self.testImageList.negative_images
            out = {}
            for key in dir(obj):
                if not key.startswith("_"):
                    val = getattr(obj, key)
                    if isinstance(val, str):
                        out[key] = val
            return out

    def updateFlashTestConfig(self, filename: str, expectedResult: str, stepPercent: int = 0):
        """
        Updates the Flash L3 YAML for single-image mode.
        """
        data = self.flashReadYaml(self.l3TestConfigFile)

        params = data["Flash"]["L3"]["Test"]["Parameters"]
        expected = data["Flash"]["L3"]["Test"]["Expected_Results"]

        params["filename"] = filename
        params["listener"] = True
        params["step_percent"] = stepPercent

        expected["exceptionCode"] = "EX_NONE"
        expected["result"] = True
        expected["errorCode"] = expectedResult

        self.flashWriteYaml(self.l3TestConfigFile, data)

    def updateTestconfigurationFile(self, set_state:str, states_list:list, stress:bool):
        """
         Updates the Flash L3 test YAML configuration file.
        """
        
        data = self.flashReadYaml(self.l3TestConfigFile)
        config_statelist = data["Flash"]["L3"]["SetStates"]["Parameters"]["state"]
        repeat_count = 1

        if stress:
            config_statelist.clear()
            for states in states_list:
                config_statelist.append(states)
        else:
            config_statelist.clear()
            config_statelist.append(str(set_state))
            data["Flash"]["L3"]["SetStates"]["Parameters"]["repeat"] = repeat_count

        # write to YAML
        self.flashWriteYaml(self.l3TestConfigFile, data)

    def updateNestedDictionaries(self, original: dict, updates: dict):
        """
        Recursively updates nested dictionaries.
        Args:
            original (dict): Original dictionary to be updated
            updates (dict): Dictionary with updates
        """
        for key, value in updates.items():
            if isinstance(value, dict) and key in original:
                self.updateNestedDictionaries(original[key], value)
            else:
                original[key] = value

    def testEndFunction(self, powerOff=True):
        """
        Ends the test case.
        Args:
            powerOff (bool): Whether to power off the device
        """
        super().testEndFunction(powerOff)