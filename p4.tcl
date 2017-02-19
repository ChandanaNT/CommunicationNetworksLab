set ns [new Simulator]

set tf [open p4.tr w]
$ns trace-all $tf

set nf [open p4.nam w]
$ns namtrace-all $nf

proc finish {} {
	global ns tf nf
	$ns flush-trace
	close $tf
	close $nf
	exec nam p4.nam &
	exit 0
}

for {set i 0} {$i < 6} {incr i} {
     set n($i) [$ns node] 
}

$ns duplex-link $n(0) $n(2) 2Mb 10ms DropTail
$ns duplex-link $n(1) $n(2) 2Mb 10ms DropTail
$ns simplex-link $n(2) $n(3) 0.2Mb 100ms DropTail
$ns simplex-link $n(3) $n(2) 0.3Mb 100ms DropTail

$ns queue-limit $n(2) $n(3) 30
$ns duplex-link-op $n(2) $n(3) queuePos 0.5

$ns duplex-link-op $n(0) $n(2) orient right-up
$ns duplex-link-op $n(1) $n(2) orient right-down
$ns duplex-link-op $n(2) $n(3) orient right

set lan [$ns newLan "$n(3) $n(4) $n(5)" 0.5Mb 40ms LL Queue/DropTail MAC/802_3 Channel]

set loss_module [new ErrorModel]
$loss_module ranvar [new RandomVariable/Uniform]
$loss_module drop-target [new Agent/Null]
$ns lossmodel $loss_module $n(2) $n(3)

set tcp [new Agent/TCP/Newreno]
$ns attach-agent $n(0) $tcp
$tcp set packetSize_ 400
$tcp set windowSize_ 8000

set sink [new Agent/TCPSink/DelAck]
$ns attach-agent $n(4) $sink

$ns connect $tcp $sink

set ftp [new Application/FTP]
$ftp attach-agent $tcp

set udp [new Agent/UDP]
$ns attach-agent $n(1) $udp

set null [new Agent/Null]
$ns attach-agent $n(5) $null

$ns connect $udp $null

set cbr [new Application/Traffic/CBR]
$cbr attach-agent $udp
$cbr set packetSize_ 700
$cbr set rate_ 0.1Mb
$cbr set random_ false

$ns at 0.1 "$cbr start"
$ns at 1.0 "$ftp start"
$ns at 123.1 "$cbr stop"
$ns at 124 "$ftp stop"
$ns at 124.1 "finish"
$ns run