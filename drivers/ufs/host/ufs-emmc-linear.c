// SPDX-License-Identifier: GPL-2.0
/*
 * Simple Device Mapper target with hardcoded eMMC and UFS devices
 */

#include <linux/device-mapper.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/bio.h>
#include <linux/blkdev.h>

#define DM_MSG_PREFIX "emmc-ufs"

#define EMMC_DEVICE "/dev/mmcblk0"
#define UFS_DEVICE "/dev/sda"

struct emmc_ufs_c {
	struct dm_dev *emmc_dev;
	struct dm_dev *ufs_dev;
	sector_t emmc_sectors;
	sector_t ufs_sectors;
};

/*
 * 构造函数 - 硬编码设备
 */
static int emmc_ufs_ctr(struct dm_target *ti, unsigned int argc, char **argv)
{
	struct emmc_ufs_c *ec;
	int ret;
	struct block_device *bdev;

	ec = kzalloc(sizeof(*ec), GFP_KERNEL);
	if (!ec) {
		ti->error = "Cannot allocate context";
		return -ENOMEM;
	}

	pr_info("EMMC-UFS: Using hardcoded devices: %s and %s\n", 
		EMMC_DEVICE, UFS_DEVICE);

	// 打开硬编码的 eMMC 设备
	ret = dm_get_device(ti, EMMC_DEVICE, BLK_OPEN_READ | BLK_OPEN_WRITE, &ec->emmc_dev);
	if (ret) {
		ti->error = "eMMC device not found";
		goto bad;
	}

	// 打开硬编码的 UFS 设备
	ret = dm_get_device(ti, UFS_DEVICE, BLK_OPEN_READ | BLK_OPEN_WRITE, &ec->ufs_dev);
	if (ret) {
		ti->error = "UFS device not found";
		goto bad_emmc;
	}

	// 获取设备大小
	bdev = ec->emmc_dev->bdev;
	ec->emmc_sectors = bdev_nr_sectors(bdev);
	
	bdev = ec->ufs_dev->bdev;
	ec->ufs_sectors = bdev_nr_sectors(bdev);

	ti->private = ec;
	ti->len = ec->emmc_sectors + ec->ufs_sectors;
	ti->num_discard_bios = 1;
	ti->num_flush_bios = 1;
	ti->num_write_zeroes_bios = 1;

	pr_info("EMMC-UFS: Combined size: %llu sectors\n", 
		(unsigned long long)ti->len);

	return 0;

bad_emmc:
	dm_put_device(ti, ec->emmc_dev);
bad:
	kfree(ec);
	return ret;
}

static void emmc_ufs_dtr(struct dm_target *ti)
{
	struct emmc_ufs_c *ec = ti->private;

	dm_put_device(ti, ec->ufs_dev);
	dm_put_device(ti, ec->emmc_dev);
	kfree(ec);
}

static int emmc_ufs_map(struct dm_target *ti, struct bio *bio)
{
	struct emmc_ufs_c *ec = ti->private;
	sector_t sector = bio->bi_iter.bi_sector;

	if (sector < ec->emmc_sectors) {
		bio_set_dev(bio, ec->emmc_dev->bdev);
	} else {
		bio_set_dev(bio, ec->ufs_dev->bdev);
		bio->bi_iter.bi_sector = sector - ec->emmc_sectors;
	}

	return DM_MAPIO_REMAPPED;
}

static void emmc_ufs_status(struct dm_target *ti, status_type_t type,
			   unsigned int status_flags, char *result, unsigned int maxlen)
{
	struct emmc_ufs_c *ec = ti->private;

	switch (type) {
	case STATUSTYPE_INFO:
		printk("%llu %llu", 
		       (unsigned long long)ec->emmc_sectors,
		       (unsigned long long)ec->ufs_sectors);
		break;
	case STATUSTYPE_TABLE:
		printk("hardcoded"); // 不显示设备路径
		break;
	}
}

static int emmc_ufs_iterate_devices(struct dm_target *ti,
				   iterate_devices_callout_fn fn, void *data)
{
	struct emmc_ufs_c *ec = ti->private;
	int ret;

	ret = fn(ti, ec->emmc_dev, 0, ec->emmc_sectors, data);
	if (ret)
		return ret;
		
	return fn(ti, ec->ufs_dev, 0, ec->ufs_sectors, data);
}

static struct target_type emmc_ufs_target = {
	.name = "emmc_ufs",
	.version = {1, 0, 0},
	.features = DM_TARGET_PASSES_INTEGRITY,
	.module = THIS_MODULE,
	.ctr = emmc_ufs_ctr,
	.dtr = emmc_ufs_dtr,
	.map = emmc_ufs_map,
	.status = emmc_ufs_status,
	.iterate_devices = emmc_ufs_iterate_devices,
};

static int __init dm_emmc_ufs_init(void)
{
	int r = dm_register_target(&emmc_ufs_target);
	if (r < 0)
		DMERR("Registration failed %d", r);
	else
		pr_info("EMMC-UFS: Hardcoded target registered\n");
	return r;
}

static void __exit dm_emmc_ufs_exit(void)
{
	dm_unregister_target(&emmc_ufs_target);
	pr_info("EMMC-UFS: Module unloaded\n");
}

module_init(dm_emmc_ufs_init);
module_exit(dm_emmc_ufs_exit);

MODULE_DESCRIPTION("Hardcoded eMMC and UFS Device Mapper target");
MODULE_AUTHOR("Kernel Developer");
MODULE_LICENSE("GPL");