/*
 * Device driver for TYRION:
 *
 * A Platform device implemented using the misc subsystem
 *
 * Chae Jubb
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include "tyrion.h"

#include <linux/delay.h>

#define DRIVER_NAME "tyrion"
#define TYRION_INT_NUM 72

/*
 * Information about our device
 */
struct tyrion_dev {
	struct resource res;
	void __iomem *virtbase;
} dev;

static irqreturn_t tyrion_interrupt(int irq, void *dev_id)
{
	if (irq != TYRION_INT_NUM)
		return IRQ_NONE;
	/* TODO add irq handling */
printk(KERN_EMERG "tyrion interrupt\n");
	return IRQ_HANDLED;
}
#if 0
static void change_write_to_tyrion(struct tyrion_dev *dev, tyrion_arg_t vla)
{
	u64 message = 0;

	message |= ((u64) vla.type  << 0);
	message |= ((u64) vla.pid   << 8);
	message |= ((u64) vla.pri   << 24);
	message |= ((u64) vla.state << 32);

	iowrite32((u32) message,         dev->virtbase + CHANGE_REQ);
	iowrite32((u32) (message >> 32), dev->virtbase + CHANGE_REQ);
}
#endif

static long write_to_tyrion(tyrion_arg_t vla) {
	unsigned i;
	size_t num_to_write;
	uint32_t *buf;

	num_to_write = 2 * vla.length;
	if(!(buf = kmalloc(num_to_write * sizeof *buf, GFP_KERNEL))) {
		return -ENOMEM;
	}

	if (copy_from_user(buf, vla.mem_base, num_to_write * sizeof *buf)) {
		kfree(buf);
		return -EFAULT;
	}

	for (i = 0; i < num_to_write; ++i) {
		uint32_t value;

		value = buf[i];
		iowrite32(value, dev.virtbase);
	}

	return 0;
}

static long read_from_tyrion(tyrion_arg_t vla) {
	unsigned i;
	long ret;
	size_t num_to_read;
	uint32_t *buf;

	num_to_read = 2 * vla.length;
	if(!(buf = kmalloc(num_to_read * sizeof *buf, GFP_KERNEL))) {
		ret = -ENOMEM;
		goto failed_allocation;
	}

	for (i = 0; i < num_to_read; ++i) {
		uint32_t value;

		value = ioread32(dev.virtbase);
		buf[i] = value;
	}

	if (copy_to_user(vla.mem_base, buf, num_to_read * sizeof *buf)) {
		ret = -EFAULT;
		goto failed_copy;
	}

	kfree(buf);
	return 0;

failed_copy:
	kfree(buf);
failed_allocation:
	return ret;
}

/*
 * Handle ioctl() calls from userspace
 */
static long tyrion_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	long ret;
	tyrion_arg_t vla;

	switch (cmd) {
		case TYRION_WRITE:
			if (copy_from_user(&vla, (tyrion_arg_t *) arg,
						sizeof(tyrion_arg_t)))
				return -EACCES;
			ret = write_to_tyrion(vla);
			break;
		case TYRION_READ:
			if (copy_from_user(&vla, (tyrion_arg_t *) arg,
						sizeof(tyrion_arg_t)))
				return -EACCES;
			ret = read_from_tyrion(vla);
			break;

		default:
			return -EINVAL;
	}

	return ret;
}

/* The operations our device knows how to do */
static const struct file_operations tyrion_fops = {
	.owner          = THIS_MODULE,
	.unlocked_ioctl = tyrion_ioctl,
};

/* Information about our device for the "misc" framework -- like a char dev */
static struct miscdevice tyrion_misc_device = {
	.minor  = MISC_DYNAMIC_MINOR,
	.name   = DRIVER_NAME,
	.fops   = &tyrion_fops,
};

/*
 * Initialization code: get resources (registers) and display
 * a welcome message
 */
static int __init tyrion_probe(struct platform_device *pdev)
{
	int ret;

	/* Register ourselves as a misc device: creates /dev/tyrion */
	ret = misc_register(&tyrion_misc_device);

	/* Get the address of our registers from the device tree */
	ret = of_address_to_resource(pdev->dev.of_node, 0, &dev.res);
	if (ret) {
		ret = -ENOENT;
		goto out_deregister;
	}

	/* Make sure we can use these registers */
	if (request_mem_region(dev.res.start, resource_size(&dev.res),
			       DRIVER_NAME) == NULL) {
		ret = -EBUSY;
		goto out_deregister;
	}

	/* Arrange access to our registers */
	dev.virtbase = of_iomap(pdev->dev.of_node, 0);
	if (dev.virtbase == NULL) {
		ret = -ENOMEM;
		goto out_release_mem_region;
	}

	/* irq */
	ret = request_irq(TYRION_INT_NUM, tyrion_interrupt, 0, DRIVER_NAME, NULL);
	if (ret < 0)
		goto fail_request_irq;

	return 0;

fail_request_irq:
	iounmap(dev.virtbase);
out_release_mem_region:
	release_mem_region(dev.res.start, resource_size(&dev.res));
out_deregister:
	misc_deregister(&tyrion_misc_device);
	return ret;
}

/* Clean-up code: release resources */
static int tyrion_remove(struct platform_device *pdev)
{
	free_irq(TYRION_INT_NUM, NULL);
	iounmap(dev.virtbase);
	release_mem_region(dev.res.start, resource_size(&dev.res));
	misc_deregister(&tyrion_misc_device);
	return 0;
}

/* Which "compatible" string(s) to search for in the Device Tree */
#ifdef CONFIG_OF
static const struct of_device_id tyrion_of_match[] = {
	{ .compatible = "altr,tyrion" },
	{},
};
MODULE_DEVICE_TABLE(of, tyrion_of_match);
#endif

/* Information for registering ourselves as a "platform" driver */
static struct platform_driver tyrion_driver = {
	.driver	= {
		.name           = DRIVER_NAME,
		.owner          = THIS_MODULE,
		.of_match_table = of_match_ptr(tyrion_of_match),
	},
	.remove	= __exit_p(tyrion_remove),
};

/* Called when the module is loaded: set things up */
static int __init tyrion_init(void)
{
#if 0
	tyrion_dev_info.change_write_to_tyrion = change_write_to_tyrion;
	tyrion_dev_info.sched_write_to_tyrion = sched_write_to_tyrion;
	tyrion = &tyrion_dev_info;
#endif

	pr_info(DRIVER_NAME ": init\n");
	return platform_driver_probe(&tyrion_driver, tyrion_probe);
}

/* Called when the module is unloaded: release resources */
static void __exit tyrion_exit(void)
{
#if 0
	tyrion = NULL;
#endif

	platform_driver_unregister(&tyrion_driver);
	pr_info(DRIVER_NAME ": exit\n");
}

module_init(tyrion_init);
module_exit(tyrion_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chae Jubb");
MODULE_DESCRIPTION("TYRION");

