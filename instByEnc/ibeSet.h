#ifndef IBESET_H
#define IBESET_H

#include <stdarg.h>

#include "hdeA64.h"
#include <localUtil.h>

#include <regExBin/regexComponents.h>

template <typename fv_rd, typename fv_rn, typename fv_imms, typename fv_immr, typename fv_immLarge>
class regexInst
{
private:
    fv_rd rd;
    fv_rn rn;
    fv_imms imms;
    fv_immr immr;
    fv_immLarge immLarge;
public:
    static int parseAndCreate(uint32_t instruction, regexInst** tempRegexInst)
    {
        hde_t curInst = { 0 };
        regexInst* targInst = 0;
        int result = -1;
        SAFE_BAIL(parseInst(instruction, &curInst) == -1);

        targInst = new targInst<registerFix, registerFix, immediateFix, immediateFix, immediateFix>(
            curInst.rd, curInst.rn, curInst.imms, curInst.immr, curInst.immLarge
        );

        *tempRegexInst = targInst;
        curInst.VAL_SET;
        result = 0;
    fail:
        return result;
    }

    template <typename fv_rd, typename fv_rn, typename fv_imms, typename fv_immr, typename fv_immLarge>
    regexInst::regexInst(fv_rd rd_a, fv_rn rn_a, fv_imms imms_a, fv_immr immr_a, fv_immLarge immLarge_a)
        : rd(rd_a), rn(rn_a), imms(imms_a), immr(immr_a), immLarge(immLarge_a) { };

    regexInst();
};

class instSet
{
private:
    std::vector<regexInst*> instPatternList;
public:
    int findPattern(uint32_t* startAddress, size_t sizeSearch, uint32_t* resultAddr);

};

#endif