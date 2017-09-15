# XMR-Stak-CPU - AWS Benchmarks

This directory contains simple way how to benchmark xmr-stak-cpu using AWS.

The idea is to create Ubuntu 16.04 instances of different types and let xmr-stack-cpu running there for 20 minutes. Then the Hashrate/second is collected and saved in [aws_benchmark.log](results/aws_benchmark.log) file together with details about CPU.


## Requirements
* AWS access credentials (access keys / secret access key)
* SSH key (private: ```~/.ssh/id_rsa``` public: ```~/.ssh/id_rsa.pub```)
* [Terraform](https://www.terraform.io/)
* [jq](https://stedolan.github.io/jq/)


## Benchmark description
* [run.sh](run.sh) - main executable script where you should define variables like (vpc_id, subnet_id, pool, wallet, ...)
* [variables.tf](variables.tf) - Terraform varibales neede for creating the instances - see aws_instances where you can define what aws instance types you want to deploy
* [userdata.sh](userdata.sh) - this script is execited on freshly provisioned instance

Terraform creates new instances form latest Ubuntu 16.04 ami. The number/type of instances can be configured in ```variables.tf``` (aws_instances).

Once the instances are created the ```userdata.sh``` script is executed. It's doing the following:

* Installing the necessary packages for building xmr-stak-cpu.
* Getting the ```1.5.0``` version of xmr-stak-cpu from [github](https://github.com/fireice-uk/xmr-stak-cpu) and build the binary.
* Enable Huge Pages and generate configuration of ```cpu_threads_conf``` for xmr-stak-cpu config file.
* Get details about CPU (```lscpu```, ```lstopo```) and run xmr-stak-cpu.

The script [run.sh](run.sh) gets the details from provisioned instances and save them to [results/aws_benchmark.log](results/aws_benchmark.log).


## Example

Execution example:

```
$ ./run.sh
data.template_file.user_data_file: Refreshing state...
data.aws_ami.ubuntu: Refreshing state...
aws_key_pair.ssh_key_pair: Creating...
  fingerprint: "" => "<computed>"
  key_name:    "" => "my-tests-ssh_key_pair"
  public_key:  "" => "ssh-rsa ......."
aws_instance.app_server: Creating...
  ami:                                       "" => "ami-xxxxxx"
  associate_public_ip_address:               "" => "<computed>"
  availability_zone:                         "" => "<computed>"
  ebs_block_device.#:                        "" => "<computed>"
  ephemeral_block_device.#:                  "" => "<computed>"
  instance_initiated_shutdown_behavior:      "" => "terminate"
  instance_state:                            "" => "<computed>"
  instance_type:                             "" => "t2.nano"
  ipv6_address_count:                        "" => "<computed>"
  ipv6_addresses.#:                          "" => "<computed>"
  key_name:                                  "" => "my-tests-ssh_key_pair"
  network_interface.#:                       "" => "<computed>"
  network_interface_id:                      "" => "<computed>"
  placement_group:                           "" => "<computed>"
  primary_network_interface_id:              "" => "<computed>"
  private_dns:                               "" => "<computed>"
  private_ip:                                "" => "<computed>"
  public_dns:                                "" => "<computed>"
  public_ip:                                 "" => "<computed>"
  root_block_device.#:                       "" => "1"
  root_block_device.0.delete_on_termination: "" => "true"
  root_block_device.0.iops:                  "" => "<computed>"
  root_block_device.0.volume_size:           "" => "<computed>"
  root_block_device.0.volume_type:           "" => "standard"
  security_groups.#:                         "" => "<computed>"                                                                                                                                                              
  source_dest_check:                         "" => "true"                                                                                                                                                                    
  subnet_id:                                 "" => "subnet-xxxxxx"                                                                                                                                                         
  tags.%:                                    "" => "2"                                                                                                                                                                       
  tags.Application:                          "" => "my-tests-1"                                                                                                                                                        
  tags.Name:                                 "" => "my-tests-app-1"                                                                                                                                                    
  tenancy:                                   "" => "<computed>"                                                                                                                                                              
  user_data:                                 "" => "xxxxxx"
  volume_tags.%:                             "" => "2"
  volume_tags.Application:                   "" => "my-tests-1"
  volume_tags.Name:                          "" => "my-tests-app-1"
  vpc_security_group_ids.#:                  "" => "<computed>"
aws_key_pair.ssh_key_pair: Creation complete (ID: my-tests-ssh_key_pair)
aws_instance.app_server: Still creating... (10s elapsed)
aws_instance.app_server: Still creating... (20s elapsed)
aws_instance.app_server: Creation complete (ID: i-xxxxxx)

Apply complete! Resources: 2 added, 0 changed, 0 destroyed.

The state of your infrastructure has been saved to the path
below. This state is required to modify and destroy your
infrastructure, so keep it safe. To inspect the complete state
use the `terraform show` command.

State path:

Outputs:

AMI IDs = [
    ami-xxxxxx
]
IP Addresses = [
    xx.xx.xx.xx
]
Instance types = [
    t2.nano
]
Sleep for 25 minutes...
*** Instance type: t2.nano | HashRate: 82.1 H/s

"cpu_threads_conf" :
[
    { "low_power_mode" : true, "no_prefetch" : true, "affine_to_cpu" : 0 },
],

Architecture:          x86_64
CPU op-mode(s):        32-bit, 64-bit
Byte Order:            Little Endian
CPU(s):                1
On-line CPU(s) list:   0
Thread(s) per core:    1
Core(s) per socket:    1
Socket(s):             1
NUMA node(s):          1
Vendor ID:             GenuineIntel
CPU family:            6
Model:                 63
Model name:            Intel(R) Xeon(R) CPU E5-2676 v3 @ 2.40GHz
Stepping:              2
CPU MHz:               2400.036
BogoMIPS:              4800.07
Hypervisor vendor:     Xen
Virtualization type:   full
L1d cache:             32K
L1i cache:             32K
L2 cache:              256K
L3 cache:              30720K
NUMA node0 CPU(s):     0
Flags:                 fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ht syscall nx rdtscp lm constant_tsc rep_good nopl xtopology eagerfpu pni pclmulqdq ssse3 fma cx16 pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand hypervisor lahf_lm abm fsgsbase bmi1 avx2 smep bmi2 erms invpcid xsaveopt
/-------------------------------------------------\
| Machine (487MB)                                 |
|                                                 |
| /----------------\            /---------------\ |
| | Package P#0    |  ++--+-----+ PCI 8086:7010 | |
| |                |      |     \---------------/ |
| | /------------\ |      |                       |
| | | L3 (30MB)  | |      |     /---------------\ |
| | \------------/ |      +-----+ PCI 1013:00b8 | |
| |                |            \---------------/ |
| | /------------\ |                              |
| | | L2 (256KB) | |                              |
| | \------------/ |                              |
| |                |                              |
| | /------------\ |                              |
| | | L1d (32KB) | |                              |
| | \------------/ |                              |
| |                |                              |
| | /------------\ |                              |
| | | L1i (32KB) | |                              |
| | \------------/ |                              |
| |                |                              |
| | /------------\ |                              |
| | | Core P#0   | |                              |
| | |            | |                              |
| | | /--------\ | |                              |
| | | | PU P#0 | | |                              |
| | | \--------/ | |                              |
| | \------------/ |                              |
| \----------------/                              |
\-------------------------------------------------/


Do you really want to destroy?
  Terraform will delete all your managed infrastructure.
  There is no undo. Only 'yes' will be accepted to confirm.

  Enter a value:
data.template_file.user_data_file: Refreshing state...
aws_key_pair.ssh_key_pair: Refreshing state... (ID: my-tests-ssh_key_pair)
data.aws_ami.ubuntu: Refreshing state...
aws_instance.app_server: Refreshing state... (ID: i-xxxxxx)
aws_instance.app_server: Destroying... (ID: i-xxxxxx)
aws_key_pair.ssh_key_pair: Destroying... (ID: my-tests-ssh_key_pair)
aws_key_pair.ssh_key_pair: Destruction complete
aws_instance.app_server: Still destroying... (ID: i-xxxxxx, 10s elapsed)
aws_instance.app_server: Still destroying... (ID: i-xxxxxx, 20s elapsed)
aws_instance.app_server: Still destroying... (ID: i-xxxxxx, 30s elapsed)
aws_instance.app_server: Still destroying... (ID: i-xxxxxx, 40s elapsed)
aws_instance.app_server: Destruction complete
data.template_file.user_data_file: Destroying... (ID: xxxxxx)
data.template_file.user_data_file: Destruction complete
data.aws_ami.ubuntu: Destroying... (ID: ami-xxxxxx)
data.aws_ami.ubuntu: Destruction complete

Destroy complete! Resources: 4 destroyed.
```


## Sample results

```
$ grep '\*\*\*' aws_benchmark.log
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** t2.nano              | 125380     | 1       | 2760       | 44       44       | 84.4        | 8         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** t2.micro             | 89740      | 1       | 2130       | 35       35       | 83.2        | 8         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** t2.small             | 115150     | 1       | 2580       | 46       46       | 83          | 8         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** t2.medium            | 218030     | 2       | 4830       | 57       57       | 161.4       | 7         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** t2.large             | 291100     | 2       | 6240       | 50       50       | 167.8       | 8         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** t2.xlarge            | 373422     | 4       | 8220       | 45       45       | 335.6       | 8         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** t2.2xlarge           | 474650     | 8       | 10740      | 43       43       | 420.7       | 8         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** c4.large             | 206760     | 2       | 4530       | 48       48       | 157         | 7         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** c4.xlarge            | 252830     | 4       | 5460       | 49       49       | 175.2       | 8         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** c4.2xlarge           | 581550     | 8       | 11940      | 49       49       | 441         | 7         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** c4.4xlarge           | 152230     | 13      | 3210       | 59       59       | 92.8        | 7         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** c4.8xlarge           | 1615500    | 26      | 33060      | 72       72       | 1040.1      | 8         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** m4.large             | 245610     | 2       | 5070       | 59       59       | 143.3       | 7         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** m4.4xlarge           | 887410     | 16      | 19920      | 58       58       | 688.1       | 7         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** m4.10xlarge          | 90060      | 30      | 2000       | 45       45       | 67          | 8         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** m4.16xlarge          | 1778530    | 46      | 40590      | 79       79       | 1437.8      | 8         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** r4.large             | 200540     | 2       | 4170       | 59       59       | 144         | 8         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** r4.xlarge            | 435286     | 4       | 8940       | 65       65       | 277.8       | 7         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** r4.2xlarge           | 327410     | 8       | 7170       | 44       44       | 270         | 7         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** r4.4xlarge           | 832230     | 16      | 18060      | 65       65       | 623.7       | 7         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** r3.8xlarge           | 1059540    | 26      | 22320      | 71       71       | 991.9       | 7         | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** r4.16xlarge          | 102650     | 46      | 2100       | 49       49       | 57.5        | 7         | pool.supportxmr.com:443      |
```
