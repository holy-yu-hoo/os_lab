#!/bin/bash -e

if [[ -z "$1" ]]; then
    echo "Error: file not select"
    exit 1
fi

filename="$1"
match=$(grep -oE "Output:\s*([a-zA-Z0-9_\.-]+).*" "$filename")

if [[ -z match ]]; then
	echo "Error: output name not found"
fi

output_filename=${match##*Output:}
# output_filename+=".o"


deltmp() {
    rm -rf "$tmpdir"
}

trap deltmp EXIT INT TERM

tmpdir=$(mktemp -d)

cp "$filename" "$tmpdir/"
tmpsrc="$tmpdir/$(basename "$filename")"
tmpout="$tmpdir/$output_filename"

echo "compilation file"
g++ "$tmpsrc" -o "$tmpout"

srcdir=$(dirname "$(realpath "$filename")")
mv "$tmpout" "$srcdir/$output_filename"

rm -rf "$tmpdir"
