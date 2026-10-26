#!/bin/sh
set -eu

check_only=false
if [ "${1:-}" = --check ]; then
    check_only=true
elif [ "$#" -ne 1 ] || [ ! -f "$1" ]; then
    printf 'Usage : %s ROM.gb | --check\n' "$0" >&2
    exit 1
fi

if [ -n "${RUNNER:-}" ]; then
    command -v "$RUNNER" >/dev/null 2>&1 || { printf 'Exécutable introuvable : %s\n' "$RUNNER" >&2; exit 1; }
    printf 'Émulateur : %s\n' "$RUNNER"
    if [ "$check_only" = false ]; then exec "$RUNNER" "$1"; fi
    exit 0
fi

if [ "$(uname -s)" = Darwin ]; then
    if [ -z "${SAMEBOY_APP:-}" ]; then
        for candidate in /Applications/SameBoy.app "$HOME/Applications/SameBoy.app"; do
            if [ -d "$candidate" ]; then SAMEBOY_APP=$candidate; break; fi
        done
    fi
    if [ -n "${SAMEBOY_APP:-}" ]; then
        test -x "$SAMEBOY_APP/Contents/MacOS/SameBoy" || { printf 'Application SameBoy invalide : %s\n' "$SAMEBOY_APP" >&2; exit 1; }
        printf 'SameBoy : %s\n' "$SAMEBOY_APP"
        if [ "$check_only" = false ]; then exec open -a "$SAMEBOY_APP" "$1"; fi
        exit 0
    fi
fi

for candidate in sameboy SameBoy; do
    if command -v "$candidate" >/dev/null 2>&1; then
        printf 'SameBoy : %s\n' "$(command -v "$candidate")"
        if [ "$check_only" = false ]; then exec "$candidate" "$1"; fi
        exit 0
    fi
done
printf 'SameBoy introuvable. Définir SAMEBOY_APP (bundle macOS) ou RUNNER (exécutable) dans config.local.mk.\n' >&2
exit 1
