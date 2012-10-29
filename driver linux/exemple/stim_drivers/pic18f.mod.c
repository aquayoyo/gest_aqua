#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x8cd31698, "struct_module" },
	{ 0xd68a2a69, "mxc_get_gpio_datain" },
	{ 0x1e8a8f99, "mxc_set_gpio_direction" },
	{ 0x353e3fa5, "__get_user_4" },
	{ 0xee1e1c6d, "InitPic18f" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xc3bf91e6, "device_create" },
	{ 0x254767ac, "__class_create" },
	{ 0xea147363, "printk" },
	{ 0xa44e9a66, "register_chrdev" },
	{ 0x393e9495, "mxc_set_gpio_dataout" },
	{ 0x9ef749e2, "unregister_chrdev" },
	{ 0x4d885b78, "class_destroy" },
	{ 0x967b6d9b, "device_destroy" },
	{ 0xabbd558e, "pmic_write_reg" },
	{ 0x3b8138f2, "UnInitPic18f" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

