#!/bin/sh

. /lib/functions/smp.sh

default_rps

TX_IDS=$(cat /proc/interrupts | grep edma_eth_tx | sed -r 's/\s*([0-9]+):(\s*[a-zA-Z0-9\-]+){7}\s*edma_eth_tx([0-9]+)/\1,\3/g')

while IFS= read -r line
do
	affinity=1
	irq=$(echo $line | cut -d "," -f 1)
	queue=$(echo $line | cut -d "," -f 2)
	if   [ $(expr $queue / 4) == "0" ]; then
		affinity=4
	elif [ $(expr $queue / 4) == "1" ]; then
		affinity=8
	elif [ $(expr $queue / 4) == "2" ]; then
		affinity=1
	elif [ $(expr $queue / 4) == "3" ]; then
		affinity=2
	fi
	echo $affinity > /proc/irq/$irq/smp_affinity
done <<EOF
$TX_IDS
EOF

RX_IDS=$(cat /proc/interrupts | grep edma_eth_rx | sed -r 's/\s*([0-9]+):(\s*[a-zA-Z0-9\-]+){7}\s*edma_eth_rx([0-9]+)/\1/g')

i=0
while IFS= read -r irq
do
	echo $((1 << $i)) > /proc/irq/$irq/smp_affinity
	i=$(($i + 1))
done <<EOF
$RX_IDS
EOF

echo 1 > /sys/class/net/eth0/queues/tx-0/xps_cpus
echo 2 > /sys/class/net/eth0/queues/tx-1/xps_cpus
echo 4 > /sys/class/net/eth0/queues/tx-2/xps_cpus
echo 8 > /sys/class/net/eth0/queues/tx-3/xps_cpus
echo 1 > /sys/class/net/eth0/queues/rx-0/rps_cpus
echo 2 > /sys/class/net/eth0/queues/rx-1/rps_cpus
echo 4 > /sys/class/net/eth0/queues/rx-2/rps_cpus
echo 8 > /sys/class/net/eth0/queues/rx-3/rps_cpus
echo 1 > /sys/class/net/eth1/queues/tx-0/xps_cpus
echo 2 > /sys/class/net/eth1/queues/tx-1/xps_cpus
echo 4 > /sys/class/net/eth1/queues/tx-2/xps_cpus
echo 8 > /sys/class/net/eth1/queues/tx-3/xps_cpus
echo 1 > /sys/class/net/eth1/queues/rx-0/rps_cpus
echo 2 > /sys/class/net/eth1/queues/rx-1/rps_cpus
echo 4 > /sys/class/net/eth1/queues/rx-2/rps_cpus
echo 8 > /sys/class/net/eth1/queues/rx-3/rps_cpus
