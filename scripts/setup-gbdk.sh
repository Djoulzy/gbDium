#!/bin/sh
set -eu

# Official release assets: https://github.com/gbdk-2020/gbdk-2020/releases/tag/4.5.0
version=4.5.0
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
destination="$root/.tools/gbdk"

if [ -x "$destination/bin/lcc" ] && [ -f "$destination/.gbdium-version" ] && [ "$(cat "$destination/.gbdium-version")" = "$version" ]; then
    printf 'GBDK %s déjà installé : %s\n' "$version" "$destination"
    exit 0
fi
if [ -e "$destination" ]; then
    printf 'Le dossier %s existe déjà. Définir GBDK_HOME pour le réutiliser, ou le déplacer avant make setup.\n' "$destination" >&2
    exit 1
fi

case "$(uname -s)-$(uname -m)" in
    Darwin-arm64)
        archive=gbdk-macos-arm64.tar.gz
        checksum=289ee60e46c5a2785a21e35533f84a5131ed4a063b21b0dbdedc9a10af15bf78 ;;
    Darwin-x86_64)
        archive=gbdk-macos.tar.gz
        checksum=1aa549d12032d8f6509d11923bb28b1a453098f42597feb378e9a42541f8fd89 ;;
    Linux-x86_64)
        archive=gbdk-linux64.tar.gz
        checksum=d7857a5f6d135ee4c249043ca26aad9f2ec8ab5d4106d97720d404114f42605c ;;
    Linux-aarch64|Linux-arm64)
        archive=gbdk-linux-arm64.tar.gz
        checksum=31eb2235f0fdb60163d0b1e9574a022098d6069cd56606a1daca4478a46e0439 ;;
    *)
        printf 'Plateforme non prise en charge automatiquement. Installer GBDK manuellement et définir GBDK_HOME.\n' >&2
        exit 1 ;;
esac

for tool in curl tar; do
    command -v "$tool" >/dev/null 2>&1 || { printf 'Outil requis : %s\n' "$tool" >&2; exit 1; }
done
if command -v shasum >/dev/null 2>&1; then
    checksum_tool=shasum
elif command -v sha256sum >/dev/null 2>&1; then
    checksum_tool=sha256sum
else
    printf 'Outil requis : shasum ou sha256sum\n' >&2
    exit 1
fi

mkdir -p "$root/.tools"
temporary=$(mktemp -d "$root/.tools/gbdk-install.XXXXXX")
trap 'rm -rf "$temporary"' EXIT HUP INT TERM
printf 'Téléchargement de GBDK %s (%s)…\n' "$version" "$archive"
curl --fail --location --retry 3 --connect-timeout 15 --max-time 300 \
    "https://github.com/gbdk-2020/gbdk-2020/releases/download/$version/$archive" \
    --output "$temporary/$archive"
if [ "$checksum_tool" = shasum ]; then
    actual=$(shasum -a 256 "$temporary/$archive")
else
    actual=$(sha256sum "$temporary/$archive")
fi
if [ "${actual%% *}" != "$checksum" ]; then
    printf 'Échec de la vérification SHA-256 de %s. Installation annulée.\n' "$archive" >&2
    exit 1
fi
tar -xzf "$temporary/$archive" -C "$temporary"
test -x "$temporary/gbdk/bin/lcc"
printf '%s\n' "$version" > "$temporary/gbdk/.gbdium-version"
mv "$temporary/gbdk" "$destination"
printf 'GBDK %s installé : %s\n' "$version" "$destination"
