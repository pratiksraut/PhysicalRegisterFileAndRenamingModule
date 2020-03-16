#include <inttypes.h>
#include <cassert>
#include <stdio.h>
#include <iostream>

using namespace std;

typedef struct activeListInstr_t
{
    bool isDestValid;
    uint64_t logicalRegNum;
    uint64_t phyRegNum;
    bool isComplete;
    bool isBranchMispredict;
    bool isExecption;
    bool isInstLoad;
    bool isInstStore;
    bool isInstBranch;
    bool isValMisprediction;
    bool isLoadVioloation;
    bool isCSR;
    bool isAtomic;
    uint64_t progCounter;
} activeListInstr;

typedef struct checkpoint
{
    uint64_t *RMT;
    uint64_t freeListHead;
    uint64_t GBM;
    bool valid;
}checkpoint_t;


class renamer
{
private:
    uint64_t phyRegCount;
    uint64_t logRegCount;
    uint64_t branchStackCount;

    uint64_t *RMT;
    uint64_t *AMT;

    uint64_t *freeList;
    uint64_t freeListTail;
    uint64_t freeListHead;
    uint64_t freeListSize;
    uint64_t freeListCount;

    activeListInstr *activeList;
    uint64_t activeListSize;
    uint64_t activeListHead;
    uint64_t activeListTail;
    uint64_t activeListCount;

    uint64_t *PRF;
    bool *phyRegReadyBit;

    uint64_t GBM;

    checkpoint_t *shadowMaps;

public:
    renamer(uint64_t n_log_regs, uint64_t n_phys_regs, uint64_t n_branches);
    bool stall_reg(uint64_t bundle_dst);
    bool stall_branch(uint64_t bundle_branch);
    uint64_t get_branch_mask();
    uint64_t rename_rsrc(uint64_t log_reg);
    uint64_t rename_rdst(uint64_t log_reg);

    uint64_t checkpoint();
    bool stall_dispatch(uint64_t bundle_inst);
    uint64_t dispatch_inst(bool dest_valid, uint64_t log_reg, uint64_t phys_reg,
            bool load, bool store, bool branch, bool amo, bool csr,
            uint64_t PC);
    bool is_ready(uint64_t phys_reg);
    void clear_ready(uint64_t phys_reg);
    void set_ready(uint64_t phys_reg);
    uint64_t read(uint64_t phys_reg);
    void write(uint64_t phys_reg, uint64_t value);
    void set_complete(uint64_t AL_index);
    void resolve(uint64_t AL_index, uint64_t branch_ID, bool correct);
    bool precommit(bool &completed, bool &exception, bool &load_viol,
            bool &br_misp, bool &val_misp, bool &load, bool &store,
            bool &branch, bool &amo, bool &csr, uint64_t &PC);

    void commit();
    void set_exception(uint64_t AL_index);
    bool get_exception(uint64_t AL_index);
    void set_misprediction(uint64_t AL_index);
    void squash();
    void set_value_misprediction(uint64_t AL_index);
    void set_load_violation(uint64_t AL_index);
    void set_branch_misprediction(uint64_t AL_index);
    void restoreRMT();
};
