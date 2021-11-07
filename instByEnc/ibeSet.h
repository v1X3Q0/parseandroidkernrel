#ifndef IBESET_H
#define IBESET_H

#include <stdarg.h>
#include <list>
#include <map>

#include "hdeA64.h"
#include <localUtil.h>

#include <regExBin/regexComponents.h>

// class instSet;

template <typename fv_rd, typename fv_rn, typename fv_imms, typename fv_immr, typename fv_immLarge>
class regexInst
{
private:
// opcode? how to do it.....
    fv_rd* rd;
    fv_rn* rn;
    fv_imms* imms;
    fv_immr* immr;
    fv_immLarge* immLarge;

public:
    static int parseAndCreate(uint32_t instruction,
        regexInst** tempRegexInst)
    {
        hde_t curInst = { 0 };
        regexInst* targInst = 0;
        int result = -1;
        SAFE_BAIL(parseInst(instruction, &curInst) == -1);

        targInst = new regexInst(
            new fv_rd(curInst.rd), new fv_rn(curInst.rn), new fv_imms(curInst.imms),
            new fv_immr(curInst.immr), new fv_immLarge(curInst.immLarge)
        );

        *tempRegexInst = targInst;
        curInst.VAL_SET;
        result = 0;
    fail:
        return result;
    }

    friend bool const isEqual(regexInst* lhs, const regexInst<registerFix, registerFix, immediateFix, immediateFix, immediateFix>* rhs)
    {
        bool result = false;

        SAFE_BAIL(lhs->rd->verifyVal(rhs->rd) == false);
        SAFE_BAIL(lhs->rn->verifyVal(rhs->rn) == false);
        SAFE_BAIL(lhs->imms->verifyVal(rhs->imms) == false);
        SAFE_BAIL(lhs->immr->verifyVal(rhs->immr) == false);
        SAFE_BAIL(lhs->immLarge->verifyVal(rhs->immLarge) == false);
        
        result = true;
    fail:
        return result;
    }

    // friend class instSet;
    // friend void insertInst(regexInst* otherOne, instSet* targInstSet)
    // {
    //     targInstSet->instPatternList.push_back(otherOne);
    // }

    // void insertLocal(instSet* targInstSet)
    // {
    //     insertInst(this, targInstSet);
    // }

    bool equalCheck(const regexInst<registerFix, registerFix, immediateFix, immediateFix, immediateFix>* rhs)
    {
        return isEqual(this, rhs);
    }

    bool operator==(const regexInst<registerFix, registerFix, immediateFix, immediateFix, immediateFix>& rhs)
    {
        return equalCheck(&rhs);
    }

    bool operator!=(const regexInst<registerFix, registerFix, immediateFix, immediateFix, immediateFix>& rhs)
    {
        return !equalCheck(&rhs);        
    }

    regexInst(fv_rd* rd_a, fv_rn* rn_a, fv_imms* imms_a, fv_immr* immr_a, fv_immLarge* immLarge_a)
        : rd(rd_a), rn(rn_a), imms(imms_a), immr(immr_a), immLarge(immLarge_a) { };
    regexInst() : rd(0), rn(0), imms(0), immr(0), immLarge(0) {};
};

class instSet
{
private:
    std::map<uint32_t, opcOperandVar*> varTable;
    std::list<regexInst<opcOperand, opcOperand, opcOperand, opcOperand, opcOperand>*> instPatternList;
public:
    bool operator==(const std::list<regexInst<registerFix, registerFix, immediateFix, immediateFix, immediateFix>*>& rhs)
    {
        bool result = true;
        auto ourIt = instPatternList.begin();
        auto it = rhs.begin();

        for (; it != rhs.end(); it++, ourIt++)
        {
            if (**ourIt != **it)
            {
                result = false;
                break;
            }
        }

    fail:
        return result;
    }

    int findPattern(uint32_t* startAddress, size_t sizeSearch, uint32_t** resultAddr);
    opcOperandVar* getVar(uint32_t key);
    opcOperandVar* useKey(uint32_t key);
    
    template <typename fv_rd, typename fv_rn, typename fv_imms, typename fv_immr, typename fv_immLarge>
    void insertInst(regexInst<fv_rd, fv_rn, fv_imms, fv_immr, fv_immLarge>* newInst)
    {
        instPatternList.push_back(newInst);
    }
};

#endif