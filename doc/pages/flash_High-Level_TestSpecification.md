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

| Test | Description | HAL APIs | Expected FlashImageResult / Report |
|------|-------------|----------|----------------------------------|
| Successful Flash Completion | Trigger flashing with a correct image that meets all specifications. Verify `onProgress()` 0→100% and `onCompleted()` is called once. | `flashImageFromFile()`, `onProgress()`, `onCompleted()` | `SUCCESS = 0` <br>Report: `"Image was flashed successfully"` |
| Flash of Different Image Types | Flash application, bootloader, disaster recovery, splash screen images end-to-end and verify each type is applied successfully. | `IFlashListener` | `SUCCESS = 0` |
| Verify Flash listener | Ensure a valid listener passed to `flashImageFromFile()` receives progress and completion callbacks. | `IFlashListener` | Progress 0→100%, `onCompleted()` called once |
| End-to-End Progress and Completion Verification | Trigger a flash operation and verify `onProgress()` reports 0→100% and `onCompleted()` is called exactly once. | `onProgress()`, `onCompleted()` | Any result code; report as returned |
| Concurrent Flash Requests | Attempt multiple flash operations concurrently. Verify only the first request is accepted; subsequent requests are rejected. | `flashImageFromFile()`, `onCompleted()` | `ERROR_GENERAL = -1` <br>Report: `"Flash operation already in progress"` |
| Invalid Listener | Pass `nullptr` or invalid listener; verify operation rejects and proper error returned. | `flashImageFromFile()`, `onCompleted()` | `ERROR_GENERAL = -1` <br>Report: `"Invalid listener"` |
| Verify Image File path | Flash with an invalid or missing image file. Verify `onCompleted()` returns `ERROR_FILE_OPEN_FAIL = 1`. | `flashImageFromFile()`, `onCompleted()` | `"Failed to open input file"` |
| Verify Flash File Type | Flash with unsupported image type. Verify `onCompleted()` returns `ERROR_IMAGE_INVALID_TYPE = 2`. | `flashImageFromFile()`, `onCompleted()` | `"The image cannot be recognised as a flashable image"` |
| Verify Signature of Flash file | Flash with an image that has an invalid signature. Verify `onCompleted()` returns `ERROR_IMAGE_INVALID_SIGNATURE = 3`. | `flashImageFromFile()`, `onCompleted()` | `"The image signature failed verification"` |
| Verify Flash file size | Flash with an image whose size does not fit the target flash area. Verify `onCompleted()` returns `ERROR_IMAGE_INVALID_SIZE = 4`. | `flashImageFromFile()`, `onCompleted()` | `"Image size does not fit into the target flash area"` |
| Verify Compatible Flash image | Flash with an image incompatible with the target device. Verify `onCompleted()` returns `ERROR_IMAGE_INVALID_PRODUCT = 5`. | `flashImageFromFile()`, `onCompleted()` | `"Image is incompatible with this product"` |
| Simulate Flash Write Failure | Simulate or inject a flash write failure during operation. | `flashImageFromFile()`, `onCompleted()` | `ERROR_FLASH_WRITE_FAILED = 6` <br>Report: `"Flash write operation failed"` |
| Simulate Flash Verify Failure | Simulate failed flash read verification after writing. | `flashImageFromFile()`, `onCompleted()` | `ERROR_FLASH_VERIFY_FAILED = 7` <br>Report: `"Flash read verify operation failed"` |
| Simulate Flash Signature Verify Failure | Simulate post-write signature verification failure. | `flashImageFromFile()`, `onCompleted()` | `ERROR_FLASH_VERIFY_SIGNATURE_FAILED = 8` <br>Report: `"Flash image signature failed verification after writing"` |
| Simulate Unknown / General Error | Inject or trigger unexpected internal failure. | `flashImageFromFile()`, `onCompleted()` | `ERROR_GENERAL = -1` <br>Report: `"Internal failure or unexpected error"` |

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
