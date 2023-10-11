#include "optimizer.h"


void Optimizer(NodeList *funcdecls) {
    bool changesMade;
    do {
        // Reset the change indicator
        changesMade = false;

        // Apply constant folding optimization
        if (ConstantFolding(funcdecls)) {
            changesMade = true;
        }

        // Apply dead code elimination optimization
        if (DeadAssign(funcdecls)) {
            changesMade = true;
        }

        // If either of the optimizations made changes, the loop will run again
    } while (changesMade);
}
