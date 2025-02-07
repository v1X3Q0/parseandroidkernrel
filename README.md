# parseandroidkernel

To build parse android kernel's entire suite, its important that a user has installed:

- json-c

- liblz4

## Linux dependencies

As described in the `.gitlab-ci.yml`, the linux dependencies do not require a complex line of code for preparation. In fact, all you need is:

```bash
sudo apt-get install liblz4-dev json-c
```

parseandroidkernel uses cmake, so make sure your system has that installed as well. 

## Mac dependencies

The first depencency, liblz4, can be installed using brew:

```bash
brew install lz4
```

Then for json-c, it is required that you download, build and install from the source package.

```bash
git clone https://github.com/json-c/json-c.git && cd json-c
mkdir build-out && cd build-out
cmake ..
make
sudo make install
```

From there, you should have the proper headers installed in the location `/usr/local/include`. Note that if you override the install location of json-c, you will have to adjust what the source does accordingly as it expects that to be the install location.

# building

To build, the default triple is fine.

```bash
mkdir build-out && cd build-out
cmake ..
make
```

There are a few command line parameters that can be decribed, for one `SV_GPL` can be overwritten, if it is then source will try to not build the sv_gpl libraries, which are dedicated to being a lookup table for if a kernel symbol is supposed to be `GPL` or not.