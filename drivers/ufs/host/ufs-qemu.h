// SPDX-License-Identifier: GPL-2.0-only
/*
 * UNISOC UFS Host Controller driver
 *
 * Copyright (C) 2022 Unisoc, Inc.
 * Author: han mao <han.mao@unisoc.com>
 */

#ifndef _UFS_QEMU_H_
#define _UFS_QEMU_H_

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

struct ufs_qemu_host {
	struct ufs_hba *hba;
	struct ufs_qemu_priv *priv;
	void __iomem *ufs_dbg_mmio;
};

#endif


