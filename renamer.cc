#include <iostream>
#include "renamer.h"
#include <assert.h>

using namespace std;

renamer::renamer(uint64_t n_log_regs, uint64_t n_phys_regs, uint64_t n_branches)
{
    assert(n_phys_regs > n_log_regs);
    assert(n_branches >= 1 && n_branches <= 64);

    phyRegCount = n_phys_regs;
    logRegCount = n_log_regs;
    branchStackCount = n_branches;

    // RMT and AMT initialization
    RMT = new uint64_t[logRegCount];
    AMT = new uint64_t[logRegCount];

    for (uint64_t i = 0; i < logRegCount; ++i)
    {
        RMT[i] = i;
        AMT[i] = i;
    }

    //Active list initialization
    activeList = new activeListInstr[phyRegCount - logRegCount];
    for (uint64_t i = 0; i < activeListSize; i++)
    {
        activeList[i] = { 0 };
    }
    activeListHead = 0;
    activeListTail = 0;
    activeListSize = phyRegCount - logRegCount;
    activeListCount = 0;

    // Freelist initialization
    freeListSize = phyRegCount - logRegCount;
    freeList = new uint64_t[freeListSize];
    for (uint64_t i = logRegCount; i < phyRegCount; ++i)
    {
        freeList[i - logRegCount] = i;
    }
    freeListHead = 0;
    freeListTail = freeListSize - 1;
    freeListCount = freeListSize;

    //PRF initialization
    PRF = new uint64_t[phyRegCount];
    phyRegReadyBit = new bool[phyRegCount];

    GBM = 0;

    shadowMaps = new checkpoint_t[branchStackCount];
    for (size_t i = 0; i < branchStackCount; i++)
    {
        shadowMaps[i].valid = 0;
        shadowMaps[i].freeListHead = 0;
        shadowMaps[i].GBM = 0;
    }

    for (uint64_t i = 0; i < phyRegCount; i++)
    {
        if (i >= logRegCount)
            phyRegReadyBit[i] = 0;
        else
            phyRegReadyBit[i] = 1;
        PRF[i] = 0;
    }
}

void renamer::restoreRMT()
{
    for (uint64_t i = 0; i < logRegCount; i++)
    {
        RMT[i] = AMT[i];
    }
}

bool renamer::stall_branch(uint64_t bundle_branch)
{
    uint64_t count = 0;
    for (uint64_t i = 0; i < branchStackCount; i++)
    {
        uint64_t mask = (1 << i);
        if ((mask & GBM) == 0)
        {
            ++count;
        }
    }
    bool retVal = false;
    if (!(count >= bundle_branch))
        retVal = true;

    return retVal;
}

uint64_t renamer::get_branch_mask()
{
    return GBM;
}

bool renamer::stall_reg(uint64_t bundle_dst)
{
    bool retVal;
    uint64_t free;

    if (freeListHead < freeListTail)
    {
        free = freeListTail - freeListHead + 1;
        if (free >= bundle_dst)
            retVal = false;
        else
            retVal = true;
    }
    else if (freeListHead > freeListTail)
    {
        free = 1 + freeListSize - freeListHead + freeListTail;
        if (free >= bundle_dst)
            retVal = false;
        else
            retVal = true;
    }
    else // freeListHead == freeListTail
    {
        if (freeListCount == freeListSize) //All Free Regs are free
            retVal = false;
        else if (freeListCount == 1 && bundle_dst == 1)  // Stall (FULL)
            retVal = true;
    }
    return retVal;
}

bool renamer::stall_dispatch(uint64_t bundle_inst)
{
    bool retVal = false;
    uint64_t space;

    if (activeListTail > activeListHead)
    {
        space = (activeListSize - activeListTail) + activeListHead;
        if (!(space >= bundle_inst))
            retVal = true;
    }
    else if (activeListHead == activeListTail)
    {
        if (!(activeListCount == 0))
            retVal = true;
    }
    else if (activeListTail < activeListHead)
    {
        space = activeListHead - activeListTail;
        if (!(space >= bundle_inst))
            retVal = true;
    }
    return retVal;
}

uint64_t renamer::rename_rsrc(uint64_t log_reg)
{
    return RMT[log_reg];
}

uint64_t renamer::rename_rdst(uint64_t log_reg)
{
    RMT[log_reg] = freeList[freeListHead];
    freeListHead = (1 + freeListHead) % freeListSize;
    --freeListCount;
    return RMT[log_reg];
}

uint64_t renamer::checkpoint()
{
    uint64_t index;
    for (uint64_t i = 0; i < branchStackCount; i++)
    {
        if ((GBM & (1 << i)) == 0)
        {
            index = i;
            break;
        }
    }
    GBM = GBM | (1 << index);
    shadowMaps[index].valid = 1;
    shadowMaps[index].freeListHead = freeListHead;
    shadowMaps[index].GBM = GBM;
    shadowMaps[index].RMT = new uint64_t[logRegCount];
    for (uint64_t i = 0; i < logRegCount; i++)
    {
        shadowMaps[index].RMT[i] = RMT[i];
    }
    return index;
}

uint64_t renamer::dispatch_inst(bool dest_valid, uint64_t log_reg,
        uint64_t phys_reg, bool load, bool store, bool branch, bool amo,
        bool csr, uint64_t PC)
{
    activeListInstr temp = { 0 };
    if (dest_valid)
    {
        temp.isDestValid = 1;
        temp.logicalRegNum = log_reg;
        temp.phyRegNum = phys_reg;
    }
    temp.isInstStore = store;
    temp.isInstBranch = branch;
    temp.progCounter = PC;
    temp.isComplete = 0;
    temp.isLoadVioloation = 0;
    temp.isExecption = 0;
    temp.isAtomic = amo;
    temp.isInstLoad = load;
    temp.isComplete = 0;
    temp.isBranchMispredict = 0;
    temp.isCSR = csr;
    temp.isValMisprediction = 0;

    uint64_t retVal = activeListTail;
    activeList[activeListTail] = temp;
    activeListTail = (1 + activeListTail) % activeListSize;
    activeListCount++;

    return (retVal);

}

