// SPDX-License-Identifier: GPL-2.0-only
/*
 * UNISOC UFS Host Controller driver
 *
 * Copyright (C) 2022 Unisoc, Inc.
 * Author: han mao <han.mao@unisoc.com>
 */

#include <linux/arm-smccc.h>
#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/reset.h>
#include <ufs/ufshcd.h>
#include "ufshcd-pltfrm.h"
#include <ufs/ufs_quirks.h>
#include <ufs/unipro.h>

static void ufs_qemu_remove(struct platform_device *pdev)
{
	ufshcd_pltfrm_remove(pdev);
}

static int ufs_qemu_vops_init(struct ufs_hba *hba)
{
	struct device *dev = hba->dev;

	hba->caps |= UFSHCD_CAP_CLK_GATING | UFSHCD_CAP_HIBERN8_WITH_CLK_GATING;
	hba->caps |= UFSHCD_CAP_CLK_SCALING | UFSHCD_CAP_WB_WITH_CLK_SCALING;
	hba->caps |= UFSHCD_CAP_AUTO_BKOPS_SUSPEND;

	return 0;
}

static int ufs_qemu_mcq_config_resource(struct ufs_hba *hba)
{
	hba->mcq_base = hba->mmio_base + ufshcd_mcq_queue_cfg_addr(hba);

	return 0;
}

static int ufs_qemu_op_runtime_config(struct ufs_hba *hba)
{
	struct ufshcd_mcq_opr_info_t *opr;
	int i;

	u32 sqdao = ufsmcq_readl(hba, ufshcd_mcq_cfg_offset(REG_SQDAO, 0));
	u32 sqisao = ufsmcq_readl(hba, ufshcd_mcq_cfg_offset(REG_SQISAO, 0));
	u32 cqdao = ufsmcq_readl(hba, ufshcd_mcq_cfg_offset(REG_CQDAO, 0));
	u32 cqisao = ufsmcq_readl(hba, ufshcd_mcq_cfg_offset(REG_CQISAO, 0));

	hba->mcq_opr[OPR_SQD].offset = sqdao;
	hba->mcq_opr[OPR_SQIS].offset = sqisao;
	hba->mcq_opr[OPR_CQD].offset = cqdao;
	hba->mcq_opr[OPR_CQIS].offset = cqisao;

	for (i = 0; i < OPR_MAX; i++) {
		opr = &hba->mcq_opr[i];
		opr->stride = 48;
		opr->base = hba->mmio_base + opr->offset;
	}

	return 0;
}

static const struct ufs_hba_variant_ops ufs_qemu_hba_vops = {
	.name = "ufs-qemu",
	.init = ufs_qemu_vops_init,
	.mcq_config_resource	= ufs_qemu_mcq_config_resource,
	.op_runtime_config	= ufs_qemu_op_runtime_config,
	// .device_reset = ufs_rockchip_device_reset,
};

static const struct of_device_id ufs_qemu_of_match[] = {
	{ .compatible = "ufs-qemu", .data = &ufs_qemu_hba_vops},
	{},
};

static int ufs_qemu_probe(struct platform_device *pdev)
{
	int err;
	struct device *dev = &pdev->dev;
	const struct of_device_id *of_id;

    printk("enter int ufs_qemu probe!!!!!!!\n\n");

	of_id = of_match_node(ufs_qemu_of_match, dev->of_node);

	err = ufshcd_pltfrm_init(pdev, of_id->data);
	if (err)
		dev_err(dev, "ufshcd_pltfrm_init() failed %d\n", err);

    return 0;
}

static struct platform_driver ufs_qemu_pltform = {
    .driver = {
        .name = "ufs-qemu",
        .of_match_table = ufs_qemu_of_match,
    },
    .probe = ufs_qemu_probe,
    .remove = ufs_qemu_remove,
};

module_platform_driver(ufs_qemu_pltform);

MODULE_DESCRIPTION("QEMU UFS Host Driver");
MODULE_LICENSE("GPL v2");
