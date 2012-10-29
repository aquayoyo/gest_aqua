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
	{ 0x2680b2ec, "ov9655_set_agc" },
	{ 0xfa2a45e, "__memzero" },
	{ 0xb7ad3cec, "ov9655_get_agc" },
	{ 0x38587a00, "ov9655_set_aec" },
	{ 0x463d9387, "ov9655_get_aec" },
	{ 0xf9a482f9, "msleep" },
	{ 0x5e0dc651, "ov9655_write_reg" },
	{ 0x2f70375, "ov9655_read_reg" },
	{ 0xa5cda817, "ipu_csi_enable_mclk" },
	{ 0x98082893, "__copy_to_user" },
	{ 0x17a142df, "__copy_from_user" },
	{ 0xaeac99, "clk_put" },
	{ 0x3d4a85a5, "clk_set_rate" },
	{ 0xca7233c7, "clk_round_rate" },
	{ 0xbf290d7f, "clk_get" },
	{ 0xc3bf91e6, "device_create" },
	{ 0x254767ac, "__class_create" },
	{ 0xa44e9a66, "register_chrdev" },
	{ 0xea147363, "printk" },
	{ 0x9ef749e2, "unregister_chrdev" },
	{ 0x4d885b78, "class_destroy" },
	{ 0x967b6d9b, "device_destroy" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=ov9655";

