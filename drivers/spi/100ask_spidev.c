/*
 * Simple synchronous userspace interface to SPI devices
 *
 * Copyright (C) 2006 SWAPP
 *	Andrea Paterniani <a.paterniani@swapp-eng.it>
 * Copyright (C) 2007 David Brownell (simplification, cleanup)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/acpi.h>
#include <linux/cdev.h>

#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

#include <linux/uaccess.h>

#define SPIDEVX_IOC_MAGIC    's'
#define SPIDEVX_IOCINIT      _IOWR(SPIDEVX_IOC_MAGIC, 0, void *)
#define SPIDEVX_IOCEXIT      _IOWR(SPIDEVX_IOC_MAGIC, 1, void *)


struct spidevx_info_msg {
    int max_speed_hz;
    u16 mode;
    u16 chip_select;
};

static int spidevx_major;
static struct cdev spidevx_cdev;
static struct class *spidevx_class;


/*
 * This supports access to SPI devices using normal userspace I/O calls.
 * Note that while traditional UNIX/POSIX I/O semantics are half duplex,
 * and often mask message boundaries, full SPI support requires full duplex
 * transfers.  There are several kinds of internal message boundaries to
 * handle chipselect management and other protocol options.
 *
 * SPI has a character major number assigned.  We allocate minor numbers
 * dynamically using a bitmask.  You must use hotplug tools, such as udev
 * (or mdev with busybox) to create and destroy the /dev/spidevB.C device
 * nodes, since there is no fixed association of minor numbers with any
 * particular SPI bus or device.
 */
#define SPIDEV_MAJOR			153	/* assigned  spi 主设备号 */
#define N_SPI_MINORS			32	/* ... up to 256 */

static DECLARE_BITMAP(minors, N_SPI_MINORS);  //声明次设备位图


/* Bit masks for spi_device.mode management.  Note that incorrect
 * settings for some settings can cause *lots* of trouble for other
 * devices on a shared bus:
 *
 *  - CS_HIGH ... this device will be active when it shouldn't be
 *  - 3WIRE ... when active, it won't behave as it should
 *  - NO_CS ... there will be no explicit message boundaries; this
 *	is completely incompatible with the shared bus model
 *  - READY ... transfers may proceed when they shouldn't.
 *
 * REVISIT should changing those flags be privileged?
 */
#define SPI_MODE_MASK		(SPI_CPHA | SPI_CPOL | SPI_CS_HIGH \
				| SPI_LSB_FIRST | SPI_3WIRE | SPI_LOOP \
				| SPI_NO_CS | SPI_READY | SPI_TX_DUAL \
				| SPI_TX_QUAD | SPI_RX_DUAL | SPI_RX_QUAD)

struct spidev_data {
	dev_t			devt;  //设备号
	spinlock_t		spi_lock;  //自旋锁
	struct spi_device	*spi;  //spi 设备结构体
	struct list_head	device_entry;  //

	/* TX/RX buffers are NULL unless this device is open (users > 0) */
	struct mutex		buf_lock;  //互斥锁
	unsigned		users;  //使用者计数
	u8			*tx_buffer;  //发送缓存
	u8			*rx_buffer;  //接收缓存
	u32			speed_hz;
};

static LIST_HEAD(device_list);  //声明spi设备链表
static DEFINE_MUTEX(device_list_lock);  //定义互斥锁

static unsigned bufsiz = 4096;
module_param(bufsiz, uint, S_IRUGO);
MODULE_PARM_DESC(bufsiz, "data bytes in biggest supported SPI message");

/*-------------------------------------------------------------------------*/

static ssize_t
spidev_sync(struct spidev_data *spidev, struct spi_message *message)
{
	DECLARE_COMPLETION_ONSTACK(done);
	int status;
	struct spi_device *spi;

	spin_lock_irq(&spidev->spi_lock);
	spi = spidev->spi;
	spin_unlock_irq(&spidev->spi_lock);

	if (spi == NULL)  //判断是否有对应的设备
		status = -ESHUTDOWN;
	else
		status = spi_sync(spi, message);  //spi异步同步

	if (status == 0)
		status = message->actual_length;  //status 等于传输的实际长度

	return status;
}

