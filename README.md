# Testing Suite For FLASH HAL

## Build Steps

### 1. Build linux binder

The test suite depends on the Linux binder with `AIDL` support. Follow these steps to build it:

For detail build instruction, please refer to [Linux Binder](https://github.com/rdk-e/linux_binder_idl/tree/main)

Here outline the simplest steps are

```bash
git clone -b develop 'ssh://git@github.com/rdk-e/linux_binder_idl.git'

cd linux_binder_idl

./build-linux-binder-aidl.sh 64
```

### 2. Build this test

Clone and build the test suite:

```bash
git clone git@github.com:rdkcentral/rdk-halif-aidl-test-flash.git
```

```bash
cd rdk-halif-aidl-test-flash
```

Set below environment variable:

- AIDL_BIN: Path to the `AIDL` compiler (e.g., /path/to/linux_binder_idl/out/aidl).

- LINUX_BINDER_OUT_DIR: Path to the Linux binder output directory (e.g., /path/to/linux_binder_idl/local).

- UT_CORE_VERSION: Specific version of the UT-Core framework (e.g., 4.7.0). Defaults to the latest tagged version.

- RDK_HALIF_AIDL_VERSION: Specific version of the AIDL HAL Specification (e.g, 0.7.1). Defaults to the latest HEAD.

Example:

```bash
export UT_CORE_VERSION=4.7.0
export RDK_HALIF_AIDL_VERSION=0.7.1
export AIDL_BIN=/path/linux_binder_idl/out/aidl
export LINUX_BINDER_OUT_DIR=/path//linux_binder_idl/local/
```

By default, UT logs are disabled. To enable them, Set below environment variable

```bash
export ENABLE_UT_INFO_LOGGING=1
```

Run the build script:

```bash
./build.sh TARGET=linux
```

The build.sh script:

- Clones the `rdk-halif-aidl` repository and UT-Core framework.

- Compiles the test suite.

- Generates binaries in the `build/bin` directory.

- Also generates `vts-flash.tar.gz` tar ball in `build/bin` folder

## Run test

- Transfer to Target Device:

Copy the tarball to the target device (e.g., using scp):

```bash
scp -p <port> vts-flash.tar.gz root@target-device:/home/root/

```

- Extract on Target Device:

Log into the target device and extract the tarball:

```bash
ssh root@target-device
cd /home/root
mkdir flash
tar -zxf vts-flash.tar.gz -C flash
cd flash
```

- Run the VTS Test on Target Device

Run below command to execute the VTS Test:

```bash
./run.sh 
```
