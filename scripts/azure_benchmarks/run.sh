#!/bin/bash -ue

RESOURCE_GROUP="my_resource_group"
PREFIX="mytest"
LOCATION="eastus2"

#Address of the pool to commect with the miner
POOL_ADDRESS="pool.supportxmr.com:443"

#Wallet address used by miner
WALLET_ADDRESS="4....d"

#Password for the pool
POOL_PASSWORD="\`curl -s -H Metadata:true 'http://169.254.169.254/metadata/instance?api-version=2017-08-01' | jq -r '.compute.vmSize'\`:myemail@gmail.com"

#Use TLS to connect to the pool
USE_TLS="true"

#xmr-stak-cpu git release tag
GIT_TAG="v1.3.0-1.5.0"

#Temporary file
TMP_FILE="`mktemp --suffix=_stats_xmr-stak-cpu`"

TERRAFORM_CMD_PARAMETERS='
-var "resource_group=$RESOURCE_GROUP"
-var "prefix=$PREFIX"
-var "location=$LOCATION"
-var "pool_address=$POOL_ADDRESS"
-var "wallet_address=$WALLET_ADDRESS"
-var "pool_password=$POOL_PASSWORD"
-var "use_tls=$USE_TLS"
-var "git_tag=$GIT_TAG"
'

eval terraform apply $TERRAFORM_CMD_PARAMETERS

date
echo "Sleep for 25 minutes..."
sleep 1500

eval terraform refresh $TERRAFORM_CMD_PARAMETERS

(
VMS=`terraform output -json | jq -r '[."IP Addresses".value, ."VM Size".value] | transpose [] | @csv'`

for VM in $VMS; do
  VM_IP=`echo $VM | awk -F \" '{ print $2 }'`
  VM_SIZE=`echo $VM | awk -F \" '{ print $4 }'`

  ssh -xq ${VM_IP} -l ubuntu "curl -s http://127.0.0.1:1600/api.json" > $TMP_FILE
  THREADS=`jq '.hashrate.threads | length' $TMP_FILE`
  HASHES_TOTAL=`jq '.results.hashes_total' $TMP_FILE`
  DIFFICULTY=`jq '.results.diff_current' $TMP_FILE`
  SHARES_GOOD=`jq '.results.shares_good' $TMP_FILE`
  SHARES_TOTAL=`jq '.results.shares_total' $TMP_FILE`
  HASHRATE_TOTAL=`jq '.hashrate.total[-1]' $TMP_FILE`
  POOL=`jq -r '.connection.pool' $TMP_FILE`
  POOL_PING_TIME=`jq '.connection.ping' $TMP_FILE`

  echo "*********************************************************************************************************************************************"
  printf "| *** %-20s | %-10s | %-7s | %-10s | %-17s | %-11s | %-9s | %-28s |\n" "Type" "Hashes" "Threads" "Difficulty" "Shares Good/Total" "Total (H/s)" "Ping (ms)" "Pool"
  printf "| *** %-20s | %-10s | %-7s | %-10s | %-8s %-8s | %-11s | %-9s | %-28s |\n" "$VM_SIZE" "$HASHES_TOTAL" "$THREADS" "$DIFFICULTY" "$SHARES_GOOD" "$SHARES_TOTAL" "$HASHRATE_TOTAL" "$POOL_PING_TIME" "$POOL"

  rm $TMP_FILE

  ssh -xq ${VM_IP} -l ubuntu "cat /tmp/details.txt"
  echo -e "\n"
done
) | tee results/azure_benchmark.log

eval terraform destroy $TERRAFORM_CMD_PARAMETERS -force
