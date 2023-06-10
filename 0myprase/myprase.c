

中断向量表的位置
F:\share\si-project\Linux-4.9.88\arch\arm\kernel\entry-armv.S   最后面
转移异常向量表的过程
start_kernel
	setup_arch
		paging_init
			devicemaps_init
				/*
				 * Allocate the vector page early.
				 */
				vectors = early_alloc(PAGE_SIZE * 2);  //分配两个page的物理页帧   为什么要分配两个page frame呢？
			                                         //这里vectors table和kuser helper函数（内核空间提供的函数，但是用户空间使用）占用了一个page frame，另外异常处理的stub函数占用了另外一个page frame。
				early_trap_init-(vectors);
				/*
				 * Create a mapping for the machine vectors at the high-vectors
				 * location (0xffff0000).  If we aren't using high-vectors, also
				 * create a mapping at the low-vectors virtual address.
				 */
				map.pfn = __phys_to_pfn(virt_to_phys(vectors));
				map.virtual = 0xffff0000;
				map.length = PAGE_SIZE;

内核查找过程：
	Linux-4.9.88/arch/arm$ grep "__vectors_start" * -nr
	vi kernel/vmlinux.lds +882

/*
		* The vectors and stubs are relocatable code, and the
		* only thing that matters is their relative offsets
		*/
__vectors_start = .;
.vectors 0xffff0000 : AT(__vectors_start) {
 *(.vectors)
}
. = __vectors_start + SIZEOF(.vectors);
__vectors_end = .;


vector_irq   的定义;


/*
 * Interrupt dispatcher    //
 */
	vector_stub	irq, IRQ_MODE, 4      //  vector_stub  是一个宏      /减去4，确保返回发生中断之后的那条指令

	.long	__irq_usr			@  0  (USR_26 / USR_32)    //base address + 0
	.long	__irq_invalid			@  1  (FIQ_26 / FIQ_32)
	.long	__irq_invalid			@  2  (IRQ_26 / IRQ_32)
	.long	__irq_svc			@  3  (SVC_26 / SVC_32)     //base address + 12
	.long	__irq_invalid			@  4
	.long	__irq_invalid			@  5
	.long	__irq_invalid			@  6
	.long	__irq_invalid			@  7
	.long	__irq_invalid			@  8
	.long	__irq_invalid			@  9
	.long	__irq_invalid			@  a
	.long	__irq_invalid			@  b
	.long	__irq_invalid			@  c
	.long	__irq_invalid			@  d
	.long	__irq_invalid			@  e
	.long	__irq_invalid			@  f

//---------------------
	.macro	vector_stub, name, mode, correction=0
	.align	5

vector_\name:
	.if \correction
	sub	lr, lr, #\correction    
	.endif
//-------------------------
 vector_stub	irq, IRQ_MODE, 4------------->vector_irq

管理员模式下进入中断的处理：
//F:\share\si-project\Linux-4.9.88\arch\arm\kernel\entry-armv.S--216------------------------------------------
	.align	5
__irq_svc:
	svc_entry  //保存发生中断那一刻的现场保存在内核栈上
	irq_handler     //具体的中断处理，同user mode的处理。