bool renamer::is_ready(uint64_t phys_reg)
{
    return (phyRegReadyBit[phys_reg]);
}

void renamer::clear_ready(uint64_t phys_reg)
{

    phyRegReadyBit[phys_reg] = 0;
}

void renamer::set_ready(uint64_t phys_reg)
{
    phyRegReadyBit[phys_reg] = 1;
}

uint64_t renamer::read(uint64_t phys_reg)
{

    return (PRF[phys_reg]);
}

void renamer::write(uint64_t phys_reg, uint64_t value)
{

    PRF[phys_reg] = value;

}

void renamer::set_complete(uint64_t AL_index)
{
    activeList[AL_index].isComplete = 1;
}

void renamer::resolve(uint64_t AL_index, uint64_t branch_ID, bool correct)
{

    if (!correct)
    {
        GBM = shadowMaps[branch_ID].GBM & (~(1 << branch_ID));
        for (uint64_t i = 0; i < logRegCount; i++)
        {
            RMT[i] = shadowMaps[branch_ID].RMT[i];
        }
        delete[] shadowMaps[branch_ID].RMT;

        // Restore to Free List Head from Checkpointed Free List Head
        freeListHead = shadowMaps[branch_ID].freeListHead;

        // Update the Free List count according to restored Free List Head

        if (freeListTail == freeListHead)
        {
            freeListCount = 1;
        }
        else if (freeListTail < freeListHead)
        {
            freeListCount = freeListSize - freeListHead + freeListTail + 1;
        }
        else if (freeListTail > freeListHead)
        {
            freeListCount = freeListTail - freeListHead + 1;
        }

        uint64_t old_tail;
        old_tail = activeListTail;
        activeListTail = (AL_index + 1) % activeListSize;

        if (activeListTail < activeListHead)
        {
            activeListCount = activeListSize - activeListHead + activeListTail;
        }

        else if (activeListTail > activeListHead)
        {
            activeListCount = activeListTail - activeListHead;
        }
        else if (activeListHead == activeListTail)
        {
            if ((activeListCount == activeListSize)
                    && (old_tail == activeListHead))
            {
                // Active List is Full
            }
            else
            {
                activeListCount = 0;
            }
        }

    }
    else
    {
        GBM = GBM & (~(1 << branch_ID));
        for (uint64_t i = 0; i < branchStackCount; i++)
        {
            if (shadowMaps[i].valid)
            {
                shadowMaps[i].GBM = shadowMaps[i].GBM & (~(1 << branch_ID));
            }
        }
    }
    shadowMaps[branch_ID].valid = 0;
}

bool renamer::precommit(bool &completed, bool &exception, bool &load_viol,
        bool &br_misp, bool &val_misp, bool &load, bool &store, bool &branch,
        bool &amo, bool &csr, uint64_t &PC)
{
    bool retVal = false;
    if (!(activeListCount == 0 && activeListHead == activeListTail))
    {
        retVal = true;
        amo = activeList[activeListHead].isAtomic;
        csr = activeList[activeListHead].isCSR;
        load_viol = activeList[activeListHead].isLoadVioloation;
        completed = activeList[activeListHead].isComplete;
        exception = activeList[activeListHead].isExecption;
        store = activeList[activeListHead].isInstStore;
        branch = activeList[activeListHead].isInstBranch;
        PC = activeList[activeListHead].progCounter;
        br_misp = activeList[activeListHead].isBranchMispredict;
        val_misp = activeList[activeListHead].isValMisprediction;
        load = activeList[activeListHead].isInstLoad;
    }
    return retVal;
}

void renamer::commit()
{
    if (activeList[activeListHead].isDestValid)
    {
        freeListTail = (1 + freeListTail) % freeListSize;
        ++freeListCount;
        freeList[freeListTail] = AMT[activeList[activeListHead].logicalRegNum];
        AMT[activeList[activeListHead].logicalRegNum] =
                activeList[activeListHead].phyRegNum;
    }
    uint64_t futureHead;
    futureHead = (1 + activeListHead) % activeListSize;

    if ((activeListCount == 1) && (activeListTail == activeListHead))
    {
        --activeListCount;
    }
    else if (activeListCount == activeListSize)
    {
        --activeListCount;
        activeListHead = futureHead;
    }
    else
    {
        activeListHead = futureHead;
        --activeListCount;
    }
}

void renamer::set_exception(uint64_t AL_index)
{

    activeList[AL_index].isExecption = 1;
}

bool renamer::get_exception(uint64_t AL_index)
{

    return (activeList[AL_index].isExecption);
}

void renamer::squash()
{
    activeListTail = activeListHead;  //Active list recovery
    activeListCount = 0;
    restoreRMT();   // RMT recovery
    freeListHead = freeListTail;
    freeListHead = (freeListHead + 1) % freeListSize;
    freeListCount = freeListSize;
    GBM = 0;
}

void renamer::set_value_misprediction(uint64_t AL_index)
{
    activeList[AL_index].isValMisprediction = 1;
}

void renamer::set_load_violation(uint64_t AL_index)
{
    activeList[AL_index].isLoadVioloation = 1;
}

void renamer::set_branch_misprediction(uint64_t AL_index)
{

    activeList[AL_index].isBranchMispredict = 1;

}
