#!/usr/bin/perl
$crossname = "/madhatter/bin/crossfire";
$playerdir = "/madhatter/lib/X11/crossfire/players";
$logcount = 0;
$logdir= "/tmp/crosslog";

if (! -d $logdir) {
    mkdir($logdir,0775) || die("Unable to mkdir $logdir\n");
}
system("rm $logdir/*");

$badcount = 0;
while(1) {
    ++$logcount;
    $starttime = time;
    &doserver;
    system("rm /tmp/*$crosspid.o?");
    system("rmdir $playerdir/*.lock");
    $badcount = 0 if ((time-$startime)<300);
    ++$badcount;
    die("Crossfire really unhappy\n")
	if $badcount>10;
}

sub doserver {
    open(LOGFILE,">$logdir/$logcount")
	|| die("Unable to open $logdir/$logcount for write\n");
    $oldfh = select(LOGFILE);$|=1;select(LOGFILE);
    pipe(READ,WRITE);
    if (($crosspid=fork)>0) {
	# Parent
	close(WRITE);
	$now = time;
	$toomuch =0;
	while(<READ>) {
	   if ((time-$now)<5) {
	       ++$toomuch;
	       if ($toomuch>500) {
		   print LOGFILE "Crossfire Talks too much\n";
		   kill 'HUP', $crosspid;
		   sleep(5);
		   kill 'TERM', $crosspid;
		   sleep(5);
		   kill 'INT', $crosspid;
		   sleep(5);
		   kill 9, $crosspid;
		   last;
	       }
	   } else {
	       $toomuch = 0;
	   }
	   print LOGFILE $_;
       }
    } elsif ($crosspid==0) {
	close(READ);
	open(STDOUT,">&WRITE") || die("Can't redirect stdout\n");
	open(STDERR,">&WRITE") || die("Can't redirect stderr\n");
	exec "$crossname","-server";
	die("Exec Failed\n");
    } else {
	die("can't fork\n");
    }
    close(LOGFILE);
}
