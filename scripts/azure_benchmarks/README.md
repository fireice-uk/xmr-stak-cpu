# XMR-Stak-CPU - Azure Benchmarks

This directory contains simple way how to benchmark xmr-stak-cpu using Azure.

The idea is to create Ubuntu 16.04 instances of different types and let xmr-stack-cpu running there for 20 minutes. Then the Hashrate/second is collected and saved in [azure_benchmark.log](results/azure_benchmark.log) file together with details about CPU.


## Requirements
* Azure access
* SSH key (private: ```~/.ssh/id_rsa``` public: ```~/.ssh/id_rsa.pub```)
* [Terraform](https://www.terraform.io/)
* [jq](https://stedolan.github.io/jq/)


## Benchmark description
* [run.sh](run.sh) - main executable script where you should define variables like (resource group, location, pool, wallet, ...)
* [variables.tf](variables.tf) - Terraform varibales neede for creating the vms - see vm_sizes where you can define what vm types you want to deploy
* [userdata.sh](userdata.sh) - this script is execited on freshly provisioned instance

Terraform creates new vms form latest Ubuntu 16.04. The number/type of vms can be configured in ```variables.tf``` (vm_sizes).

Once the instances are created the ```userdata.sh``` script is executed. It's doing the following:

* Installing the necessary packages for building xmr-stak-cpu.
* Getting the ```1.5.0``` version of xmr-stak-cpu from [github](https://github.com/fireice-uk/xmr-stak-cpu) and build the binary.
* Enable Huge Pages and generate configuration of ```cpu_threads_conf``` for xmr-stak-cpu config file.
* Get details about CPU (```lscpu```, ```lstopo```) and run xmr-stak-cpu.

The script [run.sh](run.sh) gets the details from provisioned instances and save them to [results/azure_benchmark.log](results/azure_benchmark.log).


## Example

Execution example:

```
$ ./run.sh
data.template_file.user_data_file: Refreshing state...
azurerm_public_ip.pip: Creating...
  domain_name_label:            "" => "mytest-app-1"
  fqdn:                         "" => "<computed>"
  ip_address:                   "" => "<computed>"
  location:                     "" => "eastus2"
  name:                         "" => "mytest-app-1-ip"
  public_ip_address_allocation: "" => "dynamic"
  resource_group_name:          "" => "my_resource_group"
  tags.%:                       "" => "<computed>"
azurerm_virtual_network.vnet: Creating...
  address_space.#:     "" => "1"
  address_space.0:     "" => "10.0.0.0/16"
  location:            "" => "eastus2"
  name:                "" => "mytest-vnet"
  resource_group_name: "" => "my_resource_group"
  subnet.#:            "" => "<computed>"
  tags.%:              "" => "<computed>"
azurerm_virtual_network.vnet: Creation complete after 10s (ID: /subscriptions/xxxxxxxx-xxxx....Network/virtualNetworks/mytest-vnet)
azurerm_subnet.subnet: Creating...
  address_prefix:            "" => "10.0.10.0/24"
  ip_configurations.#:       "" => "<computed>"
  name:                      "" => "mytest-subnet"
  network_security_group_id: "" => "<computed>"
  resource_group_name:       "" => "my_resource_group"
  route_table_id:            "" => "<computed>"
  virtual_network_name:      "" => "mytest-vnet"
azurerm_public_ip.pip: Still creating... (10s elapsed)
azurerm_public_ip.pip: Creation complete after 11s (ID: /subscriptions/xxxxxxxx-xxxx...rk/publicIPAddresses/mytest-app-1-ip)
azurerm_subnet.subnet: Creation complete after 6s (ID: /subscriptions/xxxxxxxx-xxxx.../mytest-vnet/subnets/mytest-subnet)
azurerm_network_interface.nic: Creating...
  applied_dns_servers.#:                                        "" => "<computed>"
  dns_servers.#:                                                "" => "<computed>"
  enable_ip_forwarding:                                         "" => "false"
  internal_dns_name_label:                                      "" => "<computed>"
  internal_fqdn:                                                "" => "<computed>"
  ip_configuration.#:                                           "" => "1"
  ip_configuration.0.load_balancer_backend_address_pools_ids.#: "" => "<computed>"
  ip_configuration.0.load_balancer_inbound_nat_rules_ids.#:     "" => "<computed>"
  ip_configuration.0.name:                                      "" => "mytest-app-1-ipconfig"
  ip_configuration.0.primary:                                   "" => "<computed>"
  ip_configuration.0.private_ip_address:                        "" => "<computed>"
  ip_configuration.0.private_ip_address_allocation:             "" => "dynamic"
  ip_configuration.0.public_ip_address_id:                      "" => "/subscriptions/xxxxxxxx-xxxx/resourceGroups/my_resource_group/providers/Microsoft.Network/publicIPAddresses/mytest-app-1-ip"
  ip_configuration.0.subnet_id:                                 "" => "/subscriptions/xxxxxxxx-xxxx/resourceGroups/my_resource_group/providers/Microsoft.Network/virtualNetworks/mytest-vnet/subnets/mytest-subnet"
  location:                                                     "" => "eastus2"
  mac_address:                                                  "" => "<computed>"
  name:                                                         "" => "mytest-app-1-nic"
  private_ip_address:                                           "" => "<computed>"
  resource_group_name:                                          "" => "my_resource_group"
  tags.%:                                                       "" => "<computed>"
  virtual_machine_id:                                           "" => "<computed>"
azurerm_network_interface.nic: Creation complete after 3s (ID: /subscriptions/xxxxxxxx-xxxx...k/networkInterfaces/mytest-app-1-nic)
azurerm_virtual_machine.vm: Creating...
  availability_set_id:                                              "" => "<computed>"
  delete_data_disks_on_termination:                                 "" => "true"
  delete_os_disk_on_termination:                                    "" => "true"
  location:                                                         "" => "eastus2"
  name:                                                             "" => "mytest-app-1-vm"
  network_interface_ids.#:                                          "" => "1"
  network_interface_ids.512677734:                                  "" => "/subscriptions/xxxxxxxx-xxxx/resourceGroups/my_resource_group/providers/Microsoft.Network/networkInterfaces/mytest-app-1-nic"
  os_profile.#:                                                     "" => "1"
  os_profile.1719283786.admin_password:                             "<sensitive>" => "<sensitive>"
  os_profile.1719283786.admin_username:                             "" => "ubuntu"
  os_profile.1719283786.computer_name:                              "" => "mytest-app-1"
  os_profile.1719283786.custom_data:                                "" => "axxxxf"
  os_profile_linux_config.#:                                        "" => "1"
  os_profile_linux_config.69840937.disable_password_authentication: "" => "true"
  os_profile_linux_config.69840937.ssh_keys.#:                      "" => "1"
  os_profile_linux_config.69840937.ssh_keys.0.key_data:             "" => "ssh-rsa AxxxxX\n"
  os_profile_linux_config.69840937.ssh_keys.0.path:                 "" => "/home/ubuntu/.ssh/authorized_keys"
  resource_group_name:                                              "" => "my_resource_group"
  storage_image_reference.#:                                        "" => "1"
  storage_image_reference.1458860473.id:                            "" => ""
  storage_image_reference.1458860473.offer:                         "" => "UbuntuServer"
  storage_image_reference.1458860473.publisher:                     "" => "Canonical"
  storage_image_reference.1458860473.sku:                           "" => "16.04-LTS"
  storage_image_reference.1458860473.version:                       "" => "latest"
  storage_os_disk.#:                                                "" => "1"
  storage_os_disk.934613875.caching:                                "" => "ReadWrite"
  storage_os_disk.934613875.create_option:                          "" => "FromImage"
  storage_os_disk.934613875.disk_size_gb:                           "" => ""
  storage_os_disk.934613875.image_uri:                              "" => ""
  storage_os_disk.934613875.managed_disk_id:                        "" => "<computed>"
  storage_os_disk.934613875.managed_disk_type:                      "" => "Standard_LRS"
  storage_os_disk.934613875.name:                                   "" => "mytest-app-1-osdisk"
  storage_os_disk.934613875.os_type:                                "" => ""
  storage_os_disk.934613875.vhd_uri:                                "" => ""
  tags.%:                                                           "" => "2"
  tags.Environment:                                                 "" => "Development"
  tags.Hostname:                                                    "" => "mytest-app-1"
  vm_size:                                                          "" => "Standard_A0"
azurerm_virtual_machine.vm: Still creating... (10s elapsed)
azurerm_virtual_machine.vm: Still creating... (20s elapsed)
azurerm_virtual_machine.vm: Still creating... (30s elapsed)
azurerm_virtual_machine.vm: Still creating... (40s elapsed)
azurerm_virtual_machine.vm: Still creating... (50s elapsed)
azurerm_virtual_machine.vm: Still creating... (1m0s elapsed)
azurerm_virtual_machine.vm: Still creating... (1m10s elapsed)
azurerm_virtual_machine.vm: Still creating... (1m20s elapsed)
azurerm_virtual_machine.vm: Still creating... (1m30s elapsed)
azurerm_virtual_machine.vm: Still creating... (1m40s elapsed)
azurerm_virtual_machine.vm: Still creating... (1m50s elapsed)
azurerm_virtual_machine.vm: Still creating... (2m0s elapsed)
azurerm_virtual_machine.vm: Creation complete after 2m6s (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm)

Apply complete! Resources: 5 added, 0 changed, 0 destroyed.

Outputs:

FQDN = [
    mytest-app-1.eastus2.cloudapp.azure.com
]
Hostname = [
    mytest-app-1
]
VM Size = [
    Standard_A0
]
Tue Sep 19 14:50:05 CEST 2017
Sleep for 25 minutes...
data.template_file.user_data_file: Refreshing state...
azurerm_public_ip.pip: Refreshing state... (ID: /subscriptions/xxxxxxxx-xxxx...rk/publicIPAddresses/mytest-app-1-ip)
azurerm_virtual_network.vnet: Refreshing state... (ID: /subscriptions/xxxxxxxx-xxxx....Network/virtualNetworks/mytest-vnet)
azurerm_subnet.subnet: Refreshing state... (ID: /subscriptions/xxxxxxxx-xxxx.../mytest-vnet/subnets/mytest-subnet)
azurerm_network_interface.nic: Refreshing state... (ID: /subscriptions/xxxxxxxx-xxxx...k/networkInterfaces/mytest-app-1-nic)
azurerm_virtual_machine.vm: Refreshing state... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm)

Outputs:

FQDN = [
    mytest-app-1.eastus2.cloudapp.azure.com
]
Hostname = [
    mytest-app-1
]
IP Addresses = [
    52.xx.xx.75
]
SSH Command = [
    ssh -i ~/.ssh/id_rsa.pub ubuntu@52.xx.xx.75
]
VM Size = [
    Standard_A0
]
********************************************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A0          | 20000      | 1       | 2000       | 10       10       | 19.1        | 39        | pool.supportxmr.com:443      |

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
Model:                 45
Model name:            Intel(R) Xeon(R) CPU E5-2660 0 @ 2.20GHz
Stepping:              7
CPU MHz:               2194.683
BogoMIPS:              4389.36
Hypervisor vendor:     Microsoft
Virtualization type:   full
L1d cache:             32K
L1i cache:             32K
L2 cache:              256K
L3 cache:              20480K
NUMA node0 CPU(s):     0
Flags:                 fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ss syscall nx lm constant_tsc rep_good nopl eagerfpu pni pclmulqdq ssse3 cx16 sse4_1 sse4_2 popcnt aes xsave avx hypervisor lahf_lm xsaveopt
/-------------------------------------------------\
| Machine (667MB)                                 |
|                                                 |
| /----------------\            /---------------\ |
| | Package P#0    |  ++--+-----+ PCI 8086:7111 | |
| |                |      |     |               | |
| | /------------\ |      |     | /-----\       | |
| | | L3 (20MB)  | |      |     | | sr0 |       | |
| | \------------/ |      |     | \-----/       | |
| |                |      |     \---------------/ |
| | /------------\ |      |                       |
| | | L2 (256KB) | |      |     /---------------\ |
| | \------------/ |      +-----+ PCI 1414:5353 | |
| |                |            \---------------/ |
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


data.template_file.user_data_file: Refreshing state...
azurerm_public_ip.pip: Refreshing state... (ID: /subscriptions/xxxxxxxx-xxxx...rk/publicIPAddresses/mytest-app-1-ip)
azurerm_virtual_network.vnet: Refreshing state... (ID: /subscriptions/xxxxxxxx-xxxx....Network/virtualNetworks/mytest-vnet)
azurerm_subnet.subnet: Refreshing state... (ID: /subscriptions/xxxxxxxx-xxxx.../mytest-vnet/subnets/mytest-subnet)
azurerm_network_interface.nic: Refreshing state... (ID: /subscriptions/xxxxxxxx-xxxx...k/networkInterfaces/mytest-app-1-nic)
azurerm_virtual_machine.vm: Refreshing state... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm)
azurerm_virtual_machine.vm: Destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 10s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 20s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 30s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 40s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 50s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 1m0s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 1m10s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 1m20s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 1m30s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 1m40s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 1m50s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 2m0s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 2m10s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 2m20s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 2m30s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 2m40s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 2m50s elapsed)
azurerm_virtual_machine.vm: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...pute/virtualMachines/mytest-app-1-vm, 3m0s elapsed)
azurerm_virtual_machine.vm: Destruction complete after 3m6s
azurerm_network_interface.nic: Destroying... (ID: /subscriptions/xxxxxxxx-xxxx...k/networkInterfaces/mytest-app-1-nic)
azurerm_network_interface.nic: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...k/networkInterfaces/mytest-app-1-nic, 10s elapsed)
azurerm_network_interface.nic: Destruction complete after 13s
azurerm_public_ip.pip: Destroying... (ID: /subscriptions/xxxxxxxx-xxxx...rk/publicIPAddresses/mytest-app-1-ip)
azurerm_subnet.subnet: Destroying... (ID: /subscriptions/xxxxxxxx-xxxx.../mytest-vnet/subnets/mytest-subnet)
azurerm_public_ip.pip: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx...rk/publicIPAddresses/mytest-app-1-ip, 10s elapsed)
azurerm_subnet.subnet: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx.../mytest-vnet/subnets/mytest-subnet, 10s elapsed)
azurerm_public_ip.pip: Destruction complete after 12s
azurerm_subnet.subnet: Destruction complete after 13s
azurerm_virtual_network.vnet: Destroying... (ID: /subscriptions/xxxxxxxx-xxxx....Network/virtualNetworks/mytest-vnet)
azurerm_virtual_network.vnet: Still destroying... (ID: /subscriptions/xxxxxxxx-xxxx....Network/virtualNetworks/mytest-vnet, 10s elapsed)
azurerm_virtual_network.vnet: Destruction complete after 12s

Destroy complete! Resources: 5 destroyed.
```


## Sample results

```
$ grep '\*\*\*' azure_benchmark.log
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A0          | 18000      | 1       | 2000       | 9        9        | 18.6        | 51        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A1          | 52000      | 1       | 2000       | 26       26       | 40.8        | 29        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A2          | 101170     | 2       | 2100       | 50       50       | 83.3        | 15        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A3          | 170420     | 4       | 3600       | 52       52       | 135.7       | 15        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A4          | 189520     | 8       | 3720       | 55       55       | 147.7       | 13        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A5          | 113070     | 2       | 2400       | 49       49       | 83.4        | 15        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A6          | 351700     | 4       | 7050       | 68       68       | 161.3       | 15        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A7          | 141194     | 8       | 3030       | 52       52       | 100.5       | 14        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A1_v2       | 28000      | 1       | 2000       | 14       14       | 26.4        | 37        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A2_v2       | 101210     | 2       | 2000       | 50       50       | 80.8        | 14        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A4_v2       | 210150     | 4       | 4830       | 51       51       | 153         | 14        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_A8_v2       | 247551     | 8       | 5550       | 52       52       | 149.2       | 14        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_D1_v2       | 106260     | 1       | 2010       | 51       51       | 75          | 13        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_D2_v2       | 250750     | 2       | 4650       | 56       56       | 158.6       | 13        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_D3_v2       | 399550     | 4       | 7410       | 64       64       | 321         | 13        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_D4_v2       | 646000     | 8       | 14190      | 45       45       | 467.1       | 14        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_D5_v2       | 1374170    | 16      | 25410      | 66       66       | 810.9       | 13        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_D15_v2      | 1006490    | 20      | 21990      | 74       74       | 764.5       | 15        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_D2_v3       | 217020     | 2       | 4620       | 56       56       | 134.7       | 12        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_D4_v3       | 363210     | 4       | 7920       | 56       56       | 279.7       | 12        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_D8_v3       | 563210     | 8       | 10800      | 57       57       | 388.2       | 13        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_D16_v3      | 899170     | 16      | 17760      | 56       56       | 550.3       | 13        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_D32_v3      | 768000     | 32      | 14760      | 52       52       | 551.3       | 12        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_D64_v3      | 1877770    | 64      | 32880      | 77       77       | 1001        | 13        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_F4          | 489890     | 4       | 10770      | 60       60       | 323.7       | 12        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_F8          | 544980     | 8       | 11430      | 58       58       | 414         | 14        | pool.supportxmr.com:443      |
*********************************************************************************************************************************************
| *** Type                 | Hashes     | Threads | Difficulty | Shares Good/Total | Total (H/s) | Ping (ms) | Pool                         |
| *** Standard_F16         | 1210460    | 16      | 23220      | 78       78       | 845.5       | 13        | pool.supportxmr.com:443      |
```
