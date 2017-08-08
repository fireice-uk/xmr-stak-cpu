#!/bin/bash -eu

AWS_REGION="us-east-1"
AWS_VPC_ID="vpc-xxxxxxxx"
AWS_SUBNET_ID="subnet-xxxxxxxx"
PREFIX="my-tests"

#Address of the pool to commect with the miner
POOL_ADDRESS="mine.xmrpool.net:80"

#Wallet address used by miner
WALLET_ADDRESS="my_wallet_id"

#Password for the pool
POOL_PASSWORD="\$HOSTNAME-\`ec2metadata --instance-type\`"

#Use TLS to connect to the pool
USE_TLS="false"

#xmr-stak-cpu git release tag
GIT_TAG="v1.3.0-1.5.0"

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

echo "Sleep for 25 minutes..."
sleep 1500

(
INSTANCES=`terraform output -json | jq -r '[."IP Addresses".value, ."Instance types".value] | transpose [] | @csv'`
for INSTANCE in $INSTANCES; do
  INSTANCE_IP=`echo $INSTANCE | awk -F \" '{ print $2 }'`
  INSTANCE_TYPE=`echo $INSTANCE | awk -F \" '{ print $4 }'`
  echo -n "*** Instance type: $INSTANCE_TYPE | "
  #Use Hashrate for passed 15 minutes (last field)
  HASHRATE=`curl -s http://${INSTANCE_IP}:1600/api.json | jq -r '.hashrate.total[-1]'`
  echo "HashRate: $HASHRATE H/s"
  ssh -xq ${INSTANCE_IP} -l ubuntu "cat /tmp/details.txt"
  echo -e "\n"
done
) | tee results/aws_benchmark.log

echo "yes" | terraform destroy
