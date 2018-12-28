#!/bin/sh

## thsh launcher 
#
#  This shell script ensures that the thsh shell
#  gets control key strokes through standard in.
#
#  Launch it like this: . ./launcher.sh
#
#  Note that there should be a dot '.' first, followed by a space.
#  This "sources" the script, importing the commands into the parent,
#  rather than invoking them in a child script.
#  This script should be sourced to properly disable certain shell features.
#

set +m
old="$(stty -g)"
trap '' 2
trap "stty $(stty -g)" EXIT

stty  -echo -icanon -isig || exit $?
./thsh $@

# Clean up
set -m
stty "$old"
