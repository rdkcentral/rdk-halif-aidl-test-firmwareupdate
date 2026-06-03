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
FLASH_PROFILE_FILE_NAME = "profiles/hfp-flash-vcomponent.yaml"
FLASH_IMAGES_CONFIG_FILE = "flash_test_configuration/vts_flash_image_config.yaml"
FLASH_L3_TEST_CONFIG_FILE = "flash_test_configuration/vts_l3_flash.yaml"

class flashHelperClass(utHelperClass):
    # Set to an absolute path by the non_av_l3 runner to use the common
    # vts_l3_test_setup.yaml menu.  None (default) means use the file bundled
    # with this component, so standalone flash runs are unaffected.
    _menuConfigFile = None

    def __init__(self, testName: str, qcId: str, log: logModule = None):
        """
        Initializes the Flash HAL test helper class.
        Args:
            testName (str): Name of the test case.
            qcId (str): QC identifier for the test.
            log (logModule, optional): Logger instance; created if not provided.
        """
        self.profile_moduleName = "flash"
        super().__init__(testName, qcId, log)

        if log is None:
            log = logModule(self.__class__.__name__)
            log.setLevel(log.INFO)
        self.log = log

        # Device configuration
        self.deviceTestSetup = self.cpe.get("test")
        self.waitTime = self.cpe.get("wait_time")
        self.device_type = self.cpe.get("device_type")
        self.platform = self.cpe.get("platform")

        # Determine context: unified non-AV runner vs standalone component run
        _run_from_unified = (self.__class__._menuConfigFile is not None)

        if _run_from_unified:
            self.moduleName     = self.__class__._moduleName
            profilePath         = self.__class__._profilePath
            artifactpath        = self.__class__._dirPath
            _menu_cfg           = self.__class__._menuConfigFile
            self.moduleHFPFile  = os.path.join(str(profilePath), FLASH_PROFILE_FILE_NAME).replace("\\", "/")
            self.imageConfigFile = os.path.join(str(profilePath), FLASH_IMAGES_CONFIG_FILE).replace("\\", "/")
        else:
            self.moduleName     = "Flash"
            artifactpath        = dir_path
            _menu_cfg           = FLASH_TEST_MENU_CONFIGURATION_FILE
            self.moduleHFPFile  = os.path.join(dir_path, self.deviceTestSetup.get("profile")).replace("\\", "/")
            self.imageConfigFile = os.path.join(dir_path, "../../", FLASH_IMAGES_CONFIG_FILE)

        self.targetWorkspace  = os.path.join(self.cpe.get("target_directory"), self.moduleName)
        self.l3TestConfigFile = os.path.join(dir_path, "../../", FLASH_L3_TEST_CONFIG_FILE)

        # Load configurations
        self.testMenuConfiguration = ConfigRead(_menu_cfg, self.moduleName)
        self.testCaseConfiguration = None if _run_from_unified else ConfigRead(FLASH_TESTCASES_CONFIGURATION_FILE, self.moduleName)
        self.testImageList         = ConfigRead(self.imageConfigFile, "Flash")

        # Target config folder path
        _cfg_base = os.path.join(self.targetWorkspace, "bin") if _run_from_unified else self.targetWorkspace
        self.l3TestConfigFolder = os.path.join(_cfg_base, "flash_test_configuration").replace("\\", "/")

        # Establish HAL sessions
        self.hal_session   = self.dut.getConsoleSession("ssh_hal_test_0")
        self.hal_session_1 = self.dut.getConsoleSession("ssh_hal_test_1")

        # Initialize UT menu navigator, utilities and user response
        self.utMenu          = UTSuiteNavigatorClass(self.testMenuConfiguration, None, self.hal_session)
        self.utilities       = utBaseUtils()
        self.testUserResponse = utUserResponse()

        # Copy required artifacts to target device
        self.utilities.change_directory(self.hal_session, self.targetWorkspace)
        for artifact in self.testMenuConfiguration.test.artifacts:
            filesPath = os.path.join(artifactpath, artifact)
            self.utilities.sftpCopy(self.hal_session, filesPath, self.targetWorkspace)
            if tarfile.is_tarfile(filesPath):
                target_tarfile = os.path.join(self.targetWorkspace, os.path.basename(artifact)).replace("\\", "/")
                self.utilities.untar(self.hal_session, target_tarfile, self.targetWorkspace)

        self.utilities.change_directory(self.hal_session_1, self.targetWorkspace)
        if _run_from_unified:
            self.targetWorkspace = os.path.join(self.targetWorkspace, "bin").replace("\\", "/")
            self.utilities.change_directory(self.hal_session, self.targetWorkspace)
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