static inline ssize_t
spidev_sync_write(struct spidev_data *spidev, size_t len)
{
	struct spi_transfer	t = {
			.tx_buf		= spidev->tx_buffer,  //发送缓冲区
			.len		= len,   //发送长度
			.speed_hz	= spidev->speed_hz,  //发送速度
		};
	struct spi_message	m;

	spi_message_init(&m);//初始化spi消息（初始化spi 传递事务队列）
	spi_message_add_tail(&t, &m);  //添加spi传递队列
	return spidev_sync(spidev, &m);  //同步写
}

static inline ssize_t
spidev_sync_read(struct spidev_data *spidev, size_t len)
{
	struct spi_transfer	t = {
			.rx_buf		= spidev->rx_buffer,  //接收缓存区
			.len		= len,  //接收长度
			.speed_hz	= spidev->speed_hz,  //接收速度
		};
	struct spi_message	m;

	spi_message_init(&m);  
	spi_message_add_tail(&t, &m);
	return spidev_sync(spidev, &m);
}

/*-------------------------------------------------------------------------*/

/* Read-only message with current device setup */
static ssize_t
spidev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct spidev_data	*spidev;
	ssize_t			status = 0;

	/* chipselect only toggles at start or end of operation */
	if (count > bufsiz)  //如果传输数据大于缓冲容量
		return -EMSGSIZE;

	spidev = filp->private_data;  //从文件私有数据指针获取 spidev_data

	mutex_lock(&spidev->buf_lock);  //加互斥锁
	status = spidev_sync_read(spidev, count);  //同步读，返回传输数据长度
	if (status > 0) {
		unsigned long	missing;  //丢失数据个数

		missing = copy_to_user(buf, spidev->rx_buffer, status);  //内核空间复制到用户空间
		if (missing == status)
			status = -EFAULT;
		else
			status = status - missing;  //计算传输成功的个数
	}
	mutex_unlock(&spidev->buf_lock);  //解锁

	return status;
}

/* Write-only message with current device setup */
static ssize_t
spidev_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	struct spidev_data	*spidev;
	ssize_t			status = 0;
	unsigned long		missing;

	/* chipselect only toggles at start or end of operation */
	if (count > bufsiz)  //传输数据大于缓冲容量
		return -EMSGSIZE;

	spidev = filp->private_data;  // 从文件私有数据指针获取spidev_data

	mutex_lock(&spidev->buf_lock); //加锁
	missing = copy_from_user(spidev->tx_buffer, buf, count);  //用户空间复制到内核空间
	if (missing == 0)
		status = spidev_sync_write(spidev, count);//同步写，返回数据传输长度
	else
		status = -EFAULT;
	mutex_unlock(&spidev->buf_lock);  //解锁

	return status;
}

static int spidev_message(struct spidev_data *spidev,
		struct spi_ioc_transfer *u_xfers, unsigned n_xfers)
{
	struct spi_message	msg;
	struct spi_transfer	*k_xfers;
	struct spi_transfer	*k_tmp;
	struct spi_ioc_transfer *u_tmp;
	unsigned		n, total, tx_total, rx_total;
	u8			*tx_buf, *rx_buf;
	int			status = -EFAULT;

	spi_message_init(&msg);  //初始化spi消息，（初始化spi传递事务队列）
	k_xfers = kcalloc(n_xfers, sizeof(*k_tmp), GFP_KERNEL);  //分配spi传输指针内存
	if (k_xfers == NULL)
		return -ENOMEM;

