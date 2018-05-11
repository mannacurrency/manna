#!/bin/sh

if [ $# -gt 0 ]; then
    FILE="$1"
    shift
    if [ -f "$FILE" ]; then
        INFO="$(head -n 1 "$FILE")"
    fi
else
    echo "Usage: $0 <filename>"
    exit 1
fi

if [ -e "$(which hg)" ]; then
    # get a string like "472fdd7d1efd+"
    DESC="$(hg identify -i)"

    # get a string like "2012-04-10 16:27:19 +0200"
    TIME="$(hg tip --template "{date|isodate}\n")"
fi

if [ -n "$DESC" ]; then
    NEWINFO="#define BUILD_DESC \"$DESC\""
else
    NEWINFO="// No build information available"
fi

# only update build.h if necessary
if [ "$INFO" != "$NEWINFO" ]; then
    echo "$NEWINFO" >"$FILE"
    echo "#define BUILD_DATE \"$TIME\"" >>"$FILE"
fi
