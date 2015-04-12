#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>
#include <linux/completion.h>
#include <linux/interrupt.h>
#include <linux/pagemap.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/log2.h>
#include <linux/slab.h>
#include <linux/dad.h>
#include <linux/fs.h>
#include <linux/mm.h>

#include <asm/uaccess.h>

#include "svd.h"

/* device struct */
struct svd_device {
	struct cdev cdev;
  struct dad_device *dad_dev; /* parent device */
  struct device *dev;
  struct module *module;
  struct mutex lock;
  struct completion completion;
  void __iomem *iomem; /* mmapped regs */
  size_t max_size;
  int number;
};

struct svd_file {
  struct svd_device *dev;
  void *vbuf; /* virtual address of contiguous buffer */
  dma_addr_t dma_handle; /* physical address of that buffer */
  size_t size;
};

static const struct dad_device_id svd_ids[] = {
  { SVD_SYNC_DEV_ID },
  { },
};

static struct class *svd_class;
static dev_t svd_devno;
static dev_t svd_n_devices;

static irqreturn_t svd_irq(int irq, void *dev) {
 struct svd_device *svd = dev;
 u32 cmd_reg;

 cmd_reg = ioread32(svd->iomem + SVD_REG_CMD);
 cmd_reg >>= SVD_CMD_IRQ_SHIFT;
 cmd_reg &= SVD_CMD_IRQ_MASK;

 if (cmd_reg == SVD_CMD_IRQ_DONE) {
	 complete_all(&svd->completion);
	 iowrite32(0, svd->iomem + SVD_REG_CMD);
	 return IRQ_HANDLED;
 }
 return IRQ_NONE;
}

static bool svd_access_ok(struct svd_device *svd,
			const struct svd_access *access) {
	unsigned max_sz = ioread32(svd->iomem + SVD_REG_MAX_SIZE);
	if (access->size > max_sz ||
		access->size <= 0)
		return false;

	return true;
}

static int svd_transfer(struct svd_device *svd, struct svd_file *file, const
		struct svd_access *access) {
	/* compute the input and output burst */
	int wait;

	unsigned sz = access->size; size_t in_buf_size = SVD_INPUT_SIZE_BYTE(sz);

	INIT_COMPLETION(svd->completion);

	if (!svd_access_ok(svd, access))
		return -EINVAL;

	iowrite32(file->dma_handle, svd->iomem + SVD_REG_SRC);
	iowrite32(file->dma_handle + in_buf_size, svd->iomem + SVD_REG_DST);
	iowrite32(access->size, svd->iomem + SVD_REG_SIZE);
	iowrite32(0x1, svd->iomem + SVD_REG_CMD);

	wait = wait_for_completion_interruptible(&svd->completion);
	if (wait < 0)
		return -EINTR;
	return 0;
}

static int svd_access_ioctl( struct svd_device *svd, struct svd_file *file,
		void __user *arg) {
	struct svd_access access;

	if (copy_from_user(&access, arg, sizeof(access)))
		return -EFAULT;

	if (!svd_access_ok(svd, &access))
		return -EINVAL;

	if(mutex_lock_interruptible(&svd->lock))
		return -EINTR;

	svd_transfer(svd, file, &access);
	mutex_unlock(&svd->lock);

	return 0;
}

static long svd_do_ioctl(
			struct file *file,
			unsigned int cm,
			void __user *arg) {
	struct svd_file *priv = file->private_data;
	struct svd_device *svd = priv->dev;

	switch (cm) {
		case SVD_IOC_ACCESS:
			return svd_access_ioctl(svd, priv, arg);
		default:
			return -ENOTTY;
	}
}

static long svd_ioctl(
			struct file *file,
			unsigned int cm,
			unsigned long arg) {
	return svd_do_ioctl(file, cm, (void __user *)arg);
}

static int svd_mmap(struct file *file, struct vm_area_struct *vma) {
	struct svd_file *priv = file->private_data;
	struct svd_device *svd = priv->dev;
	unsigned long pfn;
	size_t size;

	size = vma->vm_end - vma->vm_start;
	if (size > priv->size) {
		dev_info(svd->dev, "size: %zu, max size: %zu\n", size, priv->size);
		return -EINVAL;
	}
	pfn = page_to_pfn(virt_to_page(priv->vbuf));
	return remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot);
}

static int svd_open(struct inode *inode, struct file *file) {
	struct svd_file *priv;
	struct svd_device *svd;
	int rc;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (priv == NULL)
		return -ENOMEM;

	svd = container_of(inode->i_cdev, struct svd_device, cdev);
	priv->dev = svd;
	priv->size = svd->max_size;

	priv->vbuf = dma_alloc_coherent(NULL, priv->size, &priv->dma_handle, GFP_KERNEL);
	if (priv->vbuf == NULL) {
		dev_err(svd->dev, "cannot allocate contiguous DMA buffer of size %zu\n", priv->size);
		rc = -ENOMEM;
		goto err_dma_alloc;
	}

	if (!try_module_get(svd->module)) {
		rc = -ENODEV;
		goto err_module_get;
	}

	file->private_data = priv;
	return 0;

err_module_get:
	dma_free_coherent(NULL, priv->size, priv->vbuf, priv->dma_handle);
err_dma_alloc:
	kfree(priv);
	return rc;
}

static int svd_release(struct inode *inode, struct file *file) {
	struct svd_file *priv = file->private_data;
	struct svd_device *svd = priv->dev;

	module_put(svd->module);
	dma_free_coherent(NULL, priv->size, priv->vbuf, priv->dma_handle);
	kfree(priv);
	return 0;
}

