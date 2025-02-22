/* linuxkm_wc_port.h
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

/* included by wolfssl/wolfcrypt/wc_port.h */

#ifndef LINUXKM_WC_PORT_H
#define LINUXKM_WC_PORT_H

    #ifdef HAVE_CONFIG_H
        #ifndef PACKAGE_NAME
            #error wc_port.h included before config.h
        #endif
        /* config.h is autogenerated without gating, and is subject to repeat
         * inclusions, so gate it out here to keep autodetection masking
         * intact:
         */
        #undef HAVE_CONFIG_H
    #endif

    /* suppress inclusion of stdint-gcc.h to avoid conflicts with Linux native
     * include/linux/types.h:
     */
    #define _GCC_STDINT_H
    #define WC_PTR_TYPE uintptr_t

    /* needed to suppress inclusion of stdio.h in wolfssl/wolfcrypt/types.h */
    #define XSNPRINTF snprintf

    /* the rigmarole around kstrtoll() here is to accommodate its
     * warn-unused-result attribute.
     *
     * also needed to suppress inclusion of stdlib.h in
     * wolfssl/wolfcrypt/types.h.
     */
    #define XATOI(s) ({                                 \
          long long _xatoi_res = 0;                     \
          int _xatoi_ret = kstrtoll(s, 10, &_xatoi_res); \
          if (_xatoi_ret != 0) {                        \
            _xatoi_res = 0;                             \
          }                                             \
          (int)_xatoi_res;                              \
        })

    #ifdef BUILDING_WOLFSSL

    #if defined(CONFIG_MIPS) && defined(HAVE_LINUXKM_PIE_SUPPORT)
        /* __ZBOOT__ disables some unhelpful macros around the mem*() funcs in
         * legacy arch/mips/include/asm/string.h
         */
        #define __ZBOOT__
        #define memcmp __builtin_memcmp
        #define __ARCH_MEMCMP_NO_REDIRECT
        #define __ARCH_MEMCPY_NO_REDIRECT
        #define __builtin_memcpy memcpy
        extern void *memcpy(void *dest, const void *src, unsigned int n);
        #define __ARCH_MEMCPY_NO_REDIRECT
        #define __builtin_memset memset
        extern void *memset(void *dest, int c, unsigned int n);
    #endif

    _Pragma("GCC diagnostic push");

    /* we include all the needed kernel headers with these masked out. else
     * there are profuse warnings.
     */
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"");
    _Pragma("GCC diagnostic ignored \"-Wpointer-arith\"");
    _Pragma("GCC diagnostic ignored \"-Wshadow\"");
    _Pragma("GCC diagnostic ignored \"-Wnested-externs\"");
    _Pragma("GCC diagnostic ignored \"-Wredundant-decls\"");
    _Pragma("GCC diagnostic ignored \"-Wsign-compare\"");
    _Pragma("GCC diagnostic ignored \"-Wpointer-sign\"");
    _Pragma("GCC diagnostic ignored \"-Wbad-function-cast\"");
    _Pragma("GCC diagnostic ignored \"-Wdiscarded-qualifiers\"");
    _Pragma("GCC diagnostic ignored \"-Wtype-limits\"");
    _Pragma("GCC diagnostic ignored \"-Wswitch-enum\"");

    #include <linux/kconfig.h>
    #include <linux/kernel.h>
    #include <linux/version.h>
    #include <linux/ctype.h>
    #include <linux/init.h>
    #include <linux/module.h>
    #ifdef __PIE__
        /* without this, mm.h brings in static, but not inline, pmd_to_page(),
         * with direct references to global vmem variables.
         */
        #undef USE_SPLIT_PMD_PTLOCKS
        #define USE_SPLIT_PMD_PTLOCKS 0

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
            /* without this, static show_free_areas() mm.h brings in direct
             * reference to unexported __show_free_areas().
             */
            #define __show_free_areas my__show_free_areas
        #endif
    #endif
    #include <linux/mm.h>
    #ifndef SINGLE_THREADED
        #include <linux/kthread.h>
    #endif
    #include <linux/net.h>
    #include <linux/slab.h>
    #if defined(WOLFSSL_AESNI) || defined(USE_INTEL_SPEEDUP) || defined(WOLFSSL_SP_X86_64_ASM)
        #ifndef CONFIG_X86
            #error X86 SIMD extensions requested, but CONFIG_X86 is not set.
        #endif
        #define WOLFSSL_LINUXKM_SIMD
        #define WOLFSSL_LINUXKM_SIMD_X86
        #if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
            #include <asm/i387.h>
        #else
            #include <asm/simd.h>
        #endif
        #ifdef LINUXKM_SIMD_IRQ
            #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
                #include <asm/fpu/internal.h>
            #endif
            #if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
                #error LINUXKM_SIMD_IRQ is unavailable on linux >= 5.16 (missing exports around fpregs)
                /*
                 * #include <asm/fpu/sched.h>
                 * #include <asm/fpu/signal.h>
                 */
            #endif
        #endif
        #ifndef SAVE_VECTOR_REGISTERS
            #define SAVE_VECTOR_REGISTERS(fail_clause) { int _svr_ret = save_vector_registers_x86(); if (_svr_ret != 0) { fail_clause } }
        #endif
        #ifndef RESTORE_VECTOR_REGISTERS
            #define RESTORE_VECTOR_REGISTERS() restore_vector_registers_x86()
        #endif
    #elif defined(WOLFSSL_ARMASM) || defined(WOLFSSL_SP_ARM32_ASM) || \
          defined(WOLFSSL_SP_ARM64_ASM) || defined(WOLFSSL_SP_ARM_THUMB_ASM) ||\
          defined(WOLFSSL_SP_ARM_CORTEX_M_ASM)
        #if !defined(CONFIG_ARM) && !defined(CONFIG_ARM64)
            #error ARM SIMD extensions requested, but CONFIG_ARM* is not set.
        #endif
        #define WOLFSSL_LINUXKM_SIMD
        #define WOLFSSL_LINUXKM_SIMD_ARM
        #include <asm/fpsimd.h>
        #ifndef SAVE_VECTOR_REGISTERS
            #define SAVE_VECTOR_REGISTERS(fail_clause) { int _svr_ret = save_vector_registers_arm(); if (_svr_ret != 0) { fail_clause } }
        #endif
        #ifndef RESTORE_VECTOR_REGISTERS
            #define RESTORE_VECTOR_REGISTERS() restore_vector_registers_arm()
        #endif
        #ifdef LINUXKM_SIMD_IRQ
            #error LINUXKM_SIMD_IRQ is unavailable on ARM (not implemented)
        #endif
    #else
        #ifndef WOLFSSL_NO_ASM
            #define WOLFSSL_NO_ASM
        #endif
    #endif

    _Pragma("GCC diagnostic pop");

    /* the kernel uses -std=c89, but not -pedantic, and makes full use of anon
     * structs/unions, so we should too.
     */
    #define HAVE_ANONYMOUS_INLINE_AGGREGATES 1

    #define NO_THREAD_LS
    #define NO_ATTRIBUTE_CONSTRUCTOR

    /* kvmalloc()/kvfree() and friends added in linux commit a7c3e901 */
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
        #define HAVE_KVMALLOC
    #endif

    #ifdef HAVE_FIPS
        extern int wolfCrypt_FIPS_first(void);
        extern int wolfCrypt_FIPS_last(void);
    #endif

    #if !defined(WOLFCRYPT_ONLY) && !defined(NO_CERTS)
        /* work around backward dependency of asn.c on ssl.c. */
        struct Signer;
        struct Signer *GetCA(void *signers, unsigned char *hash);
        #ifndef NO_SKID
            struct Signer *GetCAByName(void* signers, unsigned char *hash);
        #endif
    #endif

    #if defined(__PIE__) && !defined(USE_WOLFSSL_LINUXKM_PIE_REDIRECT_TABLE)
        #error "compiling -fPIE without PIE support."
    #endif

    #if defined(HAVE_FIPS) && !defined(HAVE_LINUXKM_PIE_SUPPORT)
        #error "FIPS build requires PIE support."
    #endif

    #ifdef USE_WOLFSSL_LINUXKM_PIE_REDIRECT_TABLE

