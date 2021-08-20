#! /bin/sh

set -e

NAME="crossfire-cross-compiled"

PROJECTDIR="$(cd ../../.. && pwd)"

install -d "$HOME/.build-home/project"

OPTS=
if tty -s; then
    OPTS="$OPTS -t"
fi
if [ ! -z "$SSH_AUTH_SOCK" -a -e "$SSH_AUTH_SOCK" ]; then
    OPTS="$OPTS --volume=$SSH_AUTH_SOCK:/ssh-agent"
fi

docker run \
    --name="build-$NAME-$USER$$" \
    $OPTS \
    -i \
    --rm \
    --volume="$HOME/.build-home:/home/user" \
    --volume="$PROJECTDIR:/home/user/project" \
    --cap-add=SYS_PTRACE \
    --security-opt seccomp=unconfined \
    "build-$NAME-$USER" "$@"