	/* Construct spi_message, copying any tx data to bounce buffer.
	 * We walk the array of user-provided transfers, using each one
	 * to initialize a kernel version of the same transfer.
	 */
	tx_buf = spidev->tx_buffer;  //获取发送缓存
	rx_buf = spidev->rx_buffer;  //获取接收缓存
	total = 0;
	tx_total = 0;
	rx_total = 0;
	//n = n_xfers为spi_ioc_transfer,u_tmp=u_xfers 为要处理的spi_ioc_transfer指针
	for (n = n_xfers, k_tmp = k_xfers, u_tmp = u_xfers;
			n;
			n--, k_tmp++, u_tmp++) {
		k_tmp->len = u_tmp->len; //设置传输常速

		total += k_tmp->len;//累计传输信息的总长度
		/* Since the function returns the total length of transfers
		 * on success, restrict the total to positive int values to
		 * avoid the return value looking like an error.  Also check
		 * each transfer length to avoid arithmetic overflow.
		 */
		if (total > INT_MAX || k_tmp->len > INT_MAX) {
			status = -EMSGSIZE;
			goto done;
		}

		if (u_tmp->rx_buf) {  //接收缓冲区指针不为空
			/* this transfer needs space in RX bounce buffer */
			rx_total += k_tmp->len;
			if (rx_total > bufsiz) {
				status = -EMSGSIZE;
				goto done;
			}
			k_tmp->rx_buf = rx_buf;  //缓冲区指向接收缓冲区
			if (!access_ok(VERIFY_WRITE, (u8 __user *)
						(uintptr_t) u_tmp->rx_buf,
						u_tmp->len))
				goto done;
			rx_buf += k_tmp->len;
		}
		if (u_tmp->tx_buf) {
			/* this transfer needs space in TX bounce buffer */
			tx_total += k_tmp->len;
			if (tx_total > bufsiz) {
				status = -EMSGSIZE;
				goto done;
			}
			k_tmp->tx_buf = tx_buf;
			if (copy_from_user(tx_buf, (const u8 __user *)  //用户空间复制到内核空间
						(uintptr_t) u_tmp->tx_buf,
					u_tmp->len))
				goto done;
			tx_buf += k_tmp->len;  //缓冲区指针移动移动一个传输信息的长度
		}

		k_tmp->cs_change = !!u_tmp->cs_change;  //设置cs_change
		k_tmp->tx_nbits = u_tmp->tx_nbits;
		k_tmp->rx_nbits = u_tmp->rx_nbits;
		k_tmp->bits_per_word = u_tmp->bits_per_word;  //设置 bits_per_word 一个字多少位
		k_tmp->delay_usecs = u_tmp->delay_usecs;  //设置毫秒级延时
		k_tmp->speed_hz = u_tmp->speed_hz;  //设置速度
		if (!k_tmp->speed_hz)
			k_tmp->speed_hz = spidev->speed_hz;
#ifdef VERBOSE
		dev_dbg(&spidev->spi->dev,
			"  xfer len %u %s%s%s%dbits %u usec %uHz\n",
			u_tmp->len,
			u_tmp->rx_buf ? "rx " : "",
			u_tmp->tx_buf ? "tx " : "",
			u_tmp->cs_change ? "cs " : "",
			u_tmp->bits_per_word ? : spidev->spi->bits_per_word,
			u_tmp->delay_usecs,
			u_tmp->speed_hz ? : spidev->spi->max_speed_hz);
#endif
		spi_message_add_tail(k_tmp, &msg);  //添加到队列
	}
	//for循环的作用是将spi_ioc_trensfer 批量转换位spi传递结构体spi_transfer ,然后添加到spi传递事务队列
	status = spidev_sync(spidev, &msg);  //同步读写
	if (status < 0)
		goto done;

	/* copy any rx data out of bounce buffer */
	rx_buf = spidev->rx_buffer;  //获取接收缓冲区指针
	for (n = n_xfers, u_tmp = u_xfers; n; n--, u_tmp++) {  //批量从内核空间复制spi_ioc_treansfer 到用户空间
		if (u_tmp->rx_buf) {
			if (__copy_to_user((u8 __user *)
					(uintptr_t) u_tmp->rx_buf, rx_buf,
					u_tmp->len)) {
				status = -EFAULT;
				goto done;
			}
			rx_buf += u_tmp->len;  //rx_buf 指针位置调整指向下一个 spi_ioc_trensfer
		}
	}
	status = total; //status 等于实际的传输长度

done:
	kfree(k_xfers);  //释放 k_sfers
	return status;
}

static struct spi_ioc_transfer *
spidev_get_ioc_message(unsigned int cmd, struct spi_ioc_transfer __user *u_ioc,
		unsigned *n_ioc)
{
	struct spi_ioc_transfer	*ioc;
	u32	tmp;

	/* Check type, command number and direction */  
	if (_IOC_TYPE(cmd) != SPI_IOC_MAGIC     ////判断控制命令的类型
			|| _IOC_NR(cmd) != _IOC_NR(SPI_IOC_MESSAGE(0)) 
			|| _IOC_DIR(cmd) != _IOC_WRITE)   //判断传输方向
		return ERR_PTR(-ENOTTY);

	tmp = _IOC_SIZE(cmd);
	if ((tmp % sizeof(struct spi_ioc_transfer)) != 0)
		return ERR_PTR(-EINVAL);
	*n_ioc = tmp / sizeof(struct spi_ioc_transfer);
	if (*n_ioc == 0)
		return NULL;

	/* copy into scratch area */
	ioc = kmalloc(tmp, GFP_KERNEL);
	if (!ioc)
		return ERR_PTR(-ENOMEM);
	if (__copy_from_user(ioc, u_ioc, tmp)) {
		kfree(ioc);
		return ERR_PTR(-EFAULT);
	}
	return ioc;
}

