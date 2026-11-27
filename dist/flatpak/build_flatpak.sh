#!/bin/bash
# Build the Flatpak locally and create the pikaball.flatpak bundle (in this directory).
# Extra arguments are passed to flatpak-builder, e.g. --install to also install it for the current user.
set -e
cd "$(dirname "$0")"

flatpak-builder \
    --force-clean \
    --user \
    --install-deps-from=flathub \
    --repo=repo \
    "$@" \
    builddir io.github.butakus.pikaball.yml

echo "Building pikaball.flatpak file..."
flatpak build-bundle \
    repo pikaball.flatpak \
    io.github.butakus.pikaball \
    --runtime-repo=https://flathub.org/repo/flathub.flatpakrepo
