#!/bin/sh

LD_LIBRARY_PATH="/lib:/usr/lib:$STAGING_DIR_HOST/lib:$STAGING_DIR_HOSTPKG/lib" clippy "$@"
