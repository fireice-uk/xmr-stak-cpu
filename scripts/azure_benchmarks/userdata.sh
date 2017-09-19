#!/bin/bash -x

echo "*** Install necessary dependencies"
apt update -qq
apt install -y -qq cloud-utils git hwloc jq libmicrohttpd-dev libssl-dev cmake build-essential libhwloc-dev

echo "*** Clone git repository and switch to the chosen tag"
git clone https://github.com/fireice-uk/xmr-stak-cpu.git
git --git-dir=xmr-stak-cpu/.git --work-tree=xmr-stak-cpu checkout tags/${git_tag}

echo "*** Compile xmr-stak-cpu"
cd xmr-stak-cpu
cmake -DMICROHTTPD_ENABLE=ON -DHWLOC_ENABLE=ON .
make install

echo "*** Generate xmr-stak-cpu temporary configuration"
cat > myconfig.txt << EOF2
"cpu_threads_conf" : "null",
"use_slow_memory" : "warn",
"nicehash_nonce" : false,
"aes_override" : null,
"use_tls" : ${use_tls},
"tls_secure_algo" : true,
"tls_fingerprint" : "",
"pool_address" : "${pool_address}",
"wallet_address" : "${wallet_address}",
"pool_password" : "${pool_password}",
"call_timeout" : 10,
"retry_time" : 10,
"giveup_limit" : 0,
"verbose_level" : 3,
"h_print_time" : 60,
"daemon_mode" : true,
"output_file" : "/tmp/xmr_stak_cpu.log",
"httpd_port" : 1600,
"prefer_ipv4" : true,
EOF2

echo "*** Set Huge Pages"
sysctl -w vm.nr_hugepages=128

echo "*** Generate config for cpu_threads_conf and add it to the myconfig.txt"
./bin/xmr-stak-cpu ./myconfig.txt | sed -n -e '/BEGIN/,/END/{ /BEGIN/d; /END/d; p; }' > /tmp/details.txt
sed -i '/^"cpu_threads_conf" : "null",/d' ./myconfig.txt
cat /tmp/details.txt >> ./myconfig.txt

echo "*** Add CPU details"
lscpu >> /tmp/details.txt
lstopo --of ascii --no-legend >> /tmp/details.txt

echo "*** Run xmr-stak-cpu"
( sleep 60 ; ./bin/xmr-stak-cpu ./myconfig.txt ) &
