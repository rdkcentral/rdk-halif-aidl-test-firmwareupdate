# Flash High Level Test Specification Document

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Introduction](#introduction)
- [Module Description](#module-description)
- [Flashing image Specification](#flashing-image-specification)
- [Prerequisites](#prerequisites)
- [L2 Test Specification](#l2-test-specification)
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
- Flash Interface Definitions - [Flash HAL]()

- Flash Overview - [Flash Overview]()

## Module Description

The Flash HAL exposes a single service (IFlash) that allows clients to initiate a background firmware flashing operation using an image file stored on the device.
The HAL supports flashing of different image types, including but not limited to:

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

## Prerequisites

Test evaluates flashing an image file with the initially by copying the image in respective device via UART, IP or USB.
Respective Device should be connected to power and communication channel should be established.

## L3 Test Specification

| Test | Description | HAL APIs |
|------|-------------|----------|
| Verify a flashing of valid Image | Test will Trigger the flashing with correct image where image file meets all the specification and successful completion of flashing. On completion post `SUCCESS = 0,`  Flash image result. | `flashImageFromFile()`, `onProgress()` and `onCompleted()` |
| Verify a flash progress | Test will awaits for the callback which gives the progress of flashing the image and percentage of completion.| `onProgress()` |
| Verify a flash completion | Test will awaits for the callback  when the  image flashing operation has completed.| `onCompleted()`  |
| Verify Flash listener | Test will check for valid listener passed o the `flashImageFromFile()`to get the progress and completion status.|`IFlashListener `|
| Verify Image File path | Test for correct image file path passed to the `flashImageFromFile()` and post  `ERROR_FILE_OPEN_FAIL = 1` by `onCompleted()` if the path is wrong or file does not exist. | `flashImageFromFile()`, `onCompleted()`|
| Verify Flash File Type | Test will checks for correct file type to be flashed and post `ERROR_IMAGE_INVALID_TYPE  = 2` by `onCompleted()` if the file type is not correct. |  `flashImageFromFile()`, `onCompleted()` |
| Verify Signature of Flash file | Test will check for Signature of image file and post `ERROR_IMAGE_INVALID_SIGNATURE = 3` by `onCompleted()` if the image signature failed in verification. |  `flashImageFromFile()`, `onCompleted()` |
| Verify Flash file size | Test will check for correct size of the image file and post `ERROR_IMAGE_INVALID_SIZE = 4` by `onCompleted()` if Image size does not fit into the target flash area.  |  `flashImageFromFile()`, `onCompleted()` |
| Verify Compatible Flash image | Test will check for compatible image file with targeted device and post `ERROR_IMAGE_INVALID_PRODUCT = 5` by `onCompleted()` if Image size is not compatible product.  |  `flashImageFromFile()`, `onCompleted()` |

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