static long
spidev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int			err = 0;
	int			retval = 0;
	struct spidev_data	*spidev;
	struct spi_device	*spi;
	u32			tmp;
	unsigned		n_ioc;
	struct spi_ioc_transfer	*ioc;

	/* Check type and command number */
	if (_IOC_TYPE(cmd) != SPI_IOC_MAGIC)  // 判断控制命令的类型
		return -ENOTTY;

	/* Check access direction once here; don't repeat below.
	 * IOC_DIR is from the user perspective, while access_ok is
	 * from the kernel perspective; so they look reversed.
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)  //判断控制命令的方向是否为读
		err = !access_ok(VERIFY_WRITE,
				(void __user *)arg, _IOC_SIZE(cmd));  //判断传输数据大小
	if (err == 0 && _IOC_DIR(cmd) & _IOC_WRITE)  //判断控制命令的方向是否为写
		err = !access_ok(VERIFY_READ,
				(void __user *)arg, _IOC_SIZE(cmd));//判断传输数据大小
	if (err)
		return -EFAULT;

	/* guard against device removal before, or while,
	 * we issue this ioctl.
	 */
	spidev = filp->private_data;   //从文件私有数据中获取 spidev_data
	spin_lock_irq(&spidev->spi_lock);
	spi = spi_dev_get(spidev->spi);  //获取设备
	spin_unlock_irq(&spidev->spi_lock);

	if (spi == NULL)
		return -ESHUTDOWN;

	/* use the buffer lock here for triple duty:
	 *  - prevent I/O (from us) so calling spi_setup() is safe;
	 *  - prevent concurrent SPI_IOC_WR_* from morphing
	 *    data fields while SPI_IOC_RD_* reads them;
	 *  - SPI_IOC_MESSAGE needs the buffer locked "normally".
	 */
	mutex_lock(&spidev->buf_lock);

	switch (cmd) {
	/* read requests */
	case SPI_IOC_RD_MODE:  //设置spi读模式，
		retval = __put_user(spi->mode & SPI_MODE_MASK,
					(__u8 __user *)arg);
		break;
	case SPI_IOC_RD_MODE32:
		retval = __put_user(spi->mode & SPI_MODE_MASK,
					(__u32 __user *)arg);
		break;
	case SPI_IOC_RD_LSB_FIRST:  //设置spi读最低有效位
		retval = __put_user((spi->mode & SPI_LSB_FIRST) ?  1 : 0,
					(__u8 __user *)arg);
		break;
	case SPI_IOC_RD_BITS_PER_WORD:  //设置spi读每个字含多少位
		retval = __put_user(spi->bits_per_word, (__u8 __user *)arg);
		break;
	case SPI_IOC_RD_MAX_SPEED_HZ:  //设置 spi 读最大速度
		retval = __put_user(spidev->speed_hz, (__u32 __user *)arg);
		break;

	/* write requests */
	case SPI_IOC_WR_MODE:
	case SPI_IOC_WR_MODE32:  //设置spi 写模式
		if (cmd == SPI_IOC_WR_MODE)
			retval = __get_user(tmp, (u8 __user *)arg);
		else
			retval = __get_user(tmp, (u32 __user *)arg);
		if (retval == 0) {
			u32	save = spi->mode;  //获取spi设备模式

			if (tmp & ~SPI_MODE_MASK) {
				retval = -EINVAL;
				break;
			}

			tmp |= spi->mode & ~SPI_MODE_MASK;
			spi->mode = (u16)tmp;
			retval = spi_setup(spi);  //配置 spi 设备
			if (retval < 0)
				spi->mode = save;
			else
				dev_dbg(&spi->dev, "spi mode %x\n", tmp);
		}
		break;
	case SPI_IOC_WR_LSB_FIRST:  //设置 spi 写最低有效位
		retval = __get_user(tmp, (__u8 __user *)arg);
		if (retval == 0) {
			u32	save = spi->mode;  //获取 spi 设备模式

			if (tmp)
				spi->mode |= SPI_LSB_FIRST;
			else
				spi->mode &= ~SPI_LSB_FIRST;
			retval = spi_setup(spi);  //配置 spi 设备
			if (retval < 0)
				spi->mode = save;
			else
				dev_dbg(&spi->dev, "%csb first\n",
						tmp ? 'l' : 'm');
		}
		break;
	case SPI_IOC_WR_BITS_PER_WORD:  //设备spi写每个字含多少个位
		retval = __get_user(tmp, (__u8 __user *)arg);
		if (retval == 0) {
			u8	save = spi->bits_per_word;  //获取spi设备每个字含多少位

			spi->bits_per_word = tmp;  //更新新的spi设备， 每个字含多少个位
			retval = spi_setup(spi);  //配置spi设备
			if (retval < 0)
				spi->bits_per_word = save;
			else
				dev_dbg(&spi->dev, "%d bits per word\n", tmp);
		}
		break;
	case SPI_IOC_WR_MAX_SPEED_HZ:  //设置 spi 写 最大速度
		retval = __get_user(tmp, (__u32 __user *)arg);  //用户空间获取数据
		if (retval == 0) {
			u32	save = spi->max_speed_hz; //获取spi 设备的最大速度

			spi->max_speed_hz = tmp;  //更新新的spi设备最大速度
			retval = spi_setup(spi);  //配置设备
			if (retval >= 0)
				spidev->speed_hz = tmp;
			else
				dev_dbg(&spi->dev, "%d Hz (max)\n", tmp);
			spi->max_speed_hz = save;
		}
		break;

	default:
		/* segmented and/or full-duplex I/O request */
		/* Check message and copy into scratch area */
		ioc = spidev_get_ioc_message(cmd,
				(struct spi_ioc_transfer __user *)arg, &n_ioc);
		if (IS_ERR(ioc)) {
			retval = PTR_ERR(ioc);
			break;
		}
		if (!ioc)
			break;	/* n_ioc is also 0 */

		/* translate to spi_message, execute */
		retval = spidev_message(spidev, ioc, n_ioc);  //处理消息
		kfree(ioc);
		break;
	}

	mutex_unlock(&spidev->buf_lock);
	spi_dev_put(spi);
	return retval;
}