#ifdef CONFIG_MIPS
    #undef __ARCH_MEMCMP_NO_REDIRECT
    #undef memcmp
    extern int memcmp(const void *s1, const void *s2, size_t n);
#endif

    struct wolfssl_linuxkm_pie_redirect_table {
    #ifndef __ARCH_MEMCMP_NO_REDIRECT
        typeof(memcmp) *memcmp;
    #endif
    #ifndef __ARCH_MEMCPY_NO_REDIRECT
        typeof(memcpy) *memcpy;
    #endif
    #ifndef __ARCH_MEMSET_NO_REDIRECT
        typeof(memset) *memset;
    #endif
    #ifndef __ARCH_MEMMOVE_NO_REDIRECT
        typeof(memmove) *memmove;
    #endif
    #ifndef __ARCH_STRCMP_NO_REDIRECT
        typeof(strcmp) *strcmp;
    #endif
    #ifndef __ARCH_STRNCMP_NO_REDIRECT
        typeof(strncmp) *strncmp;
    #endif
    #ifndef __ARCH_STRCASECMP_NO_REDIRECT
        typeof(strcasecmp) *strcasecmp;
    #endif
    #ifndef __ARCH_STRNCASECMP_NO_REDIRECT
        typeof(strncasecmp) *strncasecmp;
    #endif
    #ifndef __ARCH_STRLEN_NO_REDIRECT
        typeof(strlen) *strlen;
    #endif
    #ifndef __ARCH_STRSTR_NO_REDIRECT
        typeof(strstr) *strstr;
    #endif
    #ifndef __ARCH_STRNCPY_NO_REDIRECT
        typeof(strncpy) *strncpy;
    #endif
    #ifndef __ARCH_STRNCAT_NO_REDIRECT
        typeof(strncat) *strncat;
    #endif
        typeof(kstrtoll) *kstrtoll;

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
            typeof(_printk) *_printk;
        #else
            typeof(printk) *printk;
        #endif
        typeof(snprintf) *snprintf;

        const unsigned char *_ctype;

        typeof(kmalloc) *kmalloc;
        typeof(kfree) *kfree;
        typeof(ksize) *ksize;
        typeof(krealloc) *krealloc;
        #ifdef HAVE_KVMALLOC
        typeof(kvmalloc_node) *kvmalloc_node;
        typeof(kvfree) *kvfree;
        #endif
        typeof(is_vmalloc_addr) *is_vmalloc_addr;

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
            typeof(kmalloc_trace) *kmalloc_trace;
        #else
            typeof(kmem_cache_alloc_trace) *kmem_cache_alloc_trace;
            typeof(kmalloc_order_trace) *kmalloc_order_trace;
        #endif

        typeof(get_random_bytes) *get_random_bytes;
        #if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
            typeof(getnstimeofday) *getnstimeofday;
        #elif LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
            typeof(current_kernel_time64) *current_kernel_time64;
        #else
            typeof(ktime_get_coarse_real_ts64) *ktime_get_coarse_real_ts64;
        #endif

        struct task_struct *(*get_current)(void);
        int (*preempt_count)(void);

        #ifdef WOLFSSL_LINUXKM_SIMD_X86
        typeof(irq_fpu_usable) *irq_fpu_usable;
        /* kernel_fpu_begin() replaced by kernel_fpu_begin_mask() in commit e4512289,
         * released in kernel 5.11, backported to 5.4.93
         */
        #ifdef kernel_fpu_begin
            typeof(kernel_fpu_begin_mask) *kernel_fpu_begin_mask;
        #else
            typeof(kernel_fpu_begin) *kernel_fpu_begin;
        #endif
        typeof(kernel_fpu_end) *kernel_fpu_end;

        #ifdef LINUXKM_SIMD_IRQ
            #if LINUX_VERSION_CODE < KERNEL_VERSION(5, 14, 0)
                typeof(copy_fpregs_to_fpstate) *copy_fpregs_to_fpstate;
                typeof(copy_kernel_to_fpregs) *copy_kernel_to_fpregs;
            #elif LINUX_VERSION_CODE < KERNEL_VERSION(5, 16, 0)
                typeof(save_fpregs_to_fpstate) *save_fpregs_to_fpstate;
                typeof(__restore_fpregs_from_fpstate) *__restore_fpregs_from_fpstate;
                typeof(xfeatures_mask_all) *xfeatures_mask_all;
            /*
             * #else
             *  typeof(save_fpregs_to_fpstate) *save_fpregs_to_fpstate;
             *  typeof(restore_fpregs_from_fpstate) *restore_fpregs_from_fpstate;
             *  typeof(fpu_kernel_cfg) *fpu_kernel_cfg;
             */
            #endif
        #endif
        typeof(cpu_number) *cpu_number;
        typeof(nr_cpu_ids) *nr_cpu_ids;

        #endif /* WOLFSSL_LINUXKM_SIMD_X86 */

        typeof(__mutex_init) *__mutex_init;
        #if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
            typeof(mutex_lock_nested) *mutex_lock_nested;
        #else
            typeof(mutex_lock) *mutex_lock;
        #endif
        typeof(mutex_unlock) *mutex_unlock;
        #if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
            typeof(mutex_destroy) *mutex_destroy;
        #endif

        #ifdef HAVE_FIPS
        typeof(wolfCrypt_FIPS_first) *wolfCrypt_FIPS_first;
        typeof(wolfCrypt_FIPS_last) *wolfCrypt_FIPS_last;
        #endif

        #if !defined(WOLFCRYPT_ONLY) && !defined(NO_CERTS)
        typeof(GetCA) *GetCA;
        #ifndef NO_SKID
        typeof(GetCAByName) *GetCAByName;
        #endif
        #endif

        const void *_last_slot;
    };

    extern const struct wolfssl_linuxkm_pie_redirect_table *wolfssl_linuxkm_get_pie_redirect_table(void);

    #ifdef __PIE__

    #ifndef __ARCH_MEMCMP_NO_REDIRECT
        #define memcmp (wolfssl_linuxkm_get_pie_redirect_table()->memcmp)
    #endif
    #ifndef __ARCH_MEMCPY_NO_REDIRECT
        #define memcpy (wolfssl_linuxkm_get_pie_redirect_table()->memcpy)
    #endif
    #ifndef __ARCH_MEMSET_NO_REDIRECT
        #define memset (wolfssl_linuxkm_get_pie_redirect_table()->memset)
    #endif
    #ifndef __ARCH_MEMMOVE_NO_REDIRECT
        #define memmove (wolfssl_linuxkm_get_pie_redirect_table()->memmove)
    #endif
    #ifndef __ARCH_STRCMP_NO_REDIRECT
        #define strcmp (wolfssl_linuxkm_get_pie_redirect_table()->strcmp)
    #endif
    #ifndef __ARCH_STRNCMP_NO_REDIRECT
        #define strncmp (wolfssl_linuxkm_get_pie_redirect_table()->strncmp)
    #endif
    #ifndef __ARCH_STRCASECMP_NO_REDIRECT
        #define strcasecmp (wolfssl_linuxkm_get_pie_redirect_table()->strcasecmp)
    #endif
    #ifndef __ARCH_STRNCASECMP_NO_REDIRECT
        #define strncasecmp (wolfssl_linuxkm_get_pie_redirect_table()->strncasecmp)
    #endif
    #ifndef __ARCH_STRLEN_NO_REDIRECT
        #define strlen (wolfssl_linuxkm_get_pie_redirect_table()->strlen)
    #endif
    #ifndef __ARCH_STRSTR_NO_REDIRECT
        #define strstr (wolfssl_linuxkm_get_pie_redirect_table()->strstr)
    #endif
    #ifndef __ARCH_STRNCPY_NO_REDIRECT
        #define strncpy (wolfssl_linuxkm_get_pie_redirect_table()->strncpy)
    #endif
    #ifndef __ARCH_STRNCAT_NO_REDIRECT
        #define strncat (wolfssl_linuxkm_get_pie_redirect_table()->strncat)
    #endif
    #define kstrtoll (wolfssl_linuxkm_get_pie_redirect_table()->kstrtoll)

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
        #define _printk (wolfssl_linuxkm_get_pie_redirect_table()->_printk)
    #else
        #define printk (wolfssl_linuxkm_get_pie_redirect_table()->printk)
    #endif
    #define snprintf (wolfssl_linuxkm_get_pie_redirect_table()->snprintf)

    #define _ctype (wolfssl_linuxkm_get_pie_redirect_table()->_ctype)

    #define kmalloc (wolfssl_linuxkm_get_pie_redirect_table()->kmalloc)
    #define kfree (wolfssl_linuxkm_get_pie_redirect_table()->kfree)
    #define ksize (wolfssl_linuxkm_get_pie_redirect_table()->ksize)
    #define krealloc (wolfssl_linuxkm_get_pie_redirect_table()->krealloc)
    #define kzalloc(size, flags) kmalloc(size, (flags) | __GFP_ZERO)
    #ifdef HAVE_KVMALLOC
        #define kvmalloc_node (wolfssl_linuxkm_get_pie_redirect_table()->kvmalloc_node)
        #define kvfree (wolfssl_linuxkm_get_pie_redirect_table()->kvfree)
    #endif
    #define is_vmalloc_addr (wolfssl_linuxkm_get_pie_redirect_table()->is_vmalloc_addr)
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
        #define kmalloc_trace (wolfssl_linuxkm_get_pie_redirect_table()->kmalloc_trace)
    #else
        #define kmem_cache_alloc_trace (wolfssl_linuxkm_get_pie_redirect_table()->kmem_cache_alloc_trace)
        #define kmalloc_order_trace (wolfssl_linuxkm_get_pie_redirect_table()->kmalloc_order_trace)
    #endif

    #define get_random_bytes (wolfssl_linuxkm_get_pie_redirect_table()->get_random_bytes)
    #if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
        #define getnstimeofday (wolfssl_linuxkm_get_pie_redirect_table()->getnstimeofday)
    #elif LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
        #define current_kernel_time64 (wolfssl_linuxkm_get_pie_redirect_table()->current_kernel_time64)
    #else
        #define ktime_get_coarse_real_ts64 (wolfssl_linuxkm_get_pie_redirect_table()->ktime_get_coarse_real_ts64)
    #endif

    #undef get_current
    #define get_current (wolfssl_linuxkm_get_pie_redirect_table()->get_current)
    #undef preempt_count
    #define preempt_count (wolfssl_linuxkm_get_pie_redirect_table()->preempt_count)

    #ifdef WOLFSSL_LINUXKM_SIMD_X86
        #define irq_fpu_usable (wolfssl_linuxkm_get_pie_redirect_table()->irq_fpu_usable)
        #ifdef kernel_fpu_begin
            #define kernel_fpu_begin_mask (wolfssl_linuxkm_get_pie_redirect_table()->kernel_fpu_begin_mask)
        #else
            #define kernel_fpu_begin (wolfssl_linuxkm_get_pie_redirect_table()->kernel_fpu_begin)
        #endif
        #define kernel_fpu_end (wolfssl_linuxkm_get_pie_redirect_table()->kernel_fpu_end)
        #ifdef LINUXKM_SIMD_IRQ
            #if LINUX_VERSION_CODE < KERNEL_VERSION(5, 14, 0)
                #define copy_fpregs_to_fpstate (wolfssl_linuxkm_get_pie_redirect_table()->copy_fpregs_to_fpstate)
                #define copy_kernel_to_fpregs (wolfssl_linuxkm_get_pie_redirect_table()->copy_kernel_to_fpregs)
            #elif LINUX_VERSION_CODE < KERNEL_VERSION(5, 16, 0)
                #define save_fpregs_to_fpstate (wolfssl_linuxkm_get_pie_redirect_table()->save_fpregs_to_fpstate)
                #define __restore_fpregs_from_fpstate (wolfssl_linuxkm_get_pie_redirect_table()->__restore_fpregs_from_fpstate)
                #define xfeatures_mask_all (*(wolfssl_linuxkm_get_pie_redirect_table()->xfeatures_mask_all))
            /*
             * #else
             *  #define save_fpregs_to_fpstate (wolfssl_linuxkm_get_pie_redirect_table()->save_fpregs_to_fpstate)
             *  #define restore_fpregs_from_fpstate (wolfssl_linuxkm_get_pie_redirect_table()->restore_fpregs_from_fpstate)
             *  #define fpu_kernel_cfg (*(wolfssl_linuxkm_get_pie_redirect_table()->fpu_kernel_cfg))
             */
            #endif
        #endif
        #define cpu_number (*(wolfssl_linuxkm_get_pie_redirect_table()->cpu_number))
        #define nr_cpu_ids (*(wolfssl_linuxkm_get_pie_redirect_table()->nr_cpu_ids))
    #endif

    #define __mutex_init (wolfssl_linuxkm_get_pie_redirect_table()->__mutex_init)
    #if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
        #define mutex_lock_nested (wolfssl_linuxkm_get_pie_redirect_table()->mutex_lock_nested)
    #else
        #define mutex_lock (wolfssl_linuxkm_get_pie_redirect_table()->mutex_lock)
    #endif
    #define mutex_unlock (wolfssl_linuxkm_get_pie_redirect_table()->mutex_unlock)
    #if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
        #define mutex_destroy (wolfssl_linuxkm_get_pie_redirect_table()->mutex_destroy)
    #endif

    /* per linux/ctype.h, tolower() and toupper() are macros bound to static inlines
     * that use macros that bring in the _ctype global.  for __PIE__, this needs to
     * be masked out.
     */
    #undef tolower
    #undef toupper
    #define tolower(c) (islower(c) ? (c) : ((c) + ('a'-'A')))
    #define toupper(c) (isupper(c) ? (c) : ((c) - ('a'-'A')))

    #if !defined(WOLFCRYPT_ONLY) && !defined(NO_CERTS)
        #define GetCA (wolfssl_linuxkm_get_pie_redirect_table()->GetCA)
        #ifndef NO_SKID
            #define GetCAByName (wolfssl_linuxkm_get_pie_redirect_table()->GetCAByName)
        #endif
    #endif

    #endif /* __PIE__ */

    #endif /* USE_WOLFSSL_LINUXKM_PIE_REDIRECT_TABLE */

