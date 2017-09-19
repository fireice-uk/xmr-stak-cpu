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

resource "azurerm_virtual_network" "vnet" {
  name                = "${var.prefix}-vnet"
  location            = "${var.location}"
  address_space       = ["${var.address_space}"]
  resource_group_name = "${var.resource_group}"
}

resource "azurerm_subnet" "subnet" {
  name                 = "${var.prefix}-subnet"
  virtual_network_name = "${azurerm_virtual_network.vnet.name}"
  resource_group_name  = "${var.resource_group}"
  address_prefix       = "${var.subnet_prefix}"
}

resource "azurerm_network_interface" "nic" {
  count               = "${length(var.vm_sizes)}"
  name                = "${var.prefix}-app-${count.index + 1}-nic"
  location            = "${var.location}"
  resource_group_name = "${var.resource_group}"

  ip_configuration {
    name                          = "${var.prefix}-app-${count.index + 1}-ipconfig"
    subnet_id                     = "${azurerm_subnet.subnet.id}"
    private_ip_address_allocation = "Dynamic"
    public_ip_address_id          = "${element(azurerm_public_ip.pip.*.id, count.index)}"
  }
}

resource "azurerm_public_ip" "pip" {
  count                        = "${length(var.vm_sizes)}"
  name                         = "${var.prefix}-app-${count.index + 1}-ip"
  location                     = "${var.location}"
  resource_group_name          = "${var.resource_group}"
  public_ip_address_allocation = "Dynamic"
  domain_name_label            = "${var.prefix}-app-${count.index + 1}"
}

resource "azurerm_virtual_machine" "vm" {
  count                            = "${length(var.vm_sizes)}"
  name                             = "${var.prefix}-app-${count.index + 1}-vm"
  location                         = "${var.location}"
  resource_group_name              = "${var.resource_group}"
  vm_size                          = "${element(var.vm_sizes, count.index)}"
  network_interface_ids            = ["${element(azurerm_network_interface.nic.*.id, count.index)}"]
  delete_os_disk_on_termination    = true
  delete_data_disks_on_termination = true

  storage_image_reference {
    publisher = "${var.image_publisher}"
    offer     = "${var.image_offer}"
    sku       = "${var.image_sku}"
    version   = "${var.image_version}"
  }

  storage_os_disk {
    name              = "${var.prefix}-app-${count.index + 1}-osdisk"
    managed_disk_type = "Standard_LRS"
    caching           = "ReadWrite"
    create_option     = "FromImage"
  }

  os_profile {
    computer_name  = "${var.prefix}-app-${count.index + 1}"
    admin_username = "${var.admin_username}"
    admin_password = "${var.admin_password}"
    custom_data    = "${base64encode(data.template_file.user_data_file.rendered)}"
  }

  os_profile_linux_config {
    disable_password_authentication = true
    ssh_keys {
      path = "/home/${var.admin_username}/.ssh/authorized_keys"
      key_data = "${file(var.ssh_public_key)}"
    }
  }

  tags {
    Hostname = "${var.prefix}-app-${count.index + 1}"
    Environment = "Development"
  }
}
