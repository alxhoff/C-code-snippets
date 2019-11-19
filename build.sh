#!/bin/bash

for D in *; do
    if [ -d "${D}" ]; then
        echo "Building ${D}"
        cd ${D}
        if test -f ".dontbuild"; then
            continue
        fi
        mkdir -p build
        cmake -B build
        make -C build
        cd ..
    fi
done

exit 0
