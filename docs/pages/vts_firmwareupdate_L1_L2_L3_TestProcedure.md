# FirmwareUpdate HAL L1, L3 Test Procedure

## Table of Contents

* [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
* [Build the VTS](#build-the-vts)
* [Run L1 Tests Without Python](#run-l1-tests-without-python)
* [Run Tests in Python Environment](#run-tests-in-python-environment)

  * [Update Configuration Files](#update-configuration-files)
  * [Install Python Packages](#install-python-packages)
  * [Run Test Cases](#run-test-cases)
* [L3 Test Cases](#l3-test-cases)
* [Notes](#notes)

---

## Acronyms, Terms and Abbreviations

* `HAL`  - Hardware Abstraction Layer
* `VTS`  - Vendor Test Suite
* `L1`   - Level 1 Testing
* `L2`   - Level 2 Testing
* `L3`   - Level 3 Testing
* `DUT`  - Device Under Test
* `RAFT` - Rapid Automation Framework for Testing
* `YAML` - YAML Ain't Markup Language
* `AIDL` - Android Interface Definition Language

---

## Build the VTS

### Build FirmwareUpdate VTS

```bash
git clone git@github.com:rdkcentral/rdk-halif-aidl-test-firmwareupdate.git
cd rdk-halif-aidl-test-firmwareupdate
```

### Environment Variables

```bash
export UT_LOG_LEVEL=4
export UT_CORE_VERSION=5.1.0
export RDK_HALIF_AIDL_VERSION=0.21.0
```

Enable logs (optional):

```bash
export ENABLE_UT_INFO_LOGGING=1
```
#### Note
This build has been compiled and reviewed against AIDL version 0.21.0.

### Clean

```bash
./build.sh dist_clean
```
### Build

```bash
./build.sh Target=linux
```

### Clean

```bash
./build.sh dist_clean
```

Output:

* Binary: `build/bin/`
* Tar: `vts-firmwareupdate.tar.gz`

---

## Run L1 Tests Without Python

### Copy to DUT

```bash
scp vts-firmwareupdate.tar.gz root@<DUT>:/tmp/
```

### Extract

```bash
ssh root@<DUT>
mkdir /tmp/FirmwareUpdate
tar -zxf vts-firmwareupdate.tar.gz -C /tmp/FirmwareUpdate
cd /tmp/FirmwareUpdate
```

### Run Tests

```bash
./run.sh
```

---

## Run Tests in Python Environment

### Update Configuration Files

#### Rack Configuration

* Configure:

  * `ssh_hal_test`
  * `ssh_hal_test_1`
* Ensure DUT connectivity

---

#### Device Configuration

```yaml
deviceConfig:
  cpe1:
    platform: "your_platform"
    target_directory: "/tmp/"
    wait_time: 40
    test:
      profile: "../../profiles/hfp-firmwareupdate-vcomponent.yaml"
```

---

## Install Python Packages

```bash
./install.sh
source activate_venv.sh
./install.sh
```

---

## Run Test Cases

### L1

```bash
python vts_firmwareupdate_L1_tests.py \
--config <rack_config> \
--deviceConfig <device_config>
```

---

### L3

```bash
python <vts_firmwareupdate_L3_xxx>.py \
--config <rack_config> \
--deviceConfig <device_config>
```

---

### FirmwareUpdate Image Configuration File
[test_configuration/vts_firmwareupdate_image_config.yaml](../../test_configuration/vts_firmwareupdate_image_config.yaml)

```yaml
FirmwareUpdate:
  image_types:
    PCI: "<path_to_actual_PCI_firmware_image>"
    DRI: "<path_to_actual_DRI_firmware_image>"
    SPLASH: "<path_to_actual_SPLASH_firmware_image>"
    BOOTLOADER: "<path_to_actual_BOOTLOADER_firmware_image>"

  negative_images:
    ERROR_IMAGE_INVALID_TYPE: "<path_to_invalid_type_image>"
    ERROR_IMAGE_INVALID_SIZE: "<path_to_invalid_size_image>"
    ERROR_IMAGE_INVALID_SIGNATURE: "<path_to_invalid_signature_image>"
    ERROR_IMAGE_INVALID_PRODUCT: "<path_to_invalid_product_image>"
    ERROR_FW_UPDATE_VERIFY_FAILED: "<path_to_verify_fail_image>"
    ERROR_FW_UPDATE_VERIFY_SIGNATURE_FAILED: "<path_to_verify_signature_fail_image>"
```

---

#### Notes

- Replace `<path_to_…>` in the YAML with **real** firmwareupdate image paths on the DUT.
- Each negative test image must represent the **actual expected failure case**:
  - invalid type
  - invalid size
  - invalid signature
  - invalid product mismatch
  - verify fail
  - signature verify fail

---

##### Example Mapping Table

| Test Case | VComponent Path | Actual Component Path |
|----------|------------------|------------------------|
| **PCI** | `/tmp/FirmwareUpdate/images/firmwareupdate_valid_PCI.bin` | `<path_to_actual_PCI_firmware_image>` |
| **DRI** | `/tmp/FirmwareUpdate/images/firmwareupdate_valid_DRI.bin` | `<path_to_actual_DRI_firmware_image>` |
| **SPLASH** | `/tmp/FirmwareUpdate/images/firmwareupdate_valid_SPLASH.bin` | `<path_to_actual_SPLASH_firmware_image>` |
| **BOOTLOADER** | `/tmp/FirmwareUpdate/images/firmwareupdate_valid_BOOTLOADER.bin` | `<path_to_actual_BOOTLOADER_firmware_image>` |
| **ERROR_IMAGE_INVALID_TYPE** | `/tmp/FirmwareUpdate/images/firmwareupdate_valid_PCCI.bin` | `<path_to_invalid_type_image>` |
| **ERROR_IMAGE_INVALID_SIZE** | `/tmp/FirmwareUpdate/images/PCI_invalid_size.bin` | `<path_to_invalid_size_image>` |
| **ERROR_IMAGE_INVALID_SIGNATURE** | `/tmp/FirmwareUpdate/images/PCI_invalid_signature.bin` | `<path_to_invalid_signature_image>` |
| **ERROR_IMAGE_INVALID_PRODUCT** | `/tmp/FirmwareUpdate/images/PCI_invalid_product.bin` | `<path_to_invalid_product_image>` |
| **ERROR_FW_UPDATE_VERIFY_FAILED** | `/tmp/FirmwareUpdate/images/PCI_verify_fail.bin` | `<path_to_verify_fail_image>` |
| **ERROR_FW_UPDATE_VERIFY_SIGNATURE_FAILED** | `/tmp/FirmwareUpdate/images/PCI_verify_signature_fail.bin` | `<path_to_verify_signature_fail_image>` |


### FirmwareUpdate Test Configuration File

[test_configuration/vts_l3_firmwareupdate.yaml](../../test_configuration/vts_l3_firmwareupdate.yaml)

Note:
- This config File will be updated by running L3 python test cases

---

## 1. Validate All Error Codes — `vts_firmwareupdate_L3_validateAllErrorcodes.py`

### Purpose

Validates FirmwareUpdate HAL behavior for **all negative image scenarios**.

---

### Test Flow

```mermaid
graph TD
    A[Load Negative Image List] --> B[Update YAML]
    B --> C[Copy YAML to DUT]
    C --> D[Run firmwareupdate_validateAllErrorcodes]
    D --> E[Collect Results]
    E --> F{Match Expected Error?}
    F -->|No| G[FAIL]
    F -->|Yes| H[Next Image]
```

---

### Test Steps

* Read negative images from config
* Update YAML dynamically
* Execute test on DUT
* Validate returned error codes

---

### Expected Results

| Input Image       | Expected Result               |
| ----------------- | ----------------------------- |
| Invalid type      | ERROR_IMAGE_INVALID_TYPE      |
| Invalid size      | ERROR_IMAGE_INVALID_SIZE      |
| Invalid signature | ERROR_IMAGE_INVALID_SIGNATURE |
| Verify fail       | ERROR_FW_UPDATE_VERIFY_FAILED |

---

---

## 2. Validate Firmware Update Success — `firmwareupdate_imageTypes`

### Purpose

Validates successful firmware updating of **valid images**.

---

### Test Flow

```mermaid
graph TD
    A[Parse YAML] --> B[Call firmware update API]
    B --> C[Wait Completion]
    C --> D{SUCCESS?}
    D -->|No| E[FAIL]
    D -->|Yes| F[Check Progress]
    F --> G[PASS]
```

---

### Test Steps

* Load valid image
* Call `updateFirmwareFromFile`
* Monitor progress
* Validate:

  * Progress: 0 → 100
  * Final result: SUCCESS

---

---

## 3. Validate Concurrent Firmware Update Rejection — `firmwareupdate_concurrentReject`

### Purpose

Ensures FirmwareUpdate HAL rejects parallel firmware update requests.

---

### Test Flow

```mermaid
graph TD
    A[Start Update 1] --> B[Start Update 2]
    B --> C{Rejected?}
    C -->|No| D[FAIL]
    C -->|Yes| E[Wait Completion]
    E --> F[PASS]
```

---

### Test Steps

* Start first firmware update
* Immediately trigger second
* Validate:

  * Second request rejected
  * First completes successfully

---

---

## Host–DUT Interaction Model

### Key Characteristics

* Fully **host-driven**
* YAML updated dynamically
* No reboot required
* No UT-controller used

---


## Notes

### Image Location

Ensure all images exist at:

```
/assets/firmwareupdatefiles/input/
```

---

### Timeout Configuration

Defined in YAML:

```yaml
FirmwareUpdate:
  overall:
    maxFirmwareUpdateWaitDurationMs: 900000
```

---

### Common Failures

| Issue            | Cause                    |
| ---------------- | ------------------------ |
| Timeout          | Large image / slow firmware update |
| Wrong error code | Incorrect test image     |
| No progress      | Listener not registered  |

---

## Note

### Test Setup on vDevice

To validate automation on vDevice, create a password for the root user using the following command:

```bash
passwd root
```

Then, update the same password in the `vts_rack_configuration.yaml` file.

Currently, the Python `paramiko` library does not support empty passwords on vDevice, so setting a password is necessary.

### Run on Windows Environment

To run the Python environment tests in a Windows environment, follow these additional steps after checking out the repository and compiling the code:

1. Install the python environment and update the rack configuration files as in the Linux environment.

2. Move the entire project folder (including `host`) to the Windows machine.

3. Open the file located at `framework/plugins/ut_raft/interactive_shell.py`.

   - Comment out or remove the following import lines to avoid errors on Windows:
     ```python
     import pty
     import fcntl
     ```

4. After these changes, run the test cases using Python on the Windows system.


---
