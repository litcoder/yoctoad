SUMMARY = "Reaction Game Image for Raspberry Pi"
DESCRIPTION = "A custom Linux image for Raspberry Pi with PyQt GUI and reaction game components"

LICENSE = "CLOSED"

# Inherit from core-image-base to get a minimal Linux system
inherit core-image

# Base image features
IMAGE_FEATURES += "ssh-server-dropbear debug-tweaks x11-base"

# Core system packages
IMAGE_INSTALL:append = " \
    packagegroup-core-boot \
    packagegroup-core-ssh-dropbear \
    kernel-modules \
    python3 \
    python3-pip \
    python3-setuptools \
    i2c-tools \
"

# Hardware support packages
IMAGE_INSTALL:append = " \
    rpi-gpio \
    raspi-gpio \
    libgpiod \
    libgpiod-tools \
    python3-gpiod \
"

# Weston compositor and graphics support
IMAGE_INSTALL:append = " \
    weston \
    weston-init \
    weston-examples \
    wayland \
    wayland-utils \
    mesa \
    mesa-demos \
    libdrm \
    libdrm-tests \
"

# Qt6 and C++ GUI support
IMAGE_INSTALL:append = " \
    qtbase \
    qtbase-plugins \
    qtbase-tools \
    qtwayland \
    qtwayland-plugins \
"

# Additional GUI and multimedia support
IMAGE_INSTALL:append = " \
    gstreamer1.0 \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    alsa-utils \
    alsa-lib \
    pulseaudio \
    pulseaudio-server \
"

# Custom reaction game application
IMAGE_INSTALL:append = " \
    reaction-game \
"
# Set root filesystem size (in KB)
IMAGE_ROOTFS_SIZE ?= "4194304"

# Extra space for package installation (in KB)
IMAGE_ROOTFS_EXTRA_SPACE:append = " + 1048576"

# Image format
IMAGE_FSTYPES = "tar.bz2 rpi-sdimg"
