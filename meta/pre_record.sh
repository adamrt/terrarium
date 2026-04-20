#!/usr/bin/env bash

# Clean up before recording

set -euo pipefail

OS="$(uname -s)"

# Emacs
rm -rf ~/.config/emacs/.local/cache/{savehist,saveplace,recentf}
rm -rf ~/.config/emacs/.local/cache/projectile/*
echo "(\"~/src/terrarium/\")" > ~/.config/emacs/.local/cache/projectile/projects.eld

case "$OS" in
    Darwin)
        # Safari
        killall Safari 2>/dev/null
        rm -rf ~/Library/Containers/com.apple.Safari/Data/Library/Caches/*
        rm -rf ~/Library/Safari/History.db*
        rm -rf ~/Library/Cookies/Cookies.binarycookies
        rm -rf ~/Library/Safari/LocalStorage/*.localstorage
        rm -rf ~/Library/Saved\ Application\ State/com.apple.Safari.savedState
        ;;
    Linux)
        # Firefox
        pkill firefox
        for profile in ~/.mozilla/firefox/*.default*; do
            [ -d "$profile" ] || continue
            rm -f "$profile"/places.sqlite*
            rm -f "$profile"/favicons.sqlite*
            rm -f "$profile"/cookies.sqlite*
            rm -f "$profile"/formhistory.sqlite*
            rm -f "$profile"/sessionstore.jsonlz4
            rm -rf "$profile"/sessionstore-backups/*
        done
        ;;
    *)
        echo "Unsupported OS: $OS" >&2
        exit 1
        ;;
esac
