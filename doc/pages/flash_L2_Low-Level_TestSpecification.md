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

### Test 2

| Title | Details |
| -- | -- |
| Function Name | `test_l2_Flash_flashImageFromFile` |
| Description |  Flashes an image file onto the device.|
| Test Group | Module : 01 |
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
| 1 | Obtain flashing filename   | N/A | valid filename | Ensure the sevice in connected to power |
| 2 | Obtain the listener| N/A | the listener should be type of IFlashListener  | |
| 3 | Call  flashImageFromFile() | To  Trigger thr flasging | Function return the TRUE if flashing is started |  |
| 4 | Wait for onProgress() callback| Read percentComplete | Matches value between  Percentage complete (0~100) |  |
| 5 | Wait for onCompleted callback | Read result | Matches enum value FlashImageResult  SUCCESS = 0 | indicates Image was flashed successfully. |

### Test 2

| Title | Details |
| -- | -- |
| Function Name | `test_l2_Flash_varifyFileProperties` |
| Description | Validates the filename with given file specification and throws error FlashImageResult if properties does not match |
| Test Group | Module : 02 |
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
| 1 |Obtain wrong Image File path | send the file name with incorrect file filepath to flashImageFromFile()| Listener should send callback onCompleted() with ERROR_FILE_OPEN_FAIL = 1 |  Failed to open input file |
| 2 | Obtain invalid File Type |send the invalid filetype to flashImageFromFile() | Listener should send callback onCompleted() with  ERROR_IMAGE_INVALID_TYPE = 2 | The image cannot be recognised as a flashable image. |
| 3 | Obtain the image with failed signature | send the file with failed signature image to  flashImageFromFile()| Listener should send callback onCompleted() with ERROR_IMAGE_INVALID_SIGNATURE = 3|  The image signature failed verification. |
| 4 | Obtain the incoorect size image file | Send the image file with incorrect size to flashImageFromFile() | Listener should send callback onCompleted() with ERROR_IMAGE_INVALID_SIZE = 4| Image size does not fit into the target flash area. |
| 4 | Obtain imcompatible image file | Send the incompatible image file to flashImageFromFile() | Listener should send callback onCompleted() with ERROR_IMAGE_INVALID_PRODUCT  = 5|  Image is incompatible with this product. |