#ifdef CONFIG_PREEMPT   //和preempt相关的处理
	ldr	r8, [tsk, #TI_PREEMPT]		@ get preempt count  用来判断当前是否可以发生抢占  ，如果preempt_count等于0，说明已经具备了抢占的条件，
	ldr	r0, [tsk, #TI_FLAGS]		@ get flags
	teq	r8, #0				@ if preempt count != 0
	movne	r0, #0				@ force flags to 0
	tst	r0, #_TIF_NEED_RESCHED
	blne	svc_preempt
#endif

	svc_exit r5, irq = 1			@ return from exception
 UNWIND(.fnend		)
ENDPROC(__irq_svc)
//-------------------------------------

用户模式下进入中断的处理：
//F:\share\si-project\Linux-4.9.88\arch\arm\kernel\entry-armv.S--460----------------------------
	.align	5
__irq_usr:
	usr_entry                //保存用户现场的描述
	kuser_cmpxchg_check
	irq_handler             //核心处理内容
	get_thread_info tsk   //tsk是r9，指向当前的thread info数据结构
	mov	why, #0           //why就是r8		  why其实就是r8寄存器，用来传递参数的，表示本次放回用户空间相关的系统调用是哪个？中断处理这个场景和系统调用无关，因此设定为0。
	b	ret_to_user_from_irq   //中断返回，
 UNWIND(.fnend		)
ENDPROC(__irq_usr)

//-------------------------


中断 ---GIC初始化过程

start_kernel (init\main.c)
    init_IRQ (arch\arm\kernel\irq.c)
    	irqchip_init (drivers\irqchip\irqchip.c)
    		of_irq_init (drivers\of\irq.c)
    			desc->irq_init_cb = match->data;

                ret = desc->irq_init_cb(desc->dev,
                            desc->interrupt_parent);



// include\linux\irqchip.h
#define IRQCHIP_DECLARE(name, compat, fn) OF_DECLARE_2(irqchip, name, compat, fn)

#define OF_DECLARE_2(table, name, compat, fn) \
		_OF_DECLARE(table, name, compat, fn, of_init_fn_2)

#define _OF_DECLARE(table, name, compat, fn, fn_type)			\
	static const struct of_device_id __of_table_##name		\
		__used __section(__irqchip_of_table)			\
		 = { .compatible = compat,				\
		     .data = (fn == (fn_type)NULL) ? fn : fn  }


IRQCHIP_DECLARE(cortex_a7_gic, "arm,cortex-a7-gic", gic_of_init);
展开后得到：
static const struct of_device_id __of_table_cortex_a7_gic		\
	__used __section(__irqchip_of_table)			\
	 = { .compatible = "arm,cortex-a7-gic",				\
		 .data = gic_of_init  }		 


----
为设备树节点分配设备 
	of_device_alloc (drivers/of/platform.c)
    dev = platform_device_alloc("", -1);  // 分配 platform_device   
    num_irq = of_irq_count(np);  // 计算中断数    
	
	// drivers/of/irq.c, 根据设备节点中的中断信息, 构造中断资源
	of_irq_to_resource_table(np, res, num_irq) 
        of_irq_to_resource // drivers\of\irq.c
        	int irq = irq_of_parse_and_map(dev, index);  // 获得virq, 中断号

解析设备树映射中断: irq_of_parse_and_map
	
// drivers/of/irq.c, 解析设备树中的中断信息, 保存在of_phandle_args结构体中
of_irq_parse_one(dev, index, &oirq)

// kernel/irq/irqdomain.c, 创建中断映射
irq_create_of_mapping(&oirq);             
	irq_create_fwspec_mapping(&fwspec);
		// 调用irq_domain->ops的translate或xlate,把设备节点里的中断信息解析为hwirq, type
		irq_domain_translate(domain, fwspec, &hwirq, &type)  
		
		// 看看这个hwirq是否已经映射, 如果virq非0就直接返回
		virq = irq_find_mapping(domain, hwirq); 
		
		// 否则创建映射	
		if (irq_domain_is_hierarchy(domain)) {
			// 返回未占用的virq
			// 并用irq_domain->ops->alloc函数设置irq_desc
			virq = irq_domain_alloc_irqs(domain, 1, NUMA_NO_NODE, fwspec);
			if (virq <= 0)
				return 0;
		} else {
			/* Create mapping */
			// 返回未占用的virq
			// 并通过irq_domain_associate调用irq_domain->ops->map设置irq_desc
			virq = irq_create_mapping(domain, hwirq);
		if (!virq)
				return virq;
		}

解析---gic_of_init		

F:\share\si-project\Linux-4.9.88\arch\arm\kernel\head-common.S

b	start_kernel

//F:\share\si-project\Linux-4.9.88\init\main.c

start_kernel
	set_task_stack_end_magic(&init_task) //指向任务堆栈的结构体
		end_of_stack(tsk)  //获取任务堆栈的结束地址
	//F:\share\si-project\Linux-4.9.88\arch\arm\kernel\setup.c
	smp_setup_processor_id
		is_smp //是否使用多CPU系统  imx6ull  使用单CPU系统
		cpu_logical_map(0) = cpu;  --> #define MPIDR_AFFINITY_LEVEL(mpidr, level) \ ((mpidr >> (MPIDR_LEVEL_BITS * level)) & MPIDR_LEVEL_MASK)
			u32 __cpu_logical_map[NR_CPUS] = { [0 ... NR_CPUS-1] = MPIDR_INVALID };
		//F:\share\si-project\Linux-4.9.88\arch\arm\kernel\setup.c   //printf-1--Booting Linux on physical CPU 0x0
	debug_objects_early_init  //对象调试早期初始化
			for (i = 0; i < ODEBUG_HASH_SIZE; i++)
			raw_spin_lock_init;  //原始自旋锁初始化
		for (i = 0; i < ODEBUG_POOL_SIZE; i++)
			hlist_add_head; //链表头初始化
	boot_init_stack_canary
		get_random_bytes  试着得到一个半随机的初值
	cgroup_init_early 
		init_cgroup_root
			init_cgroup_housekeeping
				INIT_LIST_HEAD--(&cgrp->self.sibling);
				INIT_LIST_HEAD--(&cgrp->self.children);
				INIT_LIST_HEAD--(&cgrp->cset_links);
				INIT_LIST_HEAD--(&cgrp->pidlists);
				init_waitqueue_head
			idr_init
				memset--(idp, 0, sizeof(struct idr));
				spin_lock_init--(&idp->lock);
			root->flags = opts->flags;
		RCU_INIT_POINTER--(init_task.cgroups, &init_css_set);   init_css_set  默认的css_set -在任何层次结构被挂载之前， init及其子进程使用。
	local_irq_disable //早期失能中断
	boot_cpu_init // 对于SMP和UP情况，标记启动cpu“present”，“online”等  
	page_address_init
	setup_arch--(&command_line);
		setup_processor
			lookup_processor_type--(read_cpuid_id())
			cpu_name = list->cpu_name;
			__cpu_architecture = __get_cpu_architecture();
	  //F:\share\si-project\Linux-4.9.88\arch\arm\kernel\setup.c       //printf-3--CPU: ARMv7 Processor [410fc075] revision 5 (ARMv7), cr=10c53c7d
			cpuid_init_hwcaps
			patch_aeabi_idiv  //printf-4--CPU: div instructions available: patching division code
			cacheid_init    //printf-5--CPU: PIPT / VIPT nonaliasing data cache, VIPT aliasing instruction cache

		mdesc = setup_machine_fdt--(__atags_pointer); //__atags_pointer 设备树物理地址
				mdesc = of_flat_dt_match_machine--(mdesc_best, arch_get_next_mach);  //获取设备树资源
							//F:\share\si-project\Linux-4.9.88\drivers\of\fdt.c	
					882 -pr_info--("Machine model: %s\n", of_flat_dt_get_machine_name()); //printf-6--OF: fdt:Machine model: Freescale i.MX6 ULL 14x14 EVK Board



	RESERVEDMEM_OF_DECLARE-(cma, "shared-dma-pool", rmem_cma_setup); // 在段属性为  "shared-dma-pool"  的初始化函数  rmem_cma_setup
	解析宏定义如下：   
	static const struct of_device_id __of_table_cma	__used __section(__reservedmem_of_table)= { 
			.compatible = "shared-dma-pool",
			.data = (rmem_cma_setup == (reservedmem_of_init_fn)NULL) ? rmem_cma_setup : rmem_cma_setup  

	//F:\share\si-project\Linux-4.9.88\drivers\base\dma-contiguous.c
		274 -pr_info("Reserved memory: created CMA memory pool at %pa, size %ld MiB\n", //printf-7--Reserved memory: created CMA memory pool at 0x8c000000, size 320 MiB



	paging_init
		build_mem_type_table
		//F:\share\si-project\Linux-4.9.88\arch\arm\mm\mmu.c
		688-pr_info--("Memory policy: %sData cache %s\n",
				ecc_mask ? "ECC enabled, " : "", cp->policy);//printf-9--emory policy: Data cache writealloc


	setup_per_cpu_areas
		pcpu_embed_first_chunk
			F:\share\si-project\Linux-4.9.88\mm\percpu.c
			2050 -pr_info("Embedded %zu pages/cpu @%p s%zu r%zu d%zu u%zu\n",   // printf-10--Embedded 15 pages/cpu @8bb30000 s30156 r8192 d23092 u61440
	
	build_all_zonelists
		5030-pr_info("Built %i zonelists in %s order, mobility grouping %s.  Total pages: %ld\n",   //printf-11--Built 1 zonelists in Zone order, mobility grouping on.  Total pages: 130048

	pidhash_init
		alloc_large_system_hash	
			7056-pr_info("%s hash table entries: %ld (order: %d, %lu bytes)\n",  //printf-13--PID hash table entries: 2048 (order: 1, 8192 bytes)	


	dcache_init_early
		alloc_large_system_hash
			7056-pr_info("%s hash table entries: %ld (order: %d, %lu bytes)\n",  //printf-14-Dentry cache hash table entries: 65536 (order: 6, 262144 bytes)

	inode_init_early		
		alloc_large_system_hash
					7056-pr_info("%s hash table entries: %ld (order: %d, %lu bytes)\n",  //printf-15-Inode-cache hash table entries: 32768 (order: 5, 131072 bytes)
	mem_init
		mem_init_print_info
			6514-pr_info("Memory: %luK/%luK available (%luK kernel code, %luK rwdata, %luK rodata, %luK init, %luK bss, %luK reserved, %luK cma-reserved"
			#ifdef	CONFIG_HIGHMEM  //printf-16--Memory: 172244K/524288K available (11264K kernel code, 1103K rwdata, 4040K rodata, 1024K init, 477K bss, 24364K reserved, 327680K cma-reserved, 0K highmem)
					", %luK highmem"

	mem_init
		504-pr_notice("Virtual kernel memory layout:\n"                     //printf-17--Virtual kernel memory layout:

			
	kmem_cache_init			F:\share\si-project\Linux-4.9.88\mm\slub.c
	4517-pr_info("SLUB: HWalign=%d, Order=%d-%d, MinObjects=%d, CPUs=%d, Nodes=%d\n",  //printf-18--SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
			cache_line_size(),



	rcu_init
		rcu_bootup_announce  //printf-19--Preemptible hierarchical RCU implementation.
		rcu_bootup_announce_oddness  //printf-20--Build-time adjustment of leaf fanout to 32.
									//printf-21--RCU restricting CPUs from NR_CPUS=4 to nr_cpu_ids=1.
		
	rcu_init_geometry
	4159-pr_info("RCU: Adjusting geometry for rcu_fanout_leaf=%d, nr_cpu_ids=%d\n", //printf-22--RCU: Adjusting geometry for rcu_fanout_leaf=32, nr_cpu_ids=1
		rcu_fanout_leaf, nr_cpu_ids);

	early_irq_init
		printk--(KERN_INFO "NR_IRQS:%d nr_irqs:%d %d\n", NR_IRQS, nr_irqs, initcnt); //printf-23--NR_IRQS:16 nr_irqs:16 16


	F:\share\si-project\Linux-4.9.88\include\linux\clocksource.h   #define CLOCKSOURCE_OF_DECLARE-(name, compat, fn) \	OF_DECLARE_1_RET-(clksrc, name, compat, fn)
	CLOCKSOURCE_OF_DECLARE-(imx6ul_timer, "fsl,imx6ul-gpt", imx6dl_timer_init_dt);
	mxc_timer_init_dt
		_mxc_timer_init
			mxc_clocksource_init
				register_current_timer_delay									
					pr_info--("Switching to timer-based delay loop, resolution %lluns\n", res);  //printf-24--Switching to timer-based delay loop, resolution 333ns
	

sched_clock_postinit
	sched_clock_register
		pr_info--("sched_clock: %u bits at %lu%cHz, resolution %lluns, wraps every %lluns\n", //printf-25--sched_clock: 32 bits at 3000kHz, resolution 333ns, wraps every 715827882841ns
			bits, r, r_unit, res, wrap);


__setup-("clocksource=", boot_override_clocksource);


__clocksource_register_scale
	__clocksource_update_freq_scale
		pr_info-("%s: mask: 0x%llx max_cycles: 0x%llx, max_idle_ns: %lld ns\n", //printf-26--clocksource: mxc_timer1: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 637086815595 ns
			cs->name, cs->mask, cs->max_cycles, cs->max_idle_ns);


	pidhash_init	
	pr_info-("pid_max: default: %u minimum: %u\n", pid_max, pid_max_min); //printf-29--pid_max: default: 32768 minimum: 301

	vfs_caches_init
		mnt_init
			mount_hashtable = alloc_large_system_hash("Mount-cache",   //printf-30--Mount-cache hash table entries: 1024 (order: 0, 4096 bytes)
			alloc_large_system_hash
			mountpoint_hashtable = alloc_large_system_hash("Mountpoint-cache", //printf-31--Mountpoint-cache hash table entries: 1024 (order: 0, 4096 bytes)
			alloc_large_system_hash


	check_writebuffer_bugs
		pr_info-("CPU: Testing write buffer coherency: "); //printf-32--CPU: Testing write buffer coherency: ok



	ftrace_init
		pr_info-("ftrace: allocating %ld entries in %ld pages\n",  //printf-33--ftrace: allocating 35227 entries in 104 pages
		count, count / ENTRIES_PER_PAGE + 1);

	rest_init
		kernel_thread-(kernel_init, NULL, CLONE_FS); 
			kernel_init
				kernel_init_freeable
					smp_prepare_cpus
						parse_dt_topology //内核打印验证
								dev_err-(dev->device, "missing clock-frequency property\n"); //printf-34--/cpus/cpu@0 missing clock-frequency property


//到此，内核线程启动了，以后的处理进入进程和线程处理

	secondary_start_kernel
		smp_store_cpu_info
			store_cpu_topology
			pr_info("CPU%u: thread %d, cpu %d, socket %d, mpidr %x\n",  //printf-35--CPU0: thread -1, cpu 0, socket 0, mpidr 80000000
					cpuid, cpu_topology[cpuid].thread_id,

	/*
	 * In most cases loadable modules do not need custom
	 * initcall levels. There are still some valid cases where
	 * a driver may be needed early if built in, and does not
	 * matter when built as a loadable module. Like bus
	 * snooping debug drivers.
	 */
#define early_initcall(fn)		module_init(fn)
#define core_initcall(fn)		module_init(fn)
#define core_initcall_sync(fn)		module_init(fn)
#define postcore_initcall(fn)		module_init(fn)
#define postcore_initcall_sync(fn)	module_init(fn)
#define arch_initcall(fn)		module_init(fn)
#define subsys_initcall(fn)		module_init(fn)
#define subsys_initcall_sync(fn)	module_init(fn)
#define fs_initcall(fn)			module_init(fn)
#define fs_initcall_sync(fn)		module_init(fn)
#define rootfs_initcall(fn)		module_init(fn)
#define device_initcall(fn)		module_init(fn)
#define device_initcall_sync(fn)	module_init(fn)
#define late_initcall(fn)		module_init(fn)
#define late_initcall_sync(fn)		module_init(fn)
	
#define console_initcall(fn)		module_init(fn)
#define security_initcall(fn)		module_init(fn)


进入内核线程调用后，系统根据 initcall_level_names  的等级开始对系统的各个功能进行初始化
	/*This only exists for built-in code, not for modules. Keep main.c:initcall_level_names[] in sync*/
	kernel_init_freeable
		do_basic_setup
			do_initcalls
				do_initcall_level(level);
					do_initcall_level
						parse_args-(initcall_level_names[level], initcall_command_line, __start___param,  
						__stop___param - __start___param, level, level, NULL, &repair_env_string);
	  		//根据level 的值 调用相关的初始化函数		
					for (fn = initcall_levels[level]; fn < initcall_levels[level+1]; fn++)
					do_one_initcall--(*fn);

	__asm__ (
	"msr	cpsr_c, %1\n\t"      //让CPU进入IRQ mode
	"add	r14, %0, %2\n\t"   //r14寄存器保存stk->irq
	"mov	sp, r14\n\t"         //设定IRQ mode的stack为stk->irq
	"msr	cpsr_c, %3\n\t"
	"add	r14, %0, %4\n\t"
	"mov	sp, r14\n\t"         //设定abt mode的stack为stk->abt
	"msr	cpsr_c, %5\n\t"
	"add	r14, %0, %6\n\t"
	"mov	sp, r14\n\t"
	"msr	cpsr_c, %7\n\t"
	"add	r14, %0, %8\n\t"
	"mov	sp, r14\n\t"          //设定und mode的stack为stk->und
	"msr	cpsr_c, %9"        //回到SVC mode
	    :                      //上面是code，下面的output部分是空的
	    : "r" (stk),          //对应上面代码中的%0
	      PLC (PSR_F_BIT | PSR_I_BIT | IRQ_MODE),    //对应上面代码中的%1
	      "I" (offsetof(struct stack, irq[0])),      //对应上面代码中的%2
	      PLC (PSR_F_BIT | PSR_I_BIT | ABT_MODE),    //对应上面代码中的%3
	      "I" (offsetof(struct stack, abt[0])),      //对应上面代码中的%4
	      PLC (PSR_F_BIT | PSR_I_BIT | UND_MODE),    //对应上面代码中的%5
	      "I" (offsetof(struct stack, und[0])),      //对应上面代码中的%6
	      PLC (PSR_F_BIT | PSR_I_BIT | FIQ_MODE),    //对应上面代码中的%7
	      "I" (offsetof(struct stack, fiq[0])),      //对应上面代码中的%8
	      PLC (PSR_F_BIT | PSR_I_BIT | SVC_MODE)     //对应上面代码中的%9
	    : "r14");                               //上面是input操作数列表，r14是要clobbered register列表

	/*嵌入式汇编的语法格式是：
	asm(code : output operand list : input operand list : clobber list);
	。在input operand list中，有两种限制符（constraint），"r"或者"I"，"I"表示立即数（Immediate operands），"r"表示用通用寄存器传递参数。
		clobber list中有一个r14，表示在汇编代码中修改了r14的值，这些信息是编译器需要的内容。

	对于SMP，bootstrap CPU会在系统初始化的时候执行cpu_init函数，进行本CPU的irq、abt和und三种模式的内核栈的设定，
	具体调用序列是：start_kernel--->setup_arch--->setup_processor--->cpu_init。
	对于系统中其他的CPU，bootstrap CPU会在系统初始化的最后，对每一个online的CPU进行初始化，
	具体的调用序列是：start_kernel--->rest_init--->kernel_init--->kernel_init_freeable--->kernel_init_freeable--->smp_init--->cpu_up--->_cpu_up--->__cpu_up。
	__cpu_up函数是和CPU architecture相关的。
	对于ARM，其调用序列是__cpu_up--->boot_secondary--->smp_ops.smp_boot_secondary(SOC相关代码)--->secondary_startup--->__secondary_switched--->secondary_start_kernel--->cpu_init。

	除了初始化，系统电源管理也需要irq、abt和und stack的设定。如果我们设定的电源管理状态在进入sleep的时候，
	CPU会丢失irq、abt和und stack point寄存器的值，那么在CPU resume的过程中，要调用cpu_init来重新设定这些值
	*/

	如何定义一个tasklet？

	#define DECLARE_TASKLET(name, func, data) \
	struct tasklet_struct name = { NULL, 0, ATOMIC_INIT(0), func, data }

	#define DECLARE_TASKLET_DISABLED(name, func, data) \
	struct tasklet_struct name = { NULL, 0, ATOMIC_INIT(1), func, data }

	这两个宏都可以静态定义一个struct tasklet_struct的变量，只不过初始化后的tasklet一个是处于eable状态，一个处于disable状态的。当然，也可以动态分配tasklet，然后调用tasklet_init来初始化该tasklet。

	early_initcall(init_static_idmap);
		init_static_idmap
			identity_mapping_add			
				pr_info-("Setting up static identity map for 0x%lx - 0x%lx\n", addr, end);  //printf-36--Setting up static identity map for 0x80100000 - 0x80100058


	kernel_thread--(kernel_init, NULL, CLONE_FS); 
		kernel_init
			kernel_init_freeable
				smp_init
					smp_announce
						printk-(KERN_INFO "Brought up %d CPUs\n", num_online_cpus());  //printf-37--Brought up 1 CPUs

	kernel_init_freeable
		smp_init
			smp_cpus_done
					423-printk-(KERN_INFO "SMP: Total of %d processors activated "   //printf-38--SMP: Total of 1 processors activated (6.00 BogoMIPS).

	smp_cpus_done
			hyp_mode_check
				pr_info("CPU: All CPU(s) started in SVC mode.\n");   //printf-39--CPU: All CPU(s) started in SVC mode.


	kernel_init_freeable
		do_basic_setup
			driver_init
				devtmpfs_init
					printk(KERN_INFO "devtmpfs: initialized\n"); //printf-40--devtmpfs: initialized

	
	late_initcall-(of_unittest);
		of_unittest
			unittest_data_add
				attach_node_and_children
					__of_attach_node_sysfs
						safe_name
							pr_warn-("Duplicate name in %s, renamed to \"%s\"\n",   //printf-41--OF: Duplicate name in lcdif@021c8000, renamed to "display#1"


	core_initcall(pinctrl_init);
		pinctrl_init
			pr_info-("initialized pinctrl subsystem\n");  //printf-45--pinctrl core: initialized pinctrl subsystem


	fs_initcall(inet_init);
		inet_init
			sock_register
				2467-pr_info-("NET: Registered protocol family %d\n", ops->family);  //printf-46--NET: Registered protocol family 16	


	postcore_initcall(atomic_pool_init);
		atomic_pool_init
			pr_info-("DMA: preallocated %zu KiB pool for atomic coherent allocations\n", //printf-47--DMA: preallocated 256 KiB pool for atomic coherent allocations

	
	postcore_initcall(init_menu);
		init_menu
			cpuidle_register_governor
				cpuidle_switch_governor
					printk(KERN_INFO "cpuidle: using governor %s\n", gov->name); //printf-48--cpuidle: using governor menu

	arch_initcall(arch_hw_breakpoint_init);
		arch_hw_breakpoint_init
			pr_info("found %d " "%s" "breakpoint and %d watchpoint registers.\n",   //printf-49--hw-breakpoint: found 5 (+1 reserved) breakpoint and 4 watchpoint registers.
			pr_info("maximum watchpoint size is %u bytes.\n",   //printf-50--hw-breakpoint: maximum watchpoint size is 8 bytes.

	static struct platform_driver imx6ul_pinctrl_driver = {
		.driver = {
			.name = "imx6ul-pinctrl",
			.of_match_table = of_match_ptr(imx6ul_pinctrl_of_match),
		},
		.probe = imx6ul_pinctrl_probe,
	};
			
	arch_initcall(imx6ul_pinctrl_init);
	imx6ul_pinctrl_probe
		imx_pinctrl_probe
			imx_pinctrl_probe_dt
				imx_pinctrl_parse_functions
					imx_pinctrl_parse_groups
						dev_err-(info->dev, "Invalid fsl,pins property in node %s\n", np->full_name);  //printf-51--imx6ul-pinctrl 20e0000.iomuxc: Invalid fsl,pins property in node /soc/aips-bus@02000000/iomuxc@020e0000/imx6ul-evk/enet1grp


		imx6ul_pinctrl_probe
			imx_pinctrl_probe	
				dev_info-(&pdev->dev, "initialized IMX pinctrl driver\n"); //printf-52--imx6ul-pinctrl 20e0000.iomuxc: initialized IMX pinctrl driver

	subsys_initcall(init_scsi);
		init_scsi
			printk-(KERN_NOTICE "SCSI subsystem initialized\n");  //printf-55--SCSI subsystem initialized


		#define usb_register(driver) usb_register_driver(driver, THIS_MODULE, KBUILD_MODNAME)
			usb_register_driver
				pr_info("%s: registered new interface driver %s\n",  //printf-56--usbcore: registered new interface driver usbfs


	subsys_initcall(usb_init);
		usb_init
			usb_register_device_driver
				pr_info("%s: registered new device driver %s\n",  //printf-58--usbcore: registered new device driver usb


		static struct platform_driver i2c_imx_driver = {
			.probe = i2c_imx_probe,
			.remove = i2c_imx_remove,
			.driver = {
				.name = DRIVER_NAME,
				.pm = I2C_IMX_PM_OPS,
				.of_match_table = i2c_imx_dt_ids,
			},
			.id_table = imx_i2c_devtype,
		};
				
	subsys_initcall(i2c_adap_imx_init);
		i2c_imx_probe
			dev_info(&i2c_imx->adapter.dev, "IMX I2C adapter registered\n");  //printf-59--i2c i2c-0: IMX I2C adapter registered





