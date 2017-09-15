# AWS region
variable "aws_region" {
  description = "AWS region to launch servers."
  default     = "us-east-1"
}

# VPC ID
variable "aws_vpc_id" {
    description = "VPC ID"
    default = ""
}

# Subnet ID
variable "aws_subnet_id" {
    description = "Subnet ID"
    default = ""
}

#Default prefix
variable "prefix" {
  description = "Prefix for all created items"
  default = ""
}

# Create AWS instances
variable "aws_instances" {
  description = "EC2 Instances which will be created"
  type = "list"
  default = [
    "t2.nano", "t2.micro", "t2.small", "t2.medium", "t2.large", "t2.xlarge", "t2.2xlarge",
    "c4.large", "c4.xlarge", "c4.2xlarge", "c4.4xlarge", "c4.8xlarge",
    "m4.large", "m4.4xlarge", "m4.10xlarge", "m4.16xlarge",
    "r4.large", "r4.xlarge", "r4.2xlarge", "r4.4xlarge", "r3.8xlarge", "r4.16xlarge"
  ]
}

variable "use_tls" {
  description = "Use TLS to connect to the pool"
  default = "false"
}

variable "pool_address" {
  description = "Address of the pool to commect with the miner"
  default = ""
}

variable "wallet_address" {
  description = "Wallet address used by miner"
  default = ""
}

variable "pool_password" {
  description = "Password for the pool"
  default = ""
}

variable "git_tag" {
  description = "Git tag for checkout"
  default = "tags/v1.3.0-1.5.0"
}
