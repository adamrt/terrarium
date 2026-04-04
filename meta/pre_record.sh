#!/usr/bin/env bash

# Clean up before recording

set -euo pipefail

# Emacs
rm -rf ~/.config/emacs/.local/cache/{savehist,saveplace,recentf}
rm -rf ~/.config/emacs/.local/cache/projectile/*
echo "(\"~/src/terrarium/\")" > ~/.config/emacs/.local/cache/projectile/projects.eld

# Safari
killall Safari 2>/dev/null
rm -rf ~/Library/Containers/com.apple.Safari/Data/Library/Caches/*
rm -rf ~/Library/Safari/History.db*
rm -rf ~/Library/Cookies/Cookies.binarycookies
rm -rf ~/Library/Safari/LocalStorage/*.localstorage
rm -rf ~/Library/Saved\ Application\ State/com.apple.Safari.savedState
