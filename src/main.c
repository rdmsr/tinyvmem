#include <test.h>
#include <vmem.h>

int main(void)
{
    vmem_bootstrap();
    return vmem_run_tests();
}
