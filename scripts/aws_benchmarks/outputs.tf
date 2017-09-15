output "IP Addresses" {
  value = ["${aws_instance.app_server.*.private_ip}"]
}

output "Instance types" {
  value = ["${aws_instance.app_server.*.instance_type}"]
}
