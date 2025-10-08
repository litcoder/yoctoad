# ADProject

Enables ROS2 and Python running environment on Rasberry-pi5.

## Host Build Tools
Assuming Ubuntu24.04.
```
sudo apt install chrpath diffstat gawk zstd liblz4-tool
```

## Clone
```
git clone git@github.com:litcoder/yoctoad.git
git submodule update --init --recursive
```

## Setup and Build
```
export YOCTO_HOME=${PWD}
export TEMPLATECONF=${YOCTO_HOME}/meta-myad/conf/templates/mytemplate
source ./poky/oe-init-build-env build
bitbake ros-image-core
```

## Flash

Note: replace the `/dev/mmcblk0` below with yours, which can be found with `lsblk` command.

```
export WIC_FILE=$YOCTO_HOME/build/tmp/deploy/images/raspberrypi5/ros-image-core-humble-raspberrypi5.rootfs.wic.bz2
bzcat $WIC_FILE | sudo dd of=/dev/mmcblk0 bs=4M status=progress conv=fsync
```