#ifdef CONFIG_COMPAT
static long
spidev_compat_ioc_message(struct file *filp, unsigned int cmd,
		unsigned long arg)
{
	struct spi_ioc_transfer __user	*u_ioc;
	int				retval = 0;
	struct spidev_data		*spidev;
	struct spi_device		*spi;
	unsigned			n_ioc, n;
	struct spi_ioc_transfer		*ioc;

	u_ioc = (struct spi_ioc_transfer __user *) compat_ptr(arg);
	if (!access_ok(VERIFY_READ, u_ioc, _IOC_SIZE(cmd)))
		return -EFAULT;

	/* guard against device removal before, or while,
	 * we issue this ioctl.
	 */
	spidev = filp->private_data;
	spin_lock_irq(&spidev->spi_lock);
	spi = spi_dev_get(spidev->spi);
	spin_unlock_irq(&spidev->spi_lock);

	if (spi == NULL)
		return -ESHUTDOWN;

	/* SPI_IOC_MESSAGE needs the buffer locked "normally" */
	mutex_lock(&spidev->buf_lock);

	/* Check message and copy into scratch area */
	ioc = spidev_get_ioc_message(cmd, u_ioc, &n_ioc);
	if (IS_ERR(ioc)) {
		retval = PTR_ERR(ioc);
		goto done;
	}
	if (!ioc)
		goto done;	/* n_ioc is also 0 */

	/* Convert buffer pointers */
	for (n = 0; n < n_ioc; n++) {
		ioc[n].rx_buf = (uintptr_t) compat_ptr(ioc[n].rx_buf);
		ioc[n].tx_buf = (uintptr_t) compat_ptr(ioc[n].tx_buf);
	}

	/* translate to spi_message, execute */
	retval = spidev_message(spidev, ioc, n_ioc);
	kfree(ioc);

done:
	mutex_unlock(&spidev->buf_lock);
	spi_dev_put(spi);
	return retval;
}

static long
spidev_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	if (_IOC_TYPE(cmd) == SPI_IOC_MAGIC
			&& _IOC_NR(cmd) == _IOC_NR(SPI_IOC_MESSAGE(0))
			&& _IOC_DIR(cmd) == _IOC_WRITE)
		return spidev_compat_ioc_message(filp, cmd, arg);

	return spidev_ioctl(filp, cmd, (unsigned long)compat_ptr(arg));
}
#else
#define spidev_compat_ioctl NULL
#endif /* CONFIG_COMPAT */

