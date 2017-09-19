output "VM Size" {
  value = "${azurerm_virtual_machine.vm.*.vm_size}"
}

output "FQDN" {
  value = "${azurerm_public_ip.pip.*.fqdn}"
}

output "Hostname" {
  value = "${azurerm_public_ip.pip.*.domain_name_label}"
}

output "IP Addresses" {
  value = "${azurerm_public_ip.pip.*.ip_address}"
}

output "SSH Command" {
  value = "${
    formatlist(
      "ssh -i ${var.ssh_public_key} ${var.admin_username}@%s",
      azurerm_public_ip.pip.*.ip_address,
    )}"
}
