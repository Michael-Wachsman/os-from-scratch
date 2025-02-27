#include "Page.h"

#include <cassert>
#include <iostream>

void VirtualPage::updatePTEs() {
    assert(affiliated.size() > 0);

    bool r, w;
    if (!resident || !physicalPage->clocked) {
        r = false;
        w = false;
    } else if (!isSwap) {
        if (physicalPage->dirty) {
            r = true;
            w = true;
        } else {
            r = true;
            w = false;
        }
    } else {   // We are swap backed
        assert(affiliated.size() == blocks.size());

        if (affiliated.size() == 1) {
            if (physicalPage->dirty) {
                r = true;
                w = true;
            } else {
                r = true;
                w = false;
            }
        } else {
            r = true;
            w = false;
        }
    }

    for (auto PTE : affiliated) {
        PTE->read_enable = r;
        PTE->write_enable = w;
        if (physicalPage != nullptr) {
            PTE->ppage = physicalPage->PPN;
        }
    }
}

// void VirtualPage::setAllRead(bool val) {
//   assert(resident);
//   for (auto &pte : affiliated) {
//     pte->read_enable = val;
//     // CODE DUPLICATION
//     pte->ppage = physicalPage->PPN;
//   }
// }

// void VirtualPage::setAllWrite(bool val) {
//   assert(resident);
//   for (auto &pte : affiliated) {
//     pte->write_enable = val;
//     // CODE DUPLICATION
//     pte->ppage = physicalPage->PPN;
//   }
//}