static int spidev_open(struct inode *inode, struct file *filp)
{
	struct spidev_data	*spidev;
	int			status = -ENXIO;

	mutex_lock(&device_list_lock);

	list_for_each_entry(spidev, &device_list, device_entry) {  //遍历 device_list
		if (spidev->devt == inode->i_rdev) {  //判断设备号找到对应的设备
			status = 0; 
			break;
		}
	}

	if (status) {  //未找到对应的设备
		pr_debug("spidev: nothing for minor %d\n", iminor(inode));
		goto err_find_dev;
	}

	if (!spidev->tx_buffer) {
		spidev->tx_buffer = kmalloc(bufsiz, GFP_KERNEL);
		if (!spidev->tx_buffer) {
			dev_dbg(&spidev->spi->dev, "open/ENOMEM\n");
			status = -ENOMEM;
			goto err_find_dev;
		}
	}

	if (!spidev->rx_buffer) {
		spidev->rx_buffer = kmalloc(bufsiz, GFP_KERNEL);
		if (!spidev->rx_buffer) {
			dev_dbg(&spidev->spi->dev, "open/ENOMEM\n");
			status = -ENOMEM;
			goto err_alloc_rx_buf;
		}
	}

	spidev->users++;  //设备使用者计数增加
	filp->private_data = spidev;  //spidev_data 放在文件的私有数据里
	nonseekable_open(inode, filp);  //设置文件的打开模式，

	mutex_unlock(&device_list_lock);
	return 0;

err_alloc_rx_buf:
	kfree(spidev->tx_buffer);
	spidev->tx_buffer = NULL;
err_find_dev:
	mutex_unlock(&device_list_lock);
	return status;
}

static int spidev_release(struct inode *inode, struct file *filp)
{
	struct spidev_data	*spidev;

	mutex_lock(&device_list_lock);
	spidev = filp->private_data;  //获取数据
	filp->private_data = NULL; //清空文件按的私有数据

	/* last close? */
	spidev->users--;  //设备用户数减少
	if (!spidev->users) {  //如果使用者个数为0 
		int		dofree;

		kfree(spidev->tx_buffer);  //释放发送缓存
		spidev->tx_buffer = NULL;

		kfree(spidev->rx_buffer);  //释放接收缓存
		spidev->rx_buffer = NULL;

		spin_lock_irq(&spidev->spi_lock);
		if (spidev->spi)
			spidev->speed_hz = spidev->spi->max_speed_hz;

		/* ... after we unbound from the underlying device? */
		dofree = (spidev->spi == NULL);
		spin_unlock_irq(&spidev->spi_lock);

		if (dofree)
			kfree(spidev);
	}
	mutex_unlock(&device_list_lock);

	return 0;
}

static const struct file_operations spidev_fops = {
	.owner =	THIS_MODULE,
	/* REVISIT switch to aio primitives, so that userspace
	 * gets more complete API coverage.  It'll simplify things
	 * too, except for the locking.
	 */
	.write =	spidev_write,
	.read =		spidev_read,
	.unlocked_ioctl = spidev_ioctl,
	.compat_ioctl = spidev_compat_ioctl,
	.open =		spidev_open,
	.release =	spidev_release,
	.llseek =	no_llseek,  //文件指针移动， no_llseek 表示没有移动
};

/*-------------------------------------------------------------------------*/

/* The main reason to have this class is to make mdev/udev create the
 * /dev/spidevB.C character device nodes exposing our userspace API.
 * It also simplifies memory management.
 */

static struct class *spidev_class;

/*-------------------------------------------------------------------------*/

