#! /bin/sh

set -e

NAME="crossfire-cross-compiled"

BUILDDIR="/tmp/build-setup$$"
trap "rm -rf '$BUILDDIR'" 0
rm -rf "$BUILDDIR"
mkdir "$BUILDDIR"
if [ -e build-setup.proxy ]; then
    install build-setup.proxy "$BUILDDIR/build-setup-proxy"
else
    install /dev/null "$BUILDDIR/build-setup-proxy"
fi
UID="$(id -u)"
if [ "$UID" -eq 0 ]; then
    echo "cannot run as root" 1>&2
    exit 1
fi
if [ "$UID" -lt 1000 ]; then
    echo "cannot run with a user id less than 1000" 1>&2
    exit 1
fi
cat >"$BUILDDIR/Dockerfile" <<EOF
FROM debian:stretch

RUN DEBIAN_FRONTEND=noninteractive TERM=xterm apt-get update && apt-get install -y \
    software-properties-common \
    lsb-release \
    gnupg2 \
    apt-transport-https \
    adduser \
    sudo \
    automake

RUN apt-key adv \
    --keyserver keyserver.ubuntu.com \
    --recv-keys 86B72ED9

RUN DEBIAN_FRONTEND=noninteractive TERM=xterm add-apt-repository \
    "deb [arch=amd64] https://pkg.mxe.cc/repos/apt/ `lsb_release -sc` main"

RUN DEBIAN_FRONTEND=noninteractive TERM=xterm apt-get update

RUN DEBIAN_FRONTEND=noninteractive TERM=xterm apt-get install -y \
    mxe-x86-64-w64-mingw32.static-cc \
    mxe-x86-64-w64-mingw32.static-qt5 \
    mxe-x86-64-w64-mingw32.static-libgcrypt \
    mxe-x86-64-w64-mingw32.static-curl \
    mxe-x86-64-w64-mingw32.static-gendef

RUN echo export PS1=\'[$NAME] \\\w\\\\\$ \' >>/etc/bash.bashrc
RUN adduser --quiet --disabled-login --gecos '' --uid '$UID' user
COPY entrypoint.sh /entrypoint.sh
COPY entrypoint2.sh /entrypoint2.sh
RUN chmod 755 /entrypoint.sh /entrypoint2.sh
WORKDIR /home/user
ENTRYPOINT ["/entrypoint.sh"]
CMD ["/bin/bash"]
EOF
cat >"$BUILDDIR/entrypoint.sh" <<EOF
#! /bin/sh
exec sudo -i -u user /entrypoint2.sh "\$@"
EOF
cat >"$BUILDDIR/entrypoint2.sh" <<EOF
#! /bin/sh
export SSH_AUTH_SOCK=/ssh-agent
export LESS="-fMnQSR"
export PAGER="/usr/bin/less -fMnQSR"
export PATH="/usr/lib/mxe/usr/bin:${PATH}"
cd
cd project
exec "\$@"
EOF
docker image rm --no-prune "build-$NAME-$USER" >/dev/null 2>&1 || true
docker build -t "build-$NAME-$USER" "$BUILDDIR"
