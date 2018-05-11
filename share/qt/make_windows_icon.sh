#!/bin/bash
# create multiresolution windows icon
ICON_DST=../../src/qt/res/icons/bluecoin.ico

convert ../../src/qt/res/icons/bluecoin-16.png ../../src/qt/res/icons/bluecoin-32.png ../../src/qt/res/icons/bluecoin-48.png ${ICON_DST}
