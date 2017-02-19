set ns [new Simulator]

set tf [open p3.tr w]
$ns trace-all $tf

set nf [open p3.nam w]
$ns namtrace-all $nf

proc finish {} {
	global ns nf tf
	$ns flush-trace
	close $tf
	close $nf
	exec nam p3.nam &
	exit 0
}

for {set i 0} {$i < 7 } {incr i} {
	set n($i) [$ns node]
}

for {set i 1} {$i < 7} {incr i} {
	$ns duplex-link $n(0) $n($i) 1Mb 10ms DropTail
}

Agent/Ping instproc recv {from rtt} {
	$self instvar node_
	puts "Node [$node_ id] receved a ping from $from with RTT of $rtt"
}

for {set i 1} {$i < 7 } {incr i} {
	set p($i) [new Agent/Ping]
}

for {set i 1} {$i < 7 } {incr i} {
	$ns attach-agent $n($i) $p($i)
}

$ns connect $p(1) $p(4)
$ns connect $p(1) $p(5)
$ns connect $p(1) $p(6)
$ns connect $p(2) $p(4)
$ns connect $p(2) $p(5)
$ns connect $p(2) $p(6)


for {set i 0} {$i<50000} {incr i} {
$ns at 0.2 "$p(1) send"
}

for {set i 0} {$i<50000} {incr i} {
$ns at 0.4 "$p(2) send"
}

for {set i 0} {$i<50000} {incr i} {
$ns at 0.6 "$p(3) send"
}

$ns at 1.0 "$p(4) send"
$ns at 1.2 "$p(5) send"
$ns at 1.4 "$p(6) send"
$ns at 2.0 "finish"
$ns run
