# ADProject

## Setup and build
```
git clone git@github.com:litcoder/adproject.git
git submodule update --init --recursive
export TEMPLATECONF=${PWD}/meta-myad/conf/templates/mytemplate
source ./poky/oe-init-build-env build
bitbake ros-image-base
```
