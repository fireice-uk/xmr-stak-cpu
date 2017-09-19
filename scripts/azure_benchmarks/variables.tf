variable "resource_group" {
  description = "The name of the resource group in which to create the virtual network."
  default = ""
}

variable "prefix" {
  description = "Prefix for all created items"
  default = ""
}

variable "location" {
  description = "The location/region where the virtual network is created. Changing this forces a new resource to be created."
  default     = ""
}

variable "address_space" {
  description = "The address space that is used by the virtual network. You can supply more than one address space. Changing this forces a new resource to be created."
  default     = "10.0.0.0/16"
}

variable "subnet_prefix" {
  description = "The address prefix to use for the subnet."
  default     = "10.0.10.0/24"
}

variable "storage_account_type" {
  description = "Defines the type of storage account to be created. Valid options are Standard_LRS, Standard_ZRS, Standard_GRS, Standard_RAGRS, Premium_LRS. Changing this is sometimes valid - see the Azure documentation for more information on which types of accounts can be converted into other types."
  default     = "Standard_LRS"
}

#http://azureprice.net/?region=eastus2
variable "vm_sizes" {
  description = "Specifies the size of the virtual machine."
  type        = "list"
  default     = [
    "Standard_A0", "Standard_A1", "Standard_A2", "Standard_A3", "Standard_A4", "Standard_A5", "Standard_A6", "Standard_A7",
    "Standard_A1_v2", "Standard_A2_v2", "Standard_A4_v2", "Standard_A8_v2", 
    "Standard_D1_v2", "Standard_D2_v2", "Standard_D3_v2", "Standard_D4_v2", "Standard_D5_v2", "Standard_D15_v2",
    "Standard_D2_v3", "Standard_D4_v3", "Standard_D8_v3", "Standard_D16_v3", "Standard_D32_v3", "Standard_D64_v3",
    "Standard_F4", "Standard_F8", "Standard_F16"
  ]
}

variable "image_publisher" {
  description = "name of the publisher of the image (az vm image list)"
  default     = "Canonical"
}

variable "image_offer" {
  description = "the name of the offer (az vm image list)"
  default     = "UbuntuServer"
}

variable "image_sku" {
  description = "image sku to apply (az vm image list)"
  default     = "16.04-LTS"
}

variable "image_version" {
  description = "version of the image to apply (az vm image list)"
  default     = "latest"
}

variable "admin_username" {
  description = "administrator user name"
  default     = "ubuntu"
}

variable "admin_password" {
  description = "administrator password (recommended to disable password auth)"
  default = ""
}

variable "disable_password_authentication" {
    default = true
}

variable "ssh_public_key" {
    description = "ssh public key"
    default = "~/.ssh/id_rsa.pub"
}


variable "use_tls" {
  description = "Use TLS to connect to the pool"
  default = ""
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
