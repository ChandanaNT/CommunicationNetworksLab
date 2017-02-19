set ns [new Simulator]

set tf [open p5.tr w]
$ns trace-all $tf

set nf [open p5.nam w]
$ns namtrace-all $nf

set f1 [open f1 w]
set f2 [open f2 w]

proc finish {} {
	global ns tf nf
	$ns flush-trace
	close $tf
	close $nf
	exec nam p5.nam &
	exec xgraph f1 f2 &
	exit 0
}

proc plotWindow {tcpSource file} {
	global ns
	set time 0.1
	set now [$ns now]
	set cwnd [$tcpSource set cwnd_]
        puts $file "$now $cwnd"
	$ns at [expr $now+$time] "plotWindow $tcpSource $file"
}

for {set i 0} {$i < 6} {incr i} {
	set n($i) [$ns node]
}

$ns duplex-link $n(0) $n(2) 2Mb 10ms DropTail
$ns duplex-link $n(1) $n(2) 2Mb 10ms DropTail
$ns duplex-link $n(2) $n(3) 0.6Mb 100ms DropTail

$ns duplex-link-op $n(0) $n(2) orient right-up
$ns duplex-link-op $n(1) $n(2) orient right-down
$ns duplex-link-op $n(2) $n(3) orient right

$ns queue-limit $n(2) $n(3) 30
$ns duplex-link-op $n(2) $n(3) queuePos 0.5

set loss_module [new ErrorModel]
$loss_module ranvar [new RandomVariable/Uniform]
$loss_module drop-target [new Agent/Null]
$ns lossmodel $loss_module $n(2) $n(3)

set lan [$ns newLan "$n(3) $n(4) $n(5)" 0.5Mb 40ms LL Queue/DropTail MAC/802_3 Channel]

set tcp0 [new Agent/TCP/Newreno]
$ns attach-agent $n(0) $tcp0
$tcp0 set packetSize_ 552
$tcp0 set window_ 8000

set sink0 [new Agent/TCPSink]
$ns attach-agent $n(4) $sink0

$ns connect $tcp0 $sink0

set ftp0 [new Application/FTP]
$ftp0 attach-agent $tcp0

set tcp1 [new Agent/TCP/Newreno]
$ns attach-agent $n(5)  $tcp1
$tcp1 set packetSize_ 552
$tcp1 set window_ 8000

set sink1 [new Agent/TCPSink]
$ns attach-agent $n(1) $sink1

$ns connect $tcp1 $sink1

set ftp1 [new Application/FTP]
$ftp1 attach-agent $tcp1

$ns at 0.1 "$ftp0 start"
$ns at 0.5 "plotWindow $tcp0 $f1"
$ns at 5.0 "$ftp1 start"
$ns at 5.1 "plotWindow $tcp1 $f2"
$ns at 25.0 "$ftp0 stop"
$ns at 25.0 "$ftp1 stop"
$ns at 25.1 "finish"
$ns run 
