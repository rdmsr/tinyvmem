#include <stdio.h>
#include <vmem.h>

#define VMEM_ADDR_MIN 0
#define VMEM_ADDR_MAX (~(uintptr_t)0)

int main(void)
{
    Vmem *vm = NULL;
    void *test = NULL;
    int strat = VM_BESTFIT;
    vmem_bootstrap();

    vm = vmem_create("test", (void *)0x1000, 0x1000000, 0x1000, NULL, NULL, NULL, 0, 0);

    test = vmem_xalloc(vm, 0x1000, 0, 0, 0, VMEM_ADDR_MIN, (void *)VMEM_ADDR_MAX, strat);

    vmem_dump(vm);
    printf("xalloc returned: %p\n", test);

    return 0;
}
