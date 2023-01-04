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
static Vmem *vmem = NULL;

static void test_vmem_xalloc_no_params(void **state)
{
    int prev_in_use = vmem->stat.in_use;
    void *ret = vmem_xalloc(vmem, 0x1000, 0, 0, 0, VMEM_ADDR_MIN, VMEM_ADDR_MAX, VM_INSTANTFIT);

    (void)state;

    assert_ptr_not_equal(ret, NULL);
    assert_int_equal(vmem->stat.in_use, prev_in_use + 0x1000);

    vmem_xfree(vmem, ret, 0x1000);
}

static void test_vmem_xfree(void **state)
{
    void *ret = vmem_xalloc(vmem, 0x1000, 0, 0, 0, VMEM_ADDR_MIN, VMEM_ADDR_MAX, VM_INSTANTFIT);
    int prev_free = vmem->stat.free;

    (void)state;

    assert_ptr_not_equal(ret, NULL);

    vmem_xfree(vmem, ret, 0x1000);

    assert_int_equal(vmem->stat.free, prev_free + 0x1000);
}

static void test_vmem_xfree_coalesce(void **state)
{
    void *ptr1, *ptr2, *ptr3, *ptr4;
    int prev_free;

    (void)state;

    ptr1 = vmem_xalloc(vmem, 0x1000, 0, 0, 0, VMEM_ADDR_MIN, VMEM_ADDR_MAX, VM_INSTANTFIT);
    ptr2 = vmem_xalloc(vmem, 0x1000, 0, 0, 0, VMEM_ADDR_MIN, VMEM_ADDR_MAX, VM_INSTANTFIT);
    ptr3 = vmem_xalloc(vmem, 0x1000, 0, 0, 0, VMEM_ADDR_MIN, VMEM_ADDR_MAX, VM_INSTANTFIT);
    ptr4 = vmem_xalloc(vmem, 0x1000, 0, 0, 0, VMEM_ADDR_MIN, VMEM_ADDR_MAX, VM_INSTANTFIT);

    prev_free = vmem->stat.free;

    vmem_xfree(vmem, ptr2, 0x1000);
    vmem_xfree(vmem, ptr1, 0x1000);
    vmem_xfree(vmem, ptr4, 0x1000);
    vmem_xfree(vmem, ptr3, 0x1000);

    assert_int_equal(vmem->stat.free, prev_free + 0x4000);
}

int vmem_run_tests(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vmem_xalloc_no_params),
        cmocka_unit_test(test_vmem_xfree),
        cmocka_unit_test(test_vmem_xfree_coalesce),

    };

    vmem = vmem_create("tests", (void *)0x1000, 0x100000, 0x1000, NULL, NULL, NULL, 0, 0);

    return cmocka_run_group_tests(tests, NULL, NULL);
}
