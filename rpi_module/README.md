# RPi module

Thanks to the RPi docs, building a module for the board is a lot easier than other ARM based MPSoCs. 

Pretty much a copy from their docs:

## Get toolchain

```
git clone https://github.com/raspberrypi/tools ~/tools
```

Add toolchain to your path, or you can just pass is directly to make

``` 
echo PATH=\$PATH:~/tools/arm-bcm2708/arm-linux-gnueabihf/bin >> ~/.bashrc
source ~/.bashrc
```

## Get kernel sources

```
git clone --depth=1 https://github.com/raspberrypi/linux
```

## Build kernel

You will need to build and install the complete kernel so that your module shares the same version, otherwise you won't be able to load the kernel.

You should only have to do this once, you should then be able to meerly sent your updated module (after each build) via the network or however you so desire.

### Prereq packages
```
sudo apt install git bc bison flex libssl-dev make libc6-dev libncurses5-dev
```

For other distros (arch based) and more info on embedded Linux modules etc, check out [this](https://github.com/alxhoff/Ath9k) driver that I also documented somewhat decently.

To build the kernel enter the directory and set the `KERNEL` variable which is used later as it gives the compiled kernel's binary's name.

I am building for a RPi zero(RPi 1) so I want `KERNEL=kernel` but I will later be also building for a RPi 4 so there I will need `KERNEL=kernel71`.

```
cd linux
KERNEL=kernel
```
### Config

Then we can just invoke make, specifying the architecture (arm), the cross compiler and the config appropriate for our SoC.
If you exported the toolchain to your path you should be fine just passing in `CROSS_COMPILE=arm-linux-gnueabihf` but I have a lot of toolchains on my system and prefer to manually pass in the CC to the make command with an absolute path.

If you export toolchain to path:
```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcmrpi_defconfig
```
 or like me

 ```
 make ARCH=arm CROSS_COMPILE=~/tools/arm-bcm2708/arm-linux-gnueabihf/bin/arm-linux-gnueabihf- bcmrpi_defconfig
 ```

 Now the kernel should of generated the required `.config`. If it complains in the next step that the `.config` is missing then you forgot to specify the architecture (`ARCH`).

### Build

Invoke make

```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
```

or

```
make ARCH=arm CROSS_COMPILE=~/tools/arm-bcm2708/arm-linux-gnueabihf/bin/arm-linux-gnueabihf- zImage modules dtbs
```

This could take a while.

## Installing

Now the first time we will need to install the entire kernel.

A RPi SD card has two partitions, one is `/boot` and the other is the root filesystem `/`. 
The kernel resides in the boot partition.
Running `fdisk -l` you will see something like

```
Disk /dev/sdc: 14,86 GiB, 15931539456 bytes, 31116288 sectors
Disk model: MassStorageClass
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0xea7d04d6

Device     Boot  Start      End  Sectors  Size Id Type
/dev/sdc1         8192   532479   524288  256M  c W95 FAT32 (LBA)
/dev/sdc2       532480 31116287 30583808 14,6G 83 Linux
```

Here our boot partition is `/dev/sdc1` and the root filesystem is `/dev/sdc2`.

We only need to modify the boot partiton so we shall mount this.

Create a directory to mount to and mount the `/boot` partition.

```
sudo mkdir /mnt/rpi_boot
sudo mount /dev/sdc1 /mnt/rpi_boot
```

Now we can copy our kernel over, the kernel binary is called `zImage` and this needs to be copied over and names after our `$KERNEL` variable, set before.
We will also need to copy the device tree binary and overlays, found in `arch/arm/boot/dts`.

```
sudo cp arch/arm/boot/zImage /mnt/rpi_boot/$KERNEL.img
sudo cp arch/arm/boot/dts/*.dtb /mnt/rpi_boot/
sudo cp arch/arm/boot/dts/overlays/*.dtb /mnt/rpi_boot/overlays
```
Unmount the sd card partition

```
sudo umount /mnt/rpi_boot
```

plug it in and boot up.

# Building a kernel module

Now you can either put your module into the kernel source and perform an "in-tree" build where you compile the entire kernel (slow, but important if the current kernel version differs).

Or you can use a make file where you pass it the kernel sources so it can use only what it needs and not build the entire kernel, only your module (fast). 
This is an "out-of-tree" build.

The in-tree build is the same as above, just add your module and the appropriate additions to the kbuild files etc.

## Out-of-tree build

Let's move to a seperate directory that isn't in the linux source, thus "out-of-tree".
You can invoke the build manually with something like this

```
make -C <path_to_kernel_src> M=$PWD
```

but it's easier to use a make file, something similar to the one in this directory, [here](Makefile).

We just need to provide the kernel source directory and the cross compiling toolchain.

```
make KDIR=~/linux/ CROSS_COMPILE=~/tools/arm-bcm2708/arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
```

Then you just need to copy the module onto the RPi. Inspiration can be found [here](https://github.com/alxhoff/dotfiles/blob/master/random_scripts/build_ath9k.sh).
Modules are found in `/lib/modules/${KERNEL_VERSION}/`
Make sure to `depmod`.

I like to use `scp`.

```

```
