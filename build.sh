#!/bin/bash

for D in *; do
    if [ -d "${D}" ]; then
        echo "Building ${D}"
        pushd ${D}
        echo "In directory $(pwd)"
        if test -f ".dontbuild"; then
            echo "${D} marked to not be built, SKIPPING"
            continue
        fi
        mkdir -p build
        pushd build
        cmake ..
        make
        popd
        popd
    fi
done

exit 0
