# Specify the provider and access details
provider "aws" {
  region = "${var.aws_region}"
}

data "aws_ami" "ubuntu" {
  most_recent = true
  filter {
    name = "name"
    values = ["ubuntu/images/hvm-ssd/ubuntu-*16.04-amd64-server-*"]

  }
  filter {
    name = "virtualization-type"
    values = ["hvm"]
  }
  owners = ["099720109477"] # Canonical
}

resource "aws_key_pair" "ssh_key_pair" {
  key_name   = "${var.prefix}-ssh_key_pair"
  public_key = "${file("~/.ssh/id_rsa.pub")}"
}

data "template_file" "user_data_file" {
    template = "${file("userdata.sh")}"
    vars = {
        use_tls = "${var.use_tls}"
        pool_address = "${var.pool_address}"
        wallet_address = "${var.wallet_address}"
        pool_password = "${var.pool_password}"
        git_tag = "${var.git_tag}"
    }
}

resource "aws_instance" "app_server" {
  count = "${length(var.aws_instances)}"
  ami = "${data.aws_ami.ubuntu.id}"
  instance_initiated_shutdown_behavior = "terminate"
  instance_type = "${element(var.aws_instances, count.index)}"
  key_name = "${var.prefix}-ssh_key_pair"
  subnet_id = "${var.aws_subnet_id}"
  user_data = "${data.template_file.user_data_file.rendered}"

  tags {
    Name = "${var.prefix}-app-${count.index + 1}"
    Application = "${var.prefix}-${count.index + 1}"
  }

  volume_tags {
    Name = "${var.prefix}-app-${count.index + 1}"
    Application = "${var.prefix}-${count.index + 1}"
  }

  root_block_device {
    volume_type = "standard"
    delete_on_termination = true
  }
}
