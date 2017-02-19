set ns [new Simulator]

set tf [open p2.tr w]
$ns trace-all $tf

set nf [open p2.nam w]
$ns namtrace-all $nf

proc finish {} {
	global ns nf tf
	$ns flush-trace
	close $nf
	close $tf
	exec nam p2.nam &
	exit 0
}

set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]

$ns duplex-link $n2 $n0 1Mb 10ms DropTail
$ns duplex-link $n2 $n1 1Mb 10ms DropTail
$ns duplex-link $n2 $n3 1Mb 10ms DropTail

$ns queue-limit $n2 $n3 20
$ns duplex-link-op $n2 $n3 queuePos 0.5

set tcp [new Agent/TCP]
$ns attach-agent $n0 $tcp

set sink [new Agent/TCPSink]
$ns attach-agent $n3 $sink

$ns connect $tcp $sink

set ftp [new Application/FTP]
$ftp attach-agent $tcp 

set udp [new Agent/UDP]
$ns attach-agent $n1 $udp

set null [new Agent/Null]
$ns attach-agent $n3 $null

$ns connect $udp $null

set cbr [new Application/Traffic/CBR]
$cbr attach-agent $udp
$cbr set packetSize_ 500
$cbr set interval_ 0.05

$ns at 0.0 "$cbr start"
$ns at 0.2 "$ftp start"
$ns at 2.0 "$ftp stop"
$ns at 2.2 "$cbr stop"
$ns at 2.3 "finish"


$ns run