SUMMARY = "Reaction Game Qt6 C++ Application"
DESCRIPTION = "A Qt6 C++-based reaction game application for Raspberry Pi"

LICENSE = "CLOSED"

SRC_URI = "file://reaction_game.cpp \
           file://reaction_game.h \
           file://CMakeLists.txt \
           file://reaction_game.desktop"

S = "${WORKDIR}"

DEPENDS += "qtbase libgpiod"
RDEPENDS:${PN} += "qtbase libgpiod weston"

inherit cmake qt6-cmake

do_install() {
    install -d ${D}${bindir}
    install -d ${D}${datadir}/applications
    
    install -m 0755 ${B}/reaction_game ${D}${bindir}/
    install -m 0644 ${WORKDIR}/reaction_game.desktop ${D}${datadir}/applications/
}
