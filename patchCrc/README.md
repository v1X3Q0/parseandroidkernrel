# patchCrc

This project was created for the purposes of patching the crc of a linux kernel driver in the `.ko` format so that it may be loaded onto a different kernel then it was built for. Specifically, if an embedded device was built with a specific kernel that you cannot find online, then you can build the adjacent kernel and patch the crc's you need for each import so that loading is successful.

## building

As this is from the parseandroidkernel package, it can't be configured outside of the top project, however you can simply build the target afterwards.

```bash
cd ~/parseandroidkernel
mkdir build-out && cd build-out
cmake ..
make patchCrc
```

The executable should be there in your build directory.

## running

To run patchCrc, you just need the kernel driver.
