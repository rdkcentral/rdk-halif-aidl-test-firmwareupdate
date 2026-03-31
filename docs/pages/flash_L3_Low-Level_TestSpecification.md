# Flash HAL L3 Low Level Test Specification and Procedure Documentation

## Table of Contents

* [Scope](#scope)
* [Level 3 Test Procedure](#level-3-test-procedure)
* [Level 3 Test Cases Overview](#level-3-test-cases-overview)

---

## Scope

This document describes the Level 3 (L3) Low Level Test Specification for the Flash HAL.

L3 testing validates end-to-end Flash HAL functionality using real device operations, including:

* Flashing valid images
* Handling invalid/corrupted images
* Verifying error codes
* Concurrent request handling
* Progress callback validation

---

## Level 3 Test Procedure

The L3 Flash test suite performs:

1. YAML configuration parsing (image + expected result)
2. File transfer to DUT
3. Execution of Flash HAL APIs via UT framework
4. Monitoring progress via listener callbacks
5. Validation of final flash result and error codes

---

## Level 3 Test Cases Overview

| Test Case                   | Description                                           | HAL APIs                       |
| --------------------------- | ----------------------------------------------------- | ------------------------------ |
| flash_imageTypes            | Validate flashing of valid image types                | `IFlash::flashImageFromFile()` |
| flash_concurrentReject      | Validate rejection of concurrent flash requests       | `IFlash::flashImageFromFile()` |
| flash_validateAllErrorcodes | Validate all negative image scenarios and error codes | `IFlash::flashImageFromFile()` |

---

# Test Case Details

---

## Test 1: flash_imageTypes

| Title         | Details                                                                                              |
| ------------- | ---------------------------------------------------------------------------------------------------- |
| Function Name | `flash_imageTypes`                                                                                   |
| Description   | Validates flashing of supported image types and ensures progress callbacks and successful completion |
| Test Group    | 03                                                                                                   |
| Test Case ID  | 001                                                                                                  |
| Priority      | High                                                                                                 |

### Pre-Conditions

* A valid image file must be present on the DUT, and its path must be updated in [test_configuration/vts_flash_image_config.yaml](test_configuration/vts_flash_image_config.yaml)
* YAML updated dynamically by host test
[test_configuration/vts_l3_flash.yaml](test_configuration/vts_l3_flash.yaml)
* Flash HAL service must be running
* Listener interface must be registered

### Dependencies

* Flash HAL Interface: [IFlash](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.14.0/flash/current/com/rdk/hal/flash/IFlash.aidl)
* YAML Configuration: [test_configuration/vts_l3_flash.yaml](test_configuration/vts_l3_flash.yaml)
* UT Framework

### User Interaction

No

---

### Test Flow Diagram

```mermaid
graph TD
    A[Parse YAML Config] --> B{Config Valid?}
    B -->|No| C[FAIL]
    B -->|Yes| D[Create Listener]
    D --> E[Call flashImageFromFile]
    E --> F{API Success?}
    F -->|No| G[FAIL]
    F -->|Yes| H[Wait for Completion]
    H --> I{Result SUCCESS?}
    I -->|No| J[FAIL]
    I -->|Yes| K[Validate Progress 0→100]
    K --> L[PASS]
```

---

### Test Procedure

| Step | Description              | Test Data     | Expected Result            |
| ---- | ------------------------ | ------------- | -------------------------- |
| 1    | Parse YAML configuration | Flash.L3.Test | Config parsed successfully |
| 2    | Get filename & listener  | Valid image   | Inputs valid               |
| 3    | Call API                 | filename      | API returns success        |
| 4    | Monitor progress         | Listener      | Progress from 0 to 100     |
| 5    | Wait for completion      | Timeout       | Completion received        |
| 6    | Validate result          | SUCCESS       | Test PASS                  |

---

## Test 2: flash_concurrentReject

| Title         | Details                                               |
| ------------- | ----------------------------------------------------- |
| Function Name | `flash_concurrentReject`                              |
| Description   | Validates that concurrent flash requests are rejected |
| Test Group    | 03                                                    |
| Test Case ID  | 002                                                   |
| Priority      | High                                                  |

### Pre-Conditions

* A valid image file must be present on the DUT, and its path must be updated in [test_configuration/vts_flash_image_config.yaml](test_configuration/vts_flash_image_config.yaml)
* YAML updated dynamically by host test
[test_configuration/vts_l3_flash.yaml](test_configuration/vts_l3_flash.yaml)
* Flash HAL service running

### Dependencies

* Flash HAL Interface: [IFlash](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.14.0/flash/current/com/rdk/hal/flash/IFlash.aidl)
* Listener implementation

### User Interaction

No

---

### Test Flow Diagram

```mermaid
graph TD
    A[Start First Flash] --> B{Success?}
    B -->|No| C[FAIL]
    B -->|Yes| D[Start Second Flash]
    D --> E{Rejected?}
    E -->|No| F[FAIL]
    E -->|Yes| G[Wait First Completion]
    G --> H{SUCCESS?}
    H -->|No| I[FAIL]
    H -->|Yes| J[PASS]
```

---

### Test Procedure

| Step | Description        | Test Data | Expected Result        |
| ---- | ------------------ | --------- | ---------------------- |
| 1    | Start first flash  | filename  | Success                |
| 2    | Start second flash | same file | Rejected (ret=false)   |
| 3    | Wait completion    | listener1 | Completes successfully |
| 4    | Validate result    | SUCCESS   | Test PASS              |

---

## Test 3: flash_validateAllErrorcodes

| Title         | Details                                                             |
| ------------- | ------------------------------------------------------------------- |
| Function Name | `flash_validateAllErrorcodes`                                       |
| Description   | Validates Flash HAL behavior for all invalid images and error codes |
| Test Group    | 03                                                                  |
| Test Case ID  | 003                                                                 |
| Priority      | High                                                                |

### Pre-Conditions
* All Error simulating images file must be present on the DUT, and its path must be updated in  [test_configuration/vts_flash_image_config.yaml](test_configuration/vts_flash_image_config.yaml)
* YAML updated dynamically by host test
[test_configuration/vts_l3_flash.yaml](test_configuration/vts_l3_flash.yaml)
### Dependencies

* Flash HAL Interface: [IFlash](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.14.0/flash/current/com/rdk/hal/flash/IFlash.aidl)
* Host-side script updates YAML
* Error code mapping

### User Interaction

No

---

### Test Flow Diagram

```mermaid
graph TD
    A[Load Negative Image] --> B[Update YAML]
    B --> C[Call flashImageFromFile]
    C --> D{API Success?}
    D -->|No| E[FAIL]
    D -->|Yes| F[Wait Completion]
    F --> G{ErrorCode Match?}
    G -->|No| H[FAIL]
    G -->|Yes| I[PASS]
```

---

### Test Procedure

| Step | Description         | Test Data         | Expected Result |
| ---- | ------------------- | ----------------- | --------------- |
| 1    | Load negative image | invalid file      | Ready           |
| 2    | Update YAML         | expected error    | Config updated  |
| 3    | Call API            | filename          | API success     |
| 4    | Wait completion     | listener          | Completed       |
| 5    | Validate error      | expectedErrorCode | Matches         |
| 6    | Ensure not success  | != SUCCESS        | Test PASS       |

---

## References

* Flash HAL Interface: [IFlash](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.14.0/flash/current/com/rdk/hal/flash/IFlash.aidl)
* L3 Config: [test_configuration/vts_l3_flash.yaml](test_configuration/vts_l3_flash.yaml)
* Image Config: [test_configuration/vts_flash_image_config.yaml](test_configuration/vts_flash_image_config.yaml)

---