static int spidev_probe(struct spi_device *spi)
{
	struct spidev_data	*spidev;
	int			status;
	unsigned long		minor;

	//printk(KERN_INFO"%s OK.\n",__func__);	

	/*
	 * spidev should never be referenced in DT without a specific
	 * compatible string, it is a Linux implementation thing
	 * rather than a description of the hardware.
	 */
	
	/* Allocate driver data */
	spidev = kzalloc(sizeof(*spidev), GFP_KERNEL);   //为 spidev_data 分配内存
	if (!spidev)
		return -ENOMEM;

	/* Initialize the driver data */
	spidev->spi = spi;  //设置 spidev->spi  指向 spi
	spin_lock_init(&spidev->spi_lock);
	mutex_init(&spidev->buf_lock);

	INIT_LIST_HEAD(&spidev->device_entry);  //初始化 spidev_data 链表入口

	/* If we can allocate a minor number, hook up this device.
	 * Reusing minors is fine so long as udev or mdev is working.
	 */
	mutex_lock(&device_list_lock);
	minor = find_first_zero_bit(minors, N_SPI_MINORS);  //查找次设备位图，分配次设备号
	if (minor < N_SPI_MINORS) {
		struct device *dev;

		spidev->devt = MKDEV(SPIDEV_MAJOR, minor);  //计算出设备号
		//创建设备 /dev/spidev%d.%d (spidev 总线号，片选号)
		dev = device_create(spidev_class, &spi->dev, spidev->devt,
				    spidev, "spidev%d.%d",
				    spi->master->bus_num, spi->chip_select);
		
		status = PTR_ERR_OR_ZERO(dev);
	} else {
		dev_dbg(&spi->dev, "no minor number available!\n");
		status = -ENODEV;
	}
	if (status == 0) {  //分配次设备号成功
		set_bit(minor, minors);  //更新次设备位图
		list_add(&spidev->device_entry, &device_list); //添加进设备链表
	}
	mutex_unlock(&device_list_lock);

	spidev->speed_hz = spi->max_speed_hz;

	if (status == 0)
		spi_set_drvdata(spi, spidev);
	else
		kfree(spidev);

	return status;
}

static int spidev_remove(struct spi_device *spi)
{
	struct spidev_data	*spidev = spi_get_drvdata(spi);  //根据 spi设备获取spidev_data

	//printk(KERN_INFO"%s OK.\n",__func__);	

	/* make sure ops on existing fds can abort cleanly */
	spin_lock_irq(&spidev->spi_lock); //加自选锁
	spidev->spi = NULL;  //清空 spidev_data->spi 
	spin_unlock_irq(&spidev->spi_lock); //解锁

	/* prevent new opens */
	mutex_lock(&device_list_lock);
	list_del(&spidev->device_entry);  //删除链表入口
	device_destroy(spidev_class, spidev->devt);  //销毁设备
	clear_bit(MINOR(spidev->devt), minors);  //清除次设备位图对应位
	if (spidev->users == 0)  //如果使用者个数 为0 
		kfree(spidev);  //释放 spidev_data 内存
	mutex_unlock(&device_list_lock);

	return 0;
}

static struct spi_driver spidev_spi_driver = {
	.driver = {
		.name =		"spidev",
		//.of_match_table = of_match_ptr(spidev_dt_ids),
		//.acpi_match_table = ACPI_PTR(spidev_acpi_ids),
	},
	.probe =	spidev_probe, //spidev的probe方法(当注册了modalias域为"spidev"的spi设备或板级设备,则会调用probe方法) 
	.remove =	spidev_remove,

	/* NOTE:  suspend/resume methods are not necessary here.
	 * We don't do anything except pass the requests to/from
	 * the underlying controller.  The refrigerator handles
	 * most issues; the controller driver handles the rest.
	 */
};

/*-------------------------------------------------------------------------*/

static int  spidev_init(void)  //spidev 接口初始化
{
	int status;

	/* Claim our 256 reserved device numbers.  Then register a class
	 * that will key udev/mdev to add/remove /dev nodes.  Last, register
	 * the driver which manages those device numbers.
	 */
	BUILD_BUG_ON(N_SPI_MINORS > 256);
	status = register_chrdev(SPIDEV_MAJOR, "spi", &spidev_fops);  //注册字符设备，主设备号153，捆绑的设备操作函数集为 spidev_fops
	if (status < 0)
		return status;

	spidev_class = class_create(THIS_MODULE, "spidev");  //创建按设备类
	if (IS_ERR(spidev_class)) {
		unregister_chrdev(SPIDEV_MAJOR, spidev_spi_driver.driver.name);
		return PTR_ERR(spidev_class);
	}

	status = spi_register_driver(&spidev_spi_driver); //注册spi设备驱动spidev_spi_driver
	if (status < 0) {
		class_destroy(spidev_class);
		unregister_chrdev(SPIDEV_MAJOR, spidev_spi_driver.driver.name);
	}
	return status;
}
module_init(spidev_init);  //声明

