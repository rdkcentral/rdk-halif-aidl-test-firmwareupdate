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


## Introduction

This document provides an overview of the testing requirements for the flash module. It outlines the scope of testing, objectives, testing levels and approaches, specific test requirements.

- Flash Interface Definitions - [Flash HAL]()

- Flash Overview - [Flash Overview]()

## Module Description

The Flash `HAL` service provides the interfaces for Flashing a firmware image file onto the device.

The output of an flashing an image onto the device can take two results:

1. **Success** - Correct image has been flashed on to the device.
2. **Failure** - flashing failed due to either incorrect size, signature failure, invalid file, incompatible product,Null listener etc.

## Flashing image Specification

|#|Image file |Specification|
|-|------------------|----------------|
|1|*File existence*|The flashing image file path should be predefined.|
|2|*File Type*|The image file should be a valid flash file type|
|3|*Valid file Signature*|The image file should have valid signature|
|4|*correct size*| The image should have correct size for the target flash area|
|5|*compatible product*|The image file should be compatible with target device.|
-----------

## Prerequisites

Test evaluates flashing an image file with the initially by copying the image in respective device via UART, IP or USB.
Respective Device should be connected to power and communication channel should be established.

## L2 Test Specification

| Test | Description | HAL APIs |
|------|-------------|----------|
| Image flashing | Test will Trigger the flashing with correct image where image file meets all the specification and successful completion of flashing. On completion post `SUCCESS = 0,`  Flash image result | `flashImageFromFile()`, `onProgress()` and `onCompleted()` |
| Progress of flashing | Test will awaits for the callback which gives the progress of flashing the image and percentage of completion.| `onProgress()` |
| Completion of flashing | Test will awaits for the callback  when the  image flashing operation has completed.| `onCompleted()`  |
| Flash listener | Test will check for valid listener passed o the `flashImageFromFile()`to get the progress and completion status|`IFlashListener `|
| Image File path | Test for correct image file path passed to the `flashImageFromFile()` and post  `ERROR_FILE_OPEN_FAIL = 1` by `onCompleted()` if the path is wrong or file does not exist | `flashImageFromFile()`, `onCompleted()`|
| File Type | Test will checks for correct file type to be flashed and post `ERROR_IMAGE_INVALID_TYPE  = 2` by `onCompleted()` if the file type is not correct  |  `flashImageFromFile()`, `onCompleted()` |
| Signature verify | Test will check for Signature of image file and post `ERROR_IMAGE_INVALID_SIGNATURE = 3` by `onCompleted()` if the image signature failed in verification.  |  `flashImageFromFile()`, `onCompleted()` |
| Flash file size | Test will check for correct size of the image file and post `ERROR_IMAGE_INVALID_SIZE = 4` by `onCompleted()` if Image size does not fit into the target flash area.  |  `flashImageFromFile()`, `onCompleted()` |
| Compatible Flash image | Test will check for compatible image file with targeted device and post `ERROR_IMAGE_INVALID_PRODUCT = 5` by `onCompleted()` if Image size is not compatible product.  |  `flashImageFromFile()`, `onCompleted()` |


## L3 Test Specification

The following table outlines the core features validated in the L3 test phase

| Test | Description | HAL APIs |
|------|-------------|----------|
| Flash Verify| Test will verifies for flashing operation then post `ERROR_FLASH_VERIFY_FAILED = 7` when flash read operation is failed | `flashImageFromFile()`, `onCompleted()` |
| Flash write| Test will verifies for file write opertion and post `ERROR_FLASH_WRITE_FAILED = 6` if the write operation is failed | `flashImageFromFile()`, `onCompleted()` |
| Post Siganture verification| Test will check  if Flash image signature failed verification after writing and post `ERROR_FLASH_VERIFY_SIGNATURE_FAILED = 8` | `flashImageFromFile()`, `onCompleted()`|
| Flashing in Background | Test ensures that background flashing operation shall run at a priority which does not impact foreground audio, video or graphics operations  | `flashImageFromFile()`, `onProgress()`,`onCompleted()` |
| Post flashing image verification | Test will verifies boot and basic services initializes correctly without any corruption. | `flashImageFromFile()`, `onCompleted()` |
| Verify persistent data | Test will validate the pre-defined settings/properties remain Persisted post flashing the image | `flashImageFromFile()`, `onCompleted()` |