#ifdef WOLFSSL_LINUXKM_SIMD

#ifdef WOLFSSL_LINUXKM_SIMD_X86

    extern __must_check int allocate_wolfcrypt_linuxkm_fpu_states(void);
    extern void free_wolfcrypt_linuxkm_fpu_states(void);
    extern __must_check int save_vector_registers_x86(void);
    extern void restore_vector_registers_x86(void);

#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)

    #error kernel module ARM SIMD is not yet tested or usable.

    static WARN_UNUSED_RESULT inline int save_vector_registers_arm(void)
    {
        preempt_disable();
        if (! may_use_simd()) {
            preempt_enable();
            return BAD_STATE_E;
        } else {
            fpsimd_preserve_current_state();
            return 0;
        }
    }
    static inline void restore_vector_registers_arm(void)
    {
        fpsimd_restore_current_state();
        preempt_enable();
    }

#endif

#endif /* WOLFSSL_LINUXKM_SIMD */

    /* remove this multifariously conflicting macro, picked up from
     * Linux arch/<arch>/include/asm/current.h.
     */
    #ifndef WOLFSSL_NEED_LINUX_CURRENT
        #undef current
    #endif

    /* prevent gcc's mm_malloc.h from being included, since it unconditionally
     * includes stdlib.h, which is kernel-incompatible.
     */
    #define _MM_MALLOC_H_INCLUDED

    #ifdef HAVE_KVMALLOC
        #define malloc(x) kvmalloc_node(x, GFP_KERNEL, NUMA_NO_NODE)
        #define free(x) kvfree(x)
        void *lkm_realloc(void *ptr, size_t newsize);
        #define realloc(x, y) lkm_realloc(x, y)
    #else
        #define malloc(x) kmalloc(x, GFP_KERNEL)
        #define free(x) kfree(x)
        #define realloc(x,y) krealloc(x, y, GFP_KERNEL)
    #endif

    /* min() and max() in linux/kernel.h over-aggressively type-check, producing
     * myriad spurious -Werrors throughout the codebase.
     */
    #undef min
    #undef max

    /* work around namespace conflict between wolfssl/internal.h (enum HandShakeType)
     * and linux/key.h (extern int()).
     */
    #define key_update wc_key_update

    #define lkm_printf(format, args...) printk(KERN_INFO "wolfssl: %s(): " format, __func__, ## args)
    #define printf(...) lkm_printf(__VA_ARGS__)

    #ifdef HAVE_FIPS
        extern void fipsEntry(void);
    #endif

    /* suppress false-positive "writing 1 byte into a region of size 0" warnings
     * building old kernels with new gcc:
     */
    #if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
    _Pragma("GCC diagnostic ignored \"-Wstringop-overflow\"");
    #endif

    /* includes are all above, with incompatible warnings masked out. */
    #if LINUX_VERSION_CODE < KERNEL_VERSION(5, 5, 0)
    typedef __kernel_time_t time_t;
    #else
    typedef __kernel_time64_t time_t;
    #endif
    extern time_t time(time_t * timer);
    #define XTIME time
    #define WOLFSSL_GMTIME
    #define XGMTIME(c, t) gmtime(c)
    #define NO_TIMEVAL 1

    #endif /* BUILDING_WOLFSSL */

    /* if BUILDING_WOLFSSL, mutex.h will have already been included recursively
     * above, with the bevy of warnings suppressed, and the below include will
     * be a redundant no-op.
     */
    #include <linux/mutex.h>
    typedef struct mutex wolfSSL_Mutex;

    #define XMALLOC(s, h, t)     ({(void)(h); (void)(t); kmalloc(s, GFP_KERNEL);})
    #define XFREE(p, h, t)       ({void* _xp; (void)(h); _xp = (p); if(_xp) kfree(_xp);})
    #define XREALLOC(p, n, h, t) ({(void)(h); (void)(t); krealloc((p), (n), GFP_KERNEL);})

    #include <linux/limits.h>

    /* Linux headers define these using C expressions, but we need
     * them to be evaluable by the preprocessor, for use in sp_int.h.
     */
    #if BITS_PER_LONG == 64
        _Static_assert(sizeof(ULONG_MAX) == 8,
                       "BITS_PER_LONG is 64, but ULONG_MAX is not.");

        #undef UCHAR_MAX
        #define UCHAR_MAX 255
        #undef USHRT_MAX
        #define USHRT_MAX 65535
        #undef UINT_MAX
        #define UINT_MAX 4294967295U
        #undef ULONG_MAX
        #define ULONG_MAX 18446744073709551615UL
        #undef ULLONG_MAX
        #define ULLONG_MAX ULONG_MAX
        #undef INT_MAX
        #define INT_MAX 2147483647
        #undef LONG_MAX
        #define LONG_MAX 9223372036854775807L
        #undef LLONG_MAX
        #define LLONG_MAX LONG_MAX

    #elif BITS_PER_LONG == 32

        _Static_assert(sizeof(ULONG_MAX) == 4,
                       "BITS_PER_LONG is 32, but ULONG_MAX is not.");

        #undef UCHAR_MAX
        #define UCHAR_MAX 255
        #undef USHRT_MAX
        #define USHRT_MAX 65535
        #undef UINT_MAX
        #define UINT_MAX 4294967295U
        #undef ULONG_MAX
        #define ULONG_MAX 4294967295UL
        #undef INT_MAX
        #define INT_MAX 2147483647
        #undef LONG_MAX
        #define LONG_MAX 2147483647L

        #undef ULLONG_MAX
        #undef LLONG_MAX
        #if BITS_PER_LONG_LONG == 64
            #define ULLONG_MAX 18446744073709551615UL
            #define LLONG_MAX 9223372036854775807L
        #else
            #undef NO_64BIT
            #define NO_64BIT
            #define ULLONG_MAX ULONG_MAX
            #define LLONG_MAX LONG_MAX
        #endif

#else
        #error unexpected BITS_PER_LONG value.
#endif

#endif /* LINUXKM_WC_PORT_H */
