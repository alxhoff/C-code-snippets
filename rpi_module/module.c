
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello world");
MODULE_VERSION("1.0");
MODULE_AUTHOR("Alex Hoffman");

static int __init init(void)
{
	printk(KERN_INFO "Hello world\n");
	return 0;
}

static void __exit cleanup(void)
{
    printk(KERN_INFO "Goodbye world\n");
}

module_init(init);
module_exit(cleanup);
