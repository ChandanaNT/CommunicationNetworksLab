set ns [new Simulator]

set tf [open p1.tr w]
$ns trace-all $tf

set nf [open p1.nam w]
$ns namtrace-all $nf

proc finish {} {
	global ns tf nf
	$ns flush-trace
	close $nf
	close $tf
	exec nam p1.nam &
	exit 0
}

set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]

$n0 label Node_1
$n1 label Node_2
$n2 label Node_3

$ns duplex-link $n0 $n1 1Mb 100ms DropTail
$ns duplex-link $n1 $n2 1Mb 10ms DropTail

$ns queue-limit $n1 $n2 20

set udp [new Agent/UDP]
$ns attach-agent $n0 $udp

set null [new  Agent/Null]
$ns attach-agent $n2 $null

$ns connect $udp $null

set cbr [new Application/Traffic/CBR]


$cbr set packetSize_ 500
$cbr set rate_ 2Mb

#$cbr set interval_ 0.000238418

$cbr attach-agent $udp

$ns at 0.0 "$cbr start"
$ns at 2.0 "$cbr stop"
$ns at 2.0 "finish"

$ns run