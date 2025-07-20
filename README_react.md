# Reaction Game
A reaction game for E2E GPIO control from hardware to application level.

## Setup and build
```
git clone git@github.com:litcoder/adproject.git
git submodule update --init --recursive
export TEMPLATECONF=${PWD}/meta-reaction-game/conf/templates/mytemplate
source ./poky/oe-init-build-env build-game
bitbake reaction-game-image
```
