# Flash High Level Test Specification Document

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Introduction](#introduction)
- [Module Description](#module-description)
- [Flashing image Specification](#flashing-image-specification)
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

This document describes the *High Level Design (HLD)* for the Flash Hardware Abstraction Layer (HAL).
The Flash HAL provides a standardized interface for flashing firmware image files onto an RDK device in a non-blocking, asynchronous manner

The design defines the functional scope, architecture, operational flow, constraints, and error-handling behavior of the Flash HAL. Detailed test procedures are covered separately in the L2 and L3 test specifications..

## Reference
- Flash Interface Definitions - [Flash HAL](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.15.0/flash/current/com/rdk/hal/flash/IFlash.aidl)


## Module Description

The Flash HAL exposes a single service (IFlash) that allows clients to initiate a background firmware flashing operation using an image file stored on the device.
The HAL shall support flashing the following firmware image types::

- Application images
- Bootloader images
- Disaster recovery images
- Splash screen images

The supported image types and flashing mechanisms are platform-dependent and determined by inspection of the image file

## Flash Operation Outcomes
A flash operation results in one of the following outcomes, reported asynchronously via callbacks:

1. **Success**
	- The image has been successfully validated, written, and verified.
	- The image is marked as the preferred image for the next boot.
2. **Failure**
	- Flashing fails due to validation errors, hardware errors, or internal failures.
	- A standardized error code and optional diagnostic report are returned.

## Interaction Model

 - The client invokes flashImageFromFile() to start a flash operation.
 - The Flash HAL performs validation and flashing in the background.
 - Progress and completion are reported through IFlashListener callbacks.

## Flash Image Validation Specification

Before flashing begins, the HAL validates the image file against the following criteria:

|#|Validation |Description|
|-|------------------|----------------|
|1|*File existence*|The flashing image path should be exist.|
|2|*File Type*|The image file should be a valid flash file type.|
|3|*Valid file Signature*|The image file should have valid signature.|
|4|*correct size*| The image should have correct size for the target flash area.|
|5|*compatible product*|The image file should be compatible with target device.|

If any validation step fails, flashing is aborted and the corresponding error is reported via onCompleted()

-----------


### Prerequisites

- The test environment must be configured with the appropriate flash configuration file and flash map prior to execution.

- The image type under test shall match one of the supported and configured image types defined in the flash map.

- The image verification mechanism (e.g., signature validation, checksum/CRC validation, header validation, version checks, etc.) shall be configurable per image type.

- Each supported image type must have its corresponding validation and verification rules properly defined and enabled in the configuration before test execution.

- The test setup must support device reboot simulation and flash progress monitoring, as the flashing operation is asynchronous and interacts directly with the hardware.

**Note:** 

This feature does not have corresponding L2 system‑level requirements. The flash image handling functionality is implemented as an internal HAL/module‑level API. 
All behaviours are covered under L3 module‑level requirements; hence, L2 testing is not applicable.

L3 testing requires test setup capable of simulating device reboots and monitoring flash progress, as the operation is asynchronous and interacts directly with the hardware.

## L3 Test Specification

| Test | Description | HAL APIs |
|------|------------|----------|
| Verify Successful Flash Operation | 1. Register a valid `IFlashListener` instance.<br>2. Invoke `flashImageFromFile()` with a supported valid image file and verify it returns **true**.<br>3. Monitor `onProgress()` starting at **0%** and verify incremental updates until **100%**.<br>4. Confirm `onCompleted()` is invoked exactly once with *SUCCESS (0)*.<br>5. Repeat for all supported image types. | **IFlash:**<br>&nbsp;&nbsp;flashImageFromFile()<br>**IFlashListener:**<br>&nbsp;&nbsp;onProgress()<br>&nbsp;&nbsp;onCompleted() |
| Verify Concurrent Flash Handling | 1. Start flashing a valid image.<br>2. While flashing is in progress, invoke `flashImageFromFile()` again.<br>3. Verify second request is rejected (returns false).<br>4. Confirm only one `onCompleted()` is triggered for the active operation. | **IFlash:**<br>&nbsp;&nbsp;flashImageFromFile()<br>**IFlashListener:**<br>&nbsp;&nbsp;onCompleted() |
| Verify all Error codes |1. Execute a consolidated negative‑test sequence covering all HAL error conditions.<br>2. For each error scenario, flash the corresponding test image from `negative_images` (e.g., invalid type, invalid size, invalid signature, invalid product, write fail, verify fail, signature verify fail).<br>3. For each image:<br> a. Invoke `flashImageFromFile()`.<br> b. Confirm `onCompleted()` is invoked exactly once.<br> c. Validate the returned `result` matches the error code key associated with the test image.<br>4. Ensure that no progress callbacks are generated for pre‑validation failures (e.g., invalid type/size/signature/product).<br>5. For post‑write failures (verify fail, signature verify fail), confirm flash thread starts and `onProgress(0)` is issued before failure. | **IFlash:** flashImageFromFile()<br>**IFlashListener:** onProgress()*, onCompleted()*  | `**IFlash:**<br>&nbsp;&nbsp;flashImageFromFile()<br>**IFlashListener:**<br>&nbsp;&nbsp;onCompleted()|

### Flash images 

Test images are local data files for the test suite and shall places under assets/flashfiles/input
The following test flash images will be used for flashing process to get the different types of errors.


|#|Flash file name|image type|
|-|----|------|
|1|flash_valid_image1.hex|hex image type|
|2|flash_valid_image2.DRI|DRI images|
|3|flash_valid_image3.pc1|PC1 images|
|4|flash_valid_image4.img|U-Boot wrapped image|
|5|flash_invalid_image1.bin|Raw Binary Image|
|6|flash_unsigned_image1.DRI|Unsigned DRI image|
|7|flash_invalid_pmi1.DRI|invalid PMI image|