/* driver-defined functions for interacting with device */
static const struct file_operations svd_fops = {
	.owner           = THIS_MODULE,
  .open            = svd_open,
  .release         = svd_release,
  .unlocked_ioctl  = svd_ioctl,
  .mmap            = svd_mmap,
};	

static int svd_create_cdev(struct svd_device *svd, int ndev) {
	dev_t devno = MKDEV(MAJOR(svd_devno), ndev);
	int rc;

	/* char device registration */
	cdev_init(&svd->cdev, &svd_fops);
	svd->cdev.owner = THIS_MODULE;
	rc = cdev_add(&svd->cdev, devno, 1);
	if (rc) {
		dev_err(svd->dev, "Error %d adding cdev %d\n", rc, ndev);
		goto out;
	}

	svd->dev = device_create(svd_class, svd->dev, devno, NULL, "svd.%i", ndev);
	if (IS_ERR(svd->dev)) {
		rc = PTR_ERR(svd->dev);
		dev_err(svd->dev, "Error %d creating device %d\n", rc, ndev);
		svd->dev = NULL;
		goto device_create_failed;
	}

	dev_set_drvdata(svd->dev, svd);
	return 0;

device_create_failed:
	cdev_del(&svd->cdev);
out:
	return rc;
}

static void svd_destroy_cdev(struct svd_device *svd, int ndev) {
	dev_t devno = MKDEV(MAJOR(svd_devno), ndev);
	
	device_destroy(svd_class, devno);
	cdev_del(&svd->cdev);
}

static int svd_probe(struct dad_device *dev) {
	struct svd_device *svd;
	int dev_id;
	int rc;
	unsigned max_sz;

	svd = kzalloc(sizeof(*svd), GFP_KERNEL);
	if (svd == NULL)
		return -ENOMEM;
	svd->module = THIS_MODULE;
	svd->dad_dev = dev;
	svd->number = svd_n_devices;
	mutex_init(&svd->lock);
	init_completion(&svd->completion);

	svd->iomem = ioremap(dev->addr, dev->length);
	if (svd->iomem == NULL) {
		rc = -ENOMEM;
		goto err_ioremap;
	}

	dev_id = ioread32(svd->iomem + SVD_REG_ID);
	if (dev_id != SVD_SYNC_DEV_ID) {
		rc = -ENODEV;
		goto err_reg_read;
	}

	rc = svd_create_cdev(svd, svd->number);
	if (rc)
		goto err_cdev;

	rc = request_irq(dev->irq, svd_irq, IRQF_SHARED, "svd", svd);
	if (rc) {
		dev_info(svd->dev, "cannot request IRQ number %d\n", -EMSGSIZE);
		goto err_irq;
	}

	svd_n_devices++;
	dev_set_drvdata(&dev->device, svd);

	max_sz = ioread32(svd->iomem + SVD_REG_MAX_SIZE);
	svd->max_size = round_up(SVD_BUF_SIZE_BYTE(max_sz), PAGE_SIZE);

	dev_info(svd->dev, "device registered.\n");

	return 0;

err_irq:
	svd_destroy_cdev(svd, svd->number);
err_cdev:
	iounmap(svd->iomem);
err_reg_read:
	iounmap(svd->iomem);
err_ioremap:
	kfree(svd);
	return rc;
}

static void __exit svd_remove(struct dad_device *dev) {
	struct svd_device *svd = dev_get_drvdata(&dev->device);

	/* free_irq(dev->irq, svd->dev); */
	svd_destroy_cdev(svd, svd->number);
	iounmap(svd->iomem);
	kfree(svd);
	dev_info(svd->dev, "device unregistered.\n");
}

static struct dad_driver svd_driver = {
	.probe      = svd_probe,
	.remove     = svd_remove,
  .name       = DRV_NAME,
  .id_table   = svd_ids,
};

static int __init svd_sysfs_device_create(void) {
	int rc;

	svd_class = class_create(THIS_MODULE, "svd");
	if (IS_ERR(svd_class)) {
		printk(KERN_ERR PFX "Failed to create svd class\n");
		rc = PTR_ERR(svd_class);
		goto out;
	}

	/* Dynamically allocating dev numbers */
	rc = alloc_chrdev_region(&svd_devno, 0, SVD_MAX_DEVICES, "svd");
	if (rc) {
		printk(KERN_ERR PFX "Failed to allocate chrdev region\n");
		goto alloc_chrdev_region_failed;
	}

	return 0;

alloc_chrdev_region_failed:
	class_destroy(svd_class);
out:
	return rc;
}

static void svd_sysfs_device_remove(void) {
	dev_t devno = MKDEV(MAJOR(svd_devno), 0);

	class_destroy(svd_class);

	/* get rid of dev numbers */
	unregister_chrdev_region(devno, SVD_MAX_DEVICES);
}

static int __init svd_init(void) {
	int rc;
	printk(KERN_INFO "Device-driver initialization\n");
	rc = svd_sysfs_device_create();
	if (rc)
		return rc;

	rc = dad_register_driver(&svd_driver);
	if (rc)
		goto err;

	return 0;

err:
	svd_sysfs_device_remove();
	return rc;
}


/* shut 'er down */
static void __exit svd_exit(void) {
	printk(KERN_INFO "Device-driver shutdown\n");
	dad_unregister_driver(&svd_driver);
	svd_sysfs_device_remove();
}

module_init(svd_init)
module_exit(svd_exit)

MODULE_AUTHOR("Chae Jubb <ecj2122@columbia.edu>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("svd driver");

