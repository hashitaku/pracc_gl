#!/bin/bash

find ./subprojects -name *.wrap |\
xargs -I '{}' basename {} .wrap |\
xargs -I '{}' meson wrap update {}
