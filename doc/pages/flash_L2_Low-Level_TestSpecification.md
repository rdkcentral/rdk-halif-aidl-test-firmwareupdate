# Flash L2 Low Level Test Specification and Procedure Documentation

## Table of Contents

- [Overview](#overview)
- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Definitions](#definitions)
- [References](#references)
- [Level 2 Test Procedure](#level-2-test-procedure)

## Overview

This document describes the level 2 testing suite for the Flash module.

### Acronyms, Terms and Abbreviations

- `HAL` \- Hardware Abstraction Layer, may include some common components
- `UT`  \- Unit Test(s)
- `OEM`  \- Original Equipment Manufacture
- `SoC`  \- System on a Chip

### Definitions

- `ut-core` \- Common Testing Framework <https://github.com/rdkcentral/ut-core>, which wraps a open-source framework that can be expanded to the requirements for future framework.

### References

- `High Level Test Specification` - [flash_High-Level_TestSpecification.md](flash_High-Level_TestSpecification.md)

## Level 2 Test Procedure

The following functions are planned as part of L2 testing.

### Test 1

| Title | Details |
| -- | -- |
| Function Name | `test_flashImageFromFile` |
| Description |  Flashes an image file onto the device.|
| Test Group | Module : 02 |
| Test Case ID | 001 |
| Priority | High |

**Pre-Conditions :**
Device should be connected to power and and communication channel should be established.

**Dependencies :**
|#|Image file |Specification|
|-|------------------|----------------|
|1|*File existence*|The flashing image file path should be predefined.|
|2|*File Type*|The image file should be a valid flash file type|
|3|*Valid file Signature*|The image file should have valid signature|
|4|*correct size*| The image should have correct size for the target flash area|
|5|*compatible product*|The image file should be compatible with target device.|
-----------
**User Interaction :**
NA

**Test Procedure :**

| Variation / Steps | Description | Test Data | Expected Result | Notes |
|-------------------|-------------|-----------|----------------|-------|
| 1 | Obtain Flash service handle   | N/A | Valid handle acquired | Ensure HAL is initialized |
| 2 | Obtain flashing filename   | N/A | valid filename with filepath "/images/imagefile/flash1_0.img"| Ensure the device is connected to power |
| 3 | Obtain the listener| N/A | the listener should be type of IFlashListener  | |
| 4 | Call  flashImageFromFile() | To  Trigger the flashing | Function return the TRUE if flashing is started, return FALSE if flashing is not started  |  |
| 5 | Wait for onProgress() callback| Read percentComplete | Matches value between  Percentage complete (0~100) |  |
| 6 | Wait for onCompleted callback | Read result | Matches enum value FlashImageResult  SUCCESS = 0 | indicates Image was flashed successfully. |

### Test 2

| Title | Details |
| -- | -- |
| Function Name | `test_verifyFilepath` |
| Description | Validates the filename with correct path and throws error of type FlashImageResult if file path does not match |
| Test Group |  02 |
| Test Case ID | 002 |
| Priority | High |

**Pre-Conditions :**
Device should be connected to power and and communication channel should be established.

**Dependencies :**
None

**User Interaction :**
NA

**Test Procedure :**

| Variation / Steps | Description | Test Data | Expected Result | Notes |
|-------------------|-------------|-----------|----------------|-------|
| 1 | Obtain Flash service handle   | N/A | Valid handle acquired | Ensure HAL is initialized |
| 2 | Obtain wrong Image File path | Send the file path as "/doc/imagefile/flash1_0.img"| flash1_0.img is not present in given file |   |
| 3 | Call  flashImageFromFile() | To  Trigger the flashing | Function return the TRUE if flashing is started,return FALSE if flashing is not started |  |
| 4 | Wait for onProgress() callback| NA  | File property validation fails  | onProgress() is not triggered |
| 5 | Wait for onCompleted() callback | Read result | Matches enum value FlashImageResult  with ERROR_FILE_OPEN_FAIL = 1 | Failed to open input file. |

### Test 3

| Title | Details |
| -- | -- |
| Function Name | `test_verifyFiletype` |
| Description | Validates the filetype of image file and throws error of type FlashImageResult if file type is wrong |
| Test Group |  02 |
| Test Case ID | 003 |
| Priority | High |

**Pre-Conditions :**
Device should be connected to power and and communication channel should be established.

**Dependencies :**
None

**User Interaction :**
NA

**Test Procedure :**

| Variation / Steps | Description | Test Data | Expected Result | Notes |
|-------------------|-------------|-----------|----------------|-------|
| 1 | Obtain Flash service handle   | N/A | Valid handle acquired | Ensure HAL is initialized |
| 2 | Obtain Image with wrong File type | Send the image with file type as "/images/imagefile/flash1_0.o" | .o file type is not supported |   |
| 3 | Call  flashImageFromFile() | To  Trigger the flashing | Function return the TRUE if flashing is started,return FALSE if flashing is not started |  |
| 4 | Wait for onProgress() callback| NA  | File property validation fails  | onProgress() is not triggered |
| 5 | Wait for onCompleted() callback | Read result | Matches enum value FlashImageResult  with ERROR_IMAGE_INVALID_TYPE = 2 | The image cannot be recognised as a flashable image. |

### Test 4

| Title | Details |
| -- | -- |
| Function Name | `test_verifyImageSignature` |
| Description | Verify the signature of the image and throws error of type FlashImageResult if image is wrongly signed. |
| Test Group |  02 |
| Test Case ID | 004 |
| Priority | High |

**Pre-Conditions :**
Device should be connected to power and and communication channel should be established.

**Dependencies :**
None

**User Interaction :**
NA

**Test Procedure :**

| Variation / Steps | Description | Test Data | Expected Result | Notes |
|-------------------|-------------|-----------|----------------|-------|
| 1 | Obtain Flash service handle   | N/A | Valid handle acquired | Ensure HAL is initialized |
| 2 | Obtain Image with wrong signature | Send the image with wrong signature signed as "/images/imagefile/flash1_0.img"|  |   |
| 3 | Call  flashImageFromFile() | To  Trigger the flashing | Function return the TRUE if flashing is started, return FALSE if flashing is not started |  |
| 4 | Wait for onProgress() callback| NA  | File property validation fails  | onProgress() is not triggered |
| 5 | Wait for onCompleted() callback | Read result | Matches enum value FlashImageResult  with ERROR_IMAGE_INVALID_SIGNATURE  = 3 | The image signature failed verification. |


### Test 5

| Title | Details |
| -- | -- |
| Function Name | `test_verifyFileImageSize` |
| Description | Verify the size of the image for flashing and throws error of type FlashImageResult if file does not fit into flash area. |
| Test Group |  02 |
| Test Case ID | 005 |
| Priority | High |

**Pre-Conditions :**
Device should be connected to power and and communication channel should be established.

**Dependencies :**
None

**User Interaction :**
NA

**Test Procedure :**

| Variation / Steps | Description | Test Data | Expected Result | Notes |
|-------------------|-------------|-----------|----------------|-------|
| 1 | Obtain Flash service handle   | N/A | Valid handle acquired | Ensure HAL is initialized |
| 2 | Obtain Image with incorrect size | Send the image with incorrect size "/images/imagefile/flash1_0.img"|  |   |
| 3 | Call  flashImageFromFile() | To  Trigger the flashing | Function return the TRUE if flashing is started,return FALSE if flashing is not started |  |
| 4 | Wait for onProgress() callback| NA  | File property validation fails  | onProgress() is not triggered |
| 5 | Wait for onCompleted() callback | Read result | Matches enum value FlashImageResult  with ERROR_IMAGE_INVALID_SIGNATURE  = 4 | Image size does not fit into the target flash area. |

### Test 6

| Title | Details |
| -- | -- |
| Function Name | `test_verifyImageCompatibility` |
| Description | Verify the compatibility of the image with device and throws error of type FlashImageResult if file is not compatible. |
| Test Group |  02 |
| Test Case ID | 006 |
| Priority | High |

**Pre-Conditions :**
Device should be connected to power and and communication channel should be established.

**Dependencies :**
None

**User Interaction :**
NA

**Test Procedure :**

| Variation / Steps | Description | Test Data | Expected Result | Notes |
|-------------------|-------------|-----------|----------------|-------|
| 1 | Obtain Flash service handle   | N/A | Valid handle acquired | Ensure HAL is initialized |
| 2 | Obtain imcompatible image | Send the imcompatible "/images/imagefile/flash1_0.img"|  |  image doesn't match the device's expected specifications |
| 3 | Call  flashImageFromFile() | To  Trigger the flashing | Function return the TRUE if flashing is started, return FALSE if flashing is not started |  |
| 4 | Wait for onProgress() callback| NA  | File property validation fails  | onProgress() is not triggered |
| 5 | Wait for onCompleted() callback | Read result | Matches enum value FlashImageResult  with ERROR_IMAGE_INVALID_PRODUCT   = 5 | Image is imcompatible with device .|
