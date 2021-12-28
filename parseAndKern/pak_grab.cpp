#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>

#include <krw_util.h>

#include "spare_vmlinux.h"
#include "parseAndKern.h"

int kern_img::grab_sinittext()
{
    int result = -1;
    hde_t tempInst = {0};
    uint32_t* binBegMap = 0;
    
    SAFE_BAIL(live_kern_addr((void*)binBegin, sizeof(*binBegin), (void**)&binBegMap) == -1);
    SAFE_BAIL(parseInst(*binBegMap, &tempInst) == -1);
    _sinittext = (uint32_t*)(tempInst.immLarge + (size_t)binBegin);

    result = 0;
fail:
    SAFE_LIVE_FREE(binBegMap);
    return result;
}

int kern_img::grab_primary_switch()
{
    int result = -1;
    instSet getB;
    uint32_t* primSwitchBAddr = 0;
    size_t primSwitchOff = 0;

    getB.addNewInst(cOperand::createB<saveVar_t>(getB.checkOperand(0)));
    SAFE_BAIL(kernel_search(&getB, _sinittext, PAGE_SIZE, &primSwitchBAddr) == -1);

    getB.getVar(0, &primSwitchOff);
    __primary_switch = (uint32_t*)(primSwitchOff + (size_t)primSwitchBAddr);
    
    result = 0;
fail:
    return result;
}

int kern_img::grab_primary_switched()
{
    int result = -1;
    instSet getB;
    uint32_t* primSwitchedLdrAddr = 0;
    size_t primSwitchOff = 0;
    uint32_t* create_page_tablesAddr = 0;
    size_t create_page_tablesOff = 0;

// the old way,was find a particular branch, resolve the math for an ldr and
// afterwards add it to the found address. changed to resolving an adjacent
// routine and looking for a prologue.
    // size_t tmpMath = 0;

    // operand 1 is the immediate19, operand 2 is the register
    // getB.addNewInst(cOperand::createLDRL<saveVar_t, saveVar_t>(getB.checkOperand(0), getB.checkOperand(1)));
    // SAFE_BAIL(getB.findPattern(__primary_switch_g, PAGE_SIZE, &primSwitchedLdrAddr) == -1);

    // getB.getVar(1, &primSwitchOff);

    // tmpMath = *(size_t*)(primSwitchOff + (size_t)primSwitchedLdrAddr);
    // __primary_switched_g = (uint32_t*)(tmpMath + (size_t)binBegin);

    getB.addNewInst(new cOperand(ARM64_ISB_OP));
    getB.addNewInst(cOperand::createBL<saveVar_t>(getB.checkOperand(0)));
    SAFE_BAIL(kernel_search(&getB, __primary_switch, PAGE_SIZE, &create_page_tablesAddr) == -1);

    getB.getVar(0, &create_page_tablesOff);
    __create_page_tables = (uint32_t*)(create_page_tablesOff + (size_t)create_page_tablesAddr + sizeof(uint32_t));
    
    getB.clearInternals();
    getB.addNewInst(cOperand::createADRP<saveVar_t, saveVar_t>(getB.checkOperand(0), getB.checkOperand(1)));
    getB.addNewInst(cOperand::createASI<size_t, saveVar_t, size_t>(SP, getB.checkOperand(0), 4));
    SAFE_BAIL(kernel_search(&getB, __create_page_tables, PAGE_SIZE, &__primary_switched) == -1);

    result = 0;
fail:
    return result;
}

size_t kern_img::resolveRel(size_t rebase)
{
    return rebase - (size_t)binBegin;
}

int kern_img::grab_start_kernel_g()
{
    int result = -1;
    instSet getB;
    size_t start_kernelOff = 0;

    getB.addNewInst(cOperand::createB<saveVar_t>(getB.checkOperand(0)));
    SAFE_BAIL(kernel_search(&getB, __primary_switched, PAGE_SIZE, &start_kernel) == -1);

    getB.getVar(0, &start_kernelOff);
    start_kernel = (uint32_t*)(start_kernelOff + (size_t)start_kernel);

finish:
    result = 0;
fail:
    return result;
}

int kern_img::grab_task_struct_offs()
{
    int result = -1;
    size_t init_task = 0;
    void* init_task_mapped = 0;
    size_t* memberIter = 0;
    size_t pushable_tasks = 0;
    size_t tasks = 0;

    SAFE_BAIL(ksym_dlsym("init_task", &init_task) == -1);
    SAFE_BAIL(live_kern_addr((void*)init_task, PAGE_SIZE, &init_task_mapped) == -1);

    memberIter = (size_t*)init_task_mapped;
    for (int i = 0; i < PAGE_SIZE; i += 8)
    {
        int curIter = i / sizeof(size_t);
        
        if (
            (memberIter[curIter] == memberIter[curIter + 1]) &&
            (memberIter[curIter + 2] == memberIter[curIter + 3]) &&
            (memberIter[curIter] != 0) &&
            (memberIter[curIter + 2] != 0)
            )
        {
            // we are at task->pushable_tasks.prio_list, so the base of a
            // plist_node is at current - 8, the size of prio, plist_node's
            // first member. then subtract the size of another list to get
            // the offset for the tasks structure.
            pushable_tasks = i - sizeof(size_t) * 1;
            tasks = i - sizeof(size_t) * 3;
            goto found;
        }
    }
    goto fail;

found:
    offset_table["task_struct.tasks"] = tasks;
    offset_table["task_struct.pushable_tasks"] = pushable_tasks;

    result = 0;
fail:
    SAFE_LIVE_FREE(init_task_mapped);
    return result;
}