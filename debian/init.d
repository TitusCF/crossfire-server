#! /bin/sh
#
# This file was automatically customized by debmake on Sun, 11 Oct 1998 18:12:42 -0700
#
# Written by Miquel van Smoorenburg <miquels@drinkel.ow.org>.
# Modified for Debian GNU/Linux by Ian Murdock <imurdock@gnu.ai.mit.edu>.
# Modified for Debian by Christoph Lameter <clameter@debian.org>

PATH=/bin:/usr/bin:/sbin:/usr/sbin
DAEMON=/usr/games/crossfire
# The following value is extracted by debstd to figure out how to generate
# the postinst script. Edit the field to change the way the script is
# registered through update-rc.d (see the manpage for update-rc.d!)
FLAGS="defaults 50"

test -f $DAEMON || exit 0

umask 002

case "$1" in
  restart|force-reload)
    start-stop-daemon --stop --verbose --exec $DAEMON
    echo -n "waiting a bit for the server to stop"
    count=0
    while test "`ps aux|grep /usr/games/crossfire|wc -l`" -gt 1 
	do
	count=`expr $count + 1`
	if [ $count -gt 30 ]
	    then
	    echo -n "crossfire not ready, not waiting any longer..."
	    break
	fi
	echo -n "."
	sleep 1
    done
    echo
    start-stop-daemon --start --chuid games:games --verbose --exec $DAEMON -- -detach
    ;;
  start)
    start-stop-daemon --start --chuid games:games --verbose --exec $DAEMON -- -detach
    ;;
  stop)
    start-stop-daemon --stop --verbose --exec $DAEMON
    ;;
  *)
    echo "Usage: /etc/init.d/crossfire {start|stop|restart}"
    exit 1
    ;;
esac

exit 0
