#!/bin/bash -eu

AWS_REGION="us-east-1"
AWS_VPC_ID="vpc-xxxxxxxx"
AWS_SUBNET_ID="subnet-xxxxxxxx"
PREFIX="my-tests"

#Address of the pool to commect with the miner
POOL_ADDRESS="pool.supportxmr.com:443"

#Wallet address used by miner
WALLET_ADDRESS="my_wallet_id"

#Password for the pool
POOL_PASSWORD="\`ec2metadata --instance-type\`:myemail@gmail.com"

#Use TLS to connect to the pool
USE_TLS="true"

#xmr-stak-cpu git release tag
GIT_TAG="v1.3.0-1.5.0"

#Temporary file
TMP_FILE="`mktemp --suffix=_stats_xmr-stak-cpu`"

terraform apply \
  -var "aws_region=$AWS_REGION" \
  -var "aws_vpc_id=$AWS_VPC_ID" \
  -var "aws_subnet_id=$AWS_SUBNET_ID" \
  -var "prefix=$PREFIX" \
  -var "pool_address=$POOL_ADDRESS" \
  -var "wallet_address=$WALLET_ADDRESS" \
  -var "pool_password=$POOL_PASSWORD" \
  -var "use_tls=$USE_TLS" \
  -var "git_tag=$GIT_TAG"

date
echo "Sleep for 25 minutes..."
sleep 1500

(
INSTANCES=`terraform output -json | jq -r '[."IP Addresses".value, ."Instance types".value] | transpose [] | @csv'`

for INSTANCE in $INSTANCES; do
  INSTANCE_IP=`echo $INSTANCE | awk -F \" '{ print $2 }'`
  INSTANCE_TYPE=`echo $INSTANCE | awk -F \" '{ print $4 }'`

  ssh -xq ${INSTANCE_IP} -l ubuntu "curl -s http://127.0.0.1:1600/api.json" > $TMP_FILE
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
  printf "| *** %-20s | %-10s | %-7s | %-10s | %-8s %-8s | %-11s | %-9s | %-28s |\n" "$INSTANCE_TYPE" "$HASHES_TOTAL" "$THREADS" "$DIFFICULTY" "$SHARES_GOOD" "$SHARES_TOTAL" "$HASHRATE_TOTAL" "$POOL_PING_TIME" "$POOL"

  rm $TMP_FILE

  ssh -xq ${INSTANCE_IP} -l ubuntu "cat /tmp/details.txt"
  echo -e "\n"
done
) | tee results/aws_benchmark.log

terraform destroy -force