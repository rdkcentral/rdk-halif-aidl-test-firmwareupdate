# FirmwareUpdate High Level Test Specification Document

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Introduction](#introduction)
- [Module Description](#module-description)
- [Firmware Update Image Specification](#firmware-update-image-specification)
- [Prerequisites](#prerequisites)
- [L3 Test Specification](#l3-test-specification)

## Acronyms, Terms and Abbreviations

- `HAL`    - Hardware Abstraction Layer
- `API`    - Application Programming Interface
- `AIDL`   - Android Interface Definition Language
- `L2`     - Level 2 Testing
- `L3`     - Level 3 Testing
- `NA`     - Not Applicable
- `Caller` - Any user of the interface via the `APIs`
- `CB`     - Call-back function (suffix)
- `RDK`    - Reference Design Kit
- `JTAG`   - Joint Test Action Group
- `UART`   - Universal Asynchronous Receiver-Transmitter
- `SDK`    - Software Development Kit
- `CLI`    - Command Line Interface
- `ARM`    - Advanced RISC Machine
- `USB`    - Universal Serial Bus
- `IP`     - Internet Protocol
- `SSH`    - Secure Shell
- `CRC`    - Cyclic Redundancy Check


## Introduction

This document describes the *High Level Design (HLD)* for the FirmwareUpdate Hardware Abstraction Layer (HAL).
The FirmwareUpdate HAL provides a standardized interface for updating firmware image files on an RDK device in a non-blocking, asynchronous manner.

The design defines the functional scope, architecture, operational flow, constraints, and error-handling behavior of the FirmwareUpdate HAL. Detailed test procedures are covered separately in the L2 and L3 test specifications.

## Reference
- FirmwareUpdate Interface Definitions - [FirmwareUpdate HAL](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.15.0/firmwareupdate/current/com/rdk/hal/firmwareupdate/IFirmwareUpdate.aidl)


## Module Description

The FirmwareUpdate HAL exposes a single service (`IFirmwareUpdate`) that allows clients to initiate a background firmware update operation using an image file stored on the device.
The HAL shall support updating the following firmware image types:

- Application images
- Bootloader images
- Disaster recovery images
- Splash screen images

The supported image types and update mechanisms are platform-dependent and determined by inspection of the image file.

## Firmware Update Operation Outcomes
A firmware update operation results in one of the following outcomes, reported asynchronously via callbacks:

1. **Success**
	- The image has been successfully validated, written, and verified.
	- The image is marked as the preferred image for the next boot.
2. **Failure**
	- Firmware updating fails due to validation errors, hardware errors, or internal failures.
	- A standardized error code and optional diagnostic report are returned.

## Interaction Model

 - The client invokes updateFirmwareFromFile() to start a firmware update operation.
 - The FirmwareUpdate HAL performs validation and the update in the background.
 - Progress and completion are reported through IFirmwareUpdateListener callbacks.

## Firmware Update Image Validation Specification

Before updating begins, the HAL validates the image file against the following criteria:

|#|Validation |Description|
|-|------------------|----------------|
|1|*File existence*|The firmware image path must exist.|
|2|*File Type*|The image file must be a valid firmware update file type.|
|3|*Valid file Signature*|The image file should have valid signature.|
|4|*correct size*| The image should have correct size for the target update area.|
|5|*compatible product*|The image file should be compatible with target device.|

If any validation step fails, the firmware update is aborted and the corresponding error is reported via onCompleted().

-----------


### Prerequisites

- The test environment must be configured with the appropriate firmware update configuration file and image map prior to execution.

- The image type under test shall match one of the supported and configured image types defined in the image map.

- The image verification mechanism (e.g., signature validation, checksum/CRC validation, header validation, version checks, etc.) shall be configurable per image type.

- Each supported image type must have its corresponding validation and verification rules properly defined and enabled in the configuration before test execution.

- The test setup must support device reboot simulation and firmware update progress monitoring, as the update operation is asynchronous and interacts directly with the hardware.

**Note:** 

This feature does not have corresponding L2 system-level requirements. The firmware image handling functionality is implemented as an internal HAL/module-level API.
All behaviours are covered under L3 module‑level requirements; hence, L2 testing is not applicable.

L3 testing requires a test setup capable of simulating device reboots and monitoring firmware update progress, as the operation is asynchronous and interacts directly with the hardware.

## L3 Test Specification

| Test | Description | HAL APIs |
|------|------------|----------|
| Verify Successful Firmware Update | 1. Register a valid `IFirmwareUpdateListener` instance.<br>2. Invoke `updateFirmwareFromFile()` with a supported valid image file and verify it returns **true**.<br>3. Monitor `onProgress()` starting at **0%** and verify incremental updates until **100%**.<br>4. Confirm `onCompleted()` is invoked exactly once with *SUCCESS (0)*.<br>5. Repeat for all supported image types. | **IFirmwareUpdate:**<br>&nbsp;&nbsp;updateFirmwareFromFile()<br>**IFirmwareUpdateListener:**<br>&nbsp;&nbsp;onProgress()<br>&nbsp;&nbsp;onCompleted() |
| Verify Concurrent Firmware Update Handling | 1. Start a firmware update with a valid image.<br>2. While the update is in progress, invoke `updateFirmwareFromFile()` again.<br>3. Verify the second request is rejected (returns false).<br>4. Confirm only one `onCompleted()` is triggered for the active operation. | **IFirmwareUpdate:**<br>&nbsp;&nbsp;updateFirmwareFromFile()<br>**IFirmwareUpdateListener:**<br>&nbsp;&nbsp;onCompleted() |
| Verify all Error codes | 1. Execute a consolidated negative-test sequence covering all HAL error conditions.<br>2. For each error scenario, use the corresponding test image from `negative_images` (for example invalid type, invalid size, invalid signature, invalid product, write fail, verify fail, signature verify fail).<br>3. For each image:<br>&nbsp;&nbsp;a. Invoke `updateFirmwareFromFile()`.<br>&nbsp;&nbsp;b. Confirm `onCompleted()` is invoked exactly once.<br>&nbsp;&nbsp;c. Validate that the returned `result` matches the error code key associated with the test image.<br>4. Ensure that no progress callbacks are generated for pre-validation failures (for example invalid type, size, signature, or product).<br>5. For post-write failures such as verify fail or signature verify fail, confirm the update thread starts and `onProgress(0)` is issued before failure. | **IFirmwareUpdate:**<br>&nbsp;&nbsp;updateFirmwareFromFile()<br>**IFirmwareUpdateListener:**<br>&nbsp;&nbsp;onProgress()*<br>&nbsp;&nbsp;onCompleted()* |

### Firmware Update Images

Test images are local data files for the test suite and shall places under assets/firmwareupdate/input
The following test firmware images are used to exercise different update result and error conditions.


|#|Firmware image file name|image type|
|-|----|------|
|1|firmwareupdate_valid_image1.hex|hex image type|
|2|firmwareupdate_valid_image2.DRI|DRI images|
|3|firmwareupdate_valid_image3.pc1|PC1 images|
|4|firmwareupdate_valid_image4.img|U-Boot wrapped image|
|5|firmwareupdate_invalid_image1.bin|Raw Binary Image|
|6|firmwareupdate_unsigned_image1.DRI|Unsigned DRI image|
|7|firmwareupdate_invalid_pmi1.DRI|invalid PMI image|
