/* clang-format off */
#define inline 
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <vmem.h>
/* clang-format on */

#define VMEM_ADDR_MIN (void *)0
#define VMEM_ADDR_MAX (void *)(~(uintptr_t)0)

/* We cannot use cmocka's state since it requires C99 */
static Vmem vmem_va;
static Vmem vmem_wired;

static void *internal_allocwired(Vmem *vmem, size_t size, int vmflag)
{
    return vmem_alloc(vmem, size, vmflag);
}

static void internal_freewired(Vmem *vmem, void *ptr, size_t size)
{
    vmem_free(vmem, ptr, size);
}

static void test_vmem_alloc(void **state)
{
    int prev_in_use = vmem_va.stat.in_use;
    void *ret = vmem_alloc(&vmem_va, 0x1000, VM_INSTANTFIT);
    void *ret2 = vmem_alloc(&vmem_va, 0x1000, VM_INSTANTFIT);

    (void)state;

    assert_ptr_equal(ret, (void *)0x1000);
    assert_ptr_equal(ret2, (void *)0x2000);
    assert_int_equal(vmem_va.stat.in_use, prev_in_use + 0x2000);

    vmem_free(&vmem_va, ret, 0x1000);
    vmem_free(&vmem_va, ret2, 0x1000);
}

static void test_vmem_free(void **state)
{
    void *ret = vmem_alloc(&vmem_va, 0x1000, VM_INSTANTFIT);
    int prev_free = vmem_va.stat.free;

    (void)state;

    assert_ptr_not_equal(ret, NULL);

    vmem_free(&vmem_va, ret, 0x1000);

    assert_int_equal(vmem_va.stat.free, prev_free + 0x1000);
}

static void test_vmem_free_coalesce(void **state)
{
    void *ptr1, *ptr2, *ptr3, *ptr4;
    int prev_free;

    (void)state;

    ptr1 = vmem_alloc(&vmem_va, 0x1000, VM_INSTANTFIT);
    ptr2 = vmem_alloc(&vmem_va, 0x1000, VM_INSTANTFIT);
    ptr3 = vmem_alloc(&vmem_va, 0x1000, VM_INSTANTFIT);
    ptr4 = vmem_alloc(&vmem_va, 0x1000, VM_INSTANTFIT);

    prev_free = vmem_va.stat.free;

    vmem_xfree(&vmem_va, ptr2, 0x1000);
    vmem_xfree(&vmem_va, ptr1, 0x1000);
    vmem_xfree(&vmem_va, ptr4, 0x1000);
    vmem_xfree(&vmem_va, ptr3, 0x1000);

    assert_int_equal(vmem_va.stat.free, prev_free + 0x4000);
}

static void test_vmem_imported(void **state)
{
    void *ret = vmem_alloc(&vmem_wired, 0x1000, VM_INSTANTFIT);
    void *ret2 = vmem_alloc(&vmem_wired, 0x1000, VM_INSTANTFIT);

    (void)state;

    assert_ptr_equal(ret, (void *)0x1000);
    assert_ptr_equal(ret2, (void *)0x2000);

    vmem_free(&vmem_wired, ret, 0x1000);
    vmem_free(&vmem_wired, ret2, 0x1000);
}

int vmem_run_tests(void)
{
    int r;
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vmem_alloc),
        cmocka_unit_test(test_vmem_free),
        cmocka_unit_test(test_vmem_free_coalesce),
        cmocka_unit_test(test_vmem_imported),
    };

    vmem_init(&vmem_va, "tests-va", (void *)0x1000, 0x100000, 0x1000, NULL, NULL, NULL, 0, 0);
    vmem_init(&vmem_wired, "tests-wired", 0, 0, 0x1000, internal_allocwired, internal_freewired, &vmem_va, 0, 0);

    r = cmocka_run_group_tests(tests, NULL, NULL);

    vmem_destroy(&vmem_va);
    vmem_destroy(&vmem_wired);

    return r;
}