static void  spidev_exit(void)
{
	//printk(KERN_INFO"%s OK.\n",__func__);	

	spi_unregister_driver(&spidev_spi_driver);
	class_destroy(spidev_class);
	unregister_chrdev(SPIDEV_MAJOR, spidev_spi_driver.driver.name);

}
module_exit(spidev_exit);


/*--------------------------------add-----------------------------------*/

static struct spi_board_info spi_slave_info[] = {
	{
		.modalias	   = "spidev",		
		.bus_num	   = 0, 			 
		.max_speed_hz  = 2000000,
		.chip_select   = 0, 
	},

	{
		.modalias	   = "spidev",		
		.bus_num	   = 0, 			 
		.max_speed_hz  = 2000000,
		.chip_select   = 1, 
	},	
};

static struct spi_device *add_device[2];

static int spidevx_register(int chip_select)
{
	//dev
	struct spi_master *master = NULL;
	
	master = spi_busnum_to_master(spi_slave_info->bus_num);
	if (!master) 
		return -EINVAL;
	add_device[chip_select] = spi_new_device(master, &spi_slave_info[chip_select]);
	if (!add_device[chip_select] )
		return -EINVAL;

	//drv
	spidev_init();
	
	return 0;
}

static int spidevx_unregister(int chip_select)
{
	spi_unregister_device(add_device[chip_select]);
	return 0;
}


static int spidevx_open(struct inode *inode, struct file *file) 
{
	return 0;
}
		
static long  spidevx_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct spidevx_info_msg spi_info_msg;
	
	switch(cmd) {
	case SPIDEVX_IOCINIT:
		if (copy_from_user(&spi_info_msg, (struct spidevx_info_msg __user *)arg, sizeof(spi_info_msg)))
		{
			printk(KERN_ERR "SPIDEVX_IOCINIT: copy_from_user() fail.\n");
			return -EINVAL;
		}

		//printk("spi_info_msg.chip_select=%d \n", spi_info_msg.chip_select);
		spidevx_register(spi_info_msg.chip_select);
		break;	
	case SPIDEVX_IOCEXIT:
		if (copy_from_user(&spi_info_msg, (struct spidevx_info_msg __user *)arg, sizeof(spi_info_msg)))
		{
			printk(KERN_ERR "SPIDEVX_IOCEXIT: copy_from_user() fail.\n");
			return -EINVAL;
		}

		//printk("spi_info_msg.chip_select=%d \n", spi_info_msg.chip_select);
		spidevx_unregister(spi_info_msg.chip_select);
		break;
	default:
		ret = -ENOTTY;
		break;
	}
	
	return ret;
}

static int spidevx_release(struct inode *inode, struct file *file) 
{
	return 0;
}

static struct file_operations spidevx_fops = {
	.owner = THIS_MODULE,
    .open = spidevx_open,
	.unlocked_ioctl = spidevx_ioctl,
    .release = spidevx_release,
};

static int spidevx_drv_init(void)  
{  
  	int ret;
	dev_t spidevx_devid;
	
	if(alloc_chrdev_region(&spidevx_devid, 0, 1, "spidevx") < 0)
    {
        printk(KERN_ERR"Unable to alloc_chrdev_region.\n");
        return -EINVAL;
    } 
    spidevx_major = MAJOR(spidevx_devid);
	cdev_init(&spidevx_cdev, &spidevx_fops);        
    ret = cdev_add(&spidevx_cdev, spidevx_devid, 1);
    if (ret < 0)
    {
        printk(KERN_ERR "Unable to cdev_add.\n");
        goto error;
    }
        
    spidevx_class = class_create(THIS_MODULE, "spidevx"); 
    device_create(spidevx_class, NULL, MKDEV(spidevx_major, 0), NULL, "spidevx"); 

	return 0;
error:
    unregister_chrdev_region(MKDEV(spidevx_major, 0), 1);
    return -EINVAL;
	
}  
  
static void spidevx_drv_exit(void)  
{  
	device_destroy(spidevx_class,  MKDEV(spidevx_major, 0));
    class_destroy(spidevx_class);
 
    unregister_chrdev_region(MKDEV(spidevx_major, 0), 1);
    cdev_del(&spidevx_cdev);

	//spidev
	//SPIDEVX_IOCEXIT
	spidev_exit();
}  

module_init(spidevx_drv_init);
module_exit(spidevx_drv_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("hceng <huangcheng.job@foxmail.com>");
MODULE_DESCRIPTION("100ask series board spidev driver.");
MODULE_VERSION("v2.0");

