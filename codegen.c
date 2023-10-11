/*
***********************************************************************
  CODEGEN.C : IMPLEMENT CODE GENERATION HERE
************************************************************************
*/
#include "codegen.h"

int argCounter;
int lastUsedOffset;
char lastOffsetUsed[100];
FILE *fptr;
regInfo *regList, *regHead, *regLast;
varStoreInfo *varList, *varHead, *varLast;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE THE ASSEMBLY FILE WITH FUNCTION DETAILS
************************************************************************
*/
void InitAsm(char* funcName) {
    fprintf(fptr, "\n.globl %s", funcName);
    fprintf(fptr, "\n%s:", funcName); 

    // Init stack and base ptr
    fprintf(fptr, "\npushq %%rbp");  
    fprintf(fptr, "\nmovq %%rsp, %%rbp"); 
}

/*
***************************************************************************
   FUNCTION TO WRITE THE RETURNING CODE OF A FUNCTION IN THE ASSEMBLY FILE
****************************************************************************
*/
void RetAsm() {
    fprintf(fptr,"\npopq  %%rbp");
    fprintf(fptr, "\nretq\n");
} 

/*
***************************************************************************
  FUNCTION TO CONVERT OFFSET FROM LONG TO CHAR STRING 
****************************************************************************
*/
void LongToCharOffset() {
     lastUsedOffset = lastUsedOffset - 8;
     snprintf(lastOffsetUsed, 100,"%d", lastUsedOffset);
     strcat(lastOffsetUsed,"(%rbp)");
}

/*
***************************************************************************
  FUNCTION TO CONVERT CONSTANT VALUE TO CHAR STRING
****************************************************************************
*/
void ProcessConstant(Node* opNode) {
     char value[10];
     LongToCharOffset();
     snprintf(value, 10,"%ld", opNode->value);
     char str[100];
     snprintf(str, 100,"%d", lastUsedOffset);
     strcat(str,"(%rbp)");
     AddVarInfo("", str, opNode->value, true);
     fprintf(fptr, "\nmovq  $%s, %s", value, str);
}

/*
***************************************************************************
  FUNCTION TO SAVE VALUE IN ACCUMULATOR (RAX)
****************************************************************************
*/
void SaveValInRax(char* name) {
    char *tempReg;
    tempReg = GetNextAvailReg(true);
    if (!(strcmp(tempReg, "NoReg"))) {
        LongToCharOffset();
        fprintf(fptr, "\n movq %%rax, %s", lastOffsetUsed);
        UpdateVarInfo(name, lastOffsetUsed, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
    else {
        fprintf(fptr, "\nmovq %%rax, %s", tempReg);
        UpdateRegInfo(tempReg, 0);
        UpdateVarInfo(name, tempReg, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
}



/*
***********************************************************************
  FUNCTION TO ADD VARIABLE INFORMATION TO THE VARIABLE INFO LIST
************************************************************************
*/
void AddVarInfo(char* varName, char* location, long val, bool isConst) {
   varStoreInfo* node = malloc(sizeof(varStoreInfo));
   node->varName = varName;
   node->value = val;
   strcpy(node->location,location);
   node->isConst = isConst;
   node->next = NULL;
   node->prev = varLast;
   if (varHead == NULL) {
       varHead = node;
       varLast = node;;
       varList = node;
   } else {
       //node->prev = varLast;
       varLast->next = node;
       varLast = varLast->next;
   }
   varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO FREE THE VARIABLE INFORMATION LIST
************************************************************************
*/
void FreeVarList()
{  
   varStoreInfo* tmp;
   while (varHead != NULL)
    {  
       tmp = varHead;
       varHead = varHead->next;
       free(tmp);
    }
   varLast = NULL;
}

/*
***********************************************************************
  FUNCTION TO LOOKUP VARIABLE INFORMATION FROM THE VARINFO LIST
************************************************************************
*/
char* LookUpVarInfo(char* name, long val) {
    varList = varLast;
    if (varList == NULL) printf("NULL varlist");
    while (varList != NULL) {
        if (varList->isConst == true) {
            if (varList->value == val) return varList->location;
        }
        else {
            if (!strcmp(name,varList->varName)) return varList->location;
        }
        varList = varList->prev;
    }
    varList = varHead;
    return "";
}

/*
***********************************************************************
  FUNCTION TO UPDATE VARIABLE INFORMATION 
************************************************************************
*/
void UpdateVarInfo(char* varName, char* location, long val, bool isConst) {
  
   if (!(strcmp(LookUpVarInfo(varName, val), ""))) {
       AddVarInfo(varName, location, val, isConst);
   }
   else {
       varList = varHead;
       if (varList == NULL) printf("NULL varlist");
       while (varList != NULL) {
           if (!strcmp(varList->varName,varName)) {
               varList->value = val;
               strcpy(varList->location,location);
               varList->isConst = isConst;
               break;
        }
        varList = varList->next;
       }
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE VARIABLE INFORMATION LIST
************************************************************************
*/
void PrintVarListInfo() {
    varList = varHead;
    if (varList == NULL) printf("NULL varlist");
    while (varList != NULL) {
        if (!varList->isConst) {
            printf("\t %s : %s", varList->varName, varList->location);
        }
        else {
            printf("\t %ld : %s", varList->value, varList->location);
        }
        varList = varList->next;
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO ADD NEW REGISTER INFORMATION TO THE REGISTER INFO LIST
************************************************************************
*/
void AddRegInfo(char* name, int avail) {

   regInfo* node = malloc(sizeof(regInfo));
   node->regName = name;
   node->avail = avail;
   node->next = NULL; 

   if (regHead == NULL) {
       regHead = node;
       regList = node;
       regLast = node;
   } else {
       regLast->next = node;
       regLast = node;
   }
   regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO FREE REGISTER INFORMATION LIST
************************************************************************
*/
void FreeRegList()
{  
   regInfo* tmp;
   while (regHead != NULL)
    {  
       tmp = regHead;
       regHead = regHead->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO UPDATE THE AVAILIBILITY OF REGISTERS IN THE REG INFO LIST
************************************************************************
*/
void UpdateRegInfo(char* regName, int avail) {
    while (regList != NULL) {
        if (regName == regList->regName) {
            regList->avail = avail;
        }
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO RETURN THE NEXT AVAILABLE REGISTER
************************************************************************
*/
char* GetNextAvailReg(bool noAcc) {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        if (regList->avail == 1) {
            if (!noAcc) return regList->regName;
            // if not rax and dont return accumulator set to true, return the other reg
            // if rax and noAcc == true, skip to next avail
            if (noAcc && strcmp(regList->regName, "%rax")) { 
                return regList->regName;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return "NoReg";
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF ANY REGISTER APART FROM OR INCLUDING 
  THE ACCUMULATOR(RAX) IS AVAILABLE
************************************************************************
*/
int IfAvailReg(bool noAcc) {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        if (regList->avail == 1) {
            // registers available
            if (!noAcc) return 1;
            if (noAcc && strcmp(regList->regName, "%rax")) {
                return 1;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return 0;
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF A SPECIFIC REGISTER IS AVAILABLE
************************************************************************
*/
bool IsAvailReg(char* name) {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        if (!strcmp(regList->regName, name)) {
           if (regList->avail == 1) {
               return true;
           } 
        }
        regList = regList->next;
    }
    regList = regHead;
    return false;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE REGISTER INFORMATION
************************************************************************
*/
void PrintRegListInfo() {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        printf("\t %s : %d", regList->regName, regList->avail);
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO CREATE THE REGISTER LIST
************************************************************************
*/
void CreateRegList() {
    AddRegInfo("AX", 1);
    AddRegInfo("BX", 1);
    AddRegInfo("CX", 1);
    AddRegInfo("DX", 1);
    AddRegInfo("SI", 1);
    AddRegInfo("DI", 1);
    AddRegInfo("r8", 1);
    AddRegInfo("r9", 1);
}



/*
***********************************************************************
  THIS FUNCTION IS MEANT TO PUT THE FUNCTION ARGUMENTS ON STACK
************************************************************************
*/
int PushArgOnStack(NodeList* arguments) {
    int offset; // To keep track of where to store the argument on the stack

    while (arguments != NULL) {
        Node* arg = arguments->node;

        // Check if the node type is of a variable or parameter
        if (arg->exprCode == VARIABLE || arg->exprCode == PARAMETER) {
            // Emulate pushing the argument onto the stack
            offset = lastUsedOffset - sizeof(long); // Assuming each argument is of size 'long'
            lastUsedOffset = offset;

            // Store this argument's location in the symbol table or equivalent structure
            char location[10];
            snprintf(location, sizeof(location), "%d(%%rbp)", offset);
            AddVarInfo(arg->name, location, INVAL, false);  // Assuming INVAL indicates uninitialized
        }

        arguments = arguments->next;
    }
    return argCounter;
}



/*
*************************************************************************
  THIS FUNCTION IS MEANT TO GET THE FUNCTION ARGUMENTS FROM THE  STACK
**************************************************************************
*/
void PopArgFromStack(NodeList* arguments) {
    int offset; // To keep track of where to retrieve the argument from on the stack

    while (arguments != NULL) {
        Node* arg = arguments->node;

        // Check if the node type is of a variable or parameter
        if (arg->exprCode == VARIABLE || arg->exprCode == PARAMETER) {
            // Emulate popping the argument off the stack
            offset = lastUsedOffset; // Get the current offset for this argument
            lastUsedOffset = offset + sizeof(long); // Update the lastUsedOffset for the next argument

            // Update this argument's location in the symbol table or equivalent structure
            char location[10];
            snprintf(location, sizeof(location), "%d(%%rbp)", offset);
            UpdateVarInfo(arg->name, location, INVAL, false);  // Assuming INVAL indicates uninitialized
        }

        arguments = arguments->next;
    }
}


/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO PROCESS EACH CODE STATEMENT AND GENERATE 
  ASSEMBLY FOR IT. 
  TIP: YOU CAN MODULARIZE BETTER AND ADD NEW SMALLER FUNCTIONS IF YOU 
  WANT THAT CAN BE CALLED FROM HERE.
 ************************************************************************
 */  
void ProcessStatements(NodeList* statements) {
    while (statements != NULL) {
        Node* stmt = statements->node;
        switch (stmt->stmtCode) {
            case ASSIGN: {
                char* location = ProcessExpression(stmt->right);
                fprintf(fptr, "\tmov %s, %s\n", stmt->name, location);  // moving value to variable's location
                break;
            }
            case RETURN: {
                char* location = ProcessExpression(stmt->left);
                fprintf(fptr, "\tmov rax, %s\n", location);  // moving value to rax for return
                fprintf(fptr, "\tret\n");
                break;
            }
            default:
                fprintf(stderr, "Unknown statement type\n");
                exit(EXIT_FAILURE);
        }
        statements = statements->next;
    }
}


/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO DO CODEGEN FOR ALL THE FUNCTIONS IN THE FILE
 ************************************************************************
*/
void Codegen(NodeList* worklist) {
    fptr = fopen("assembly.s", "w+");
    if (fptr == NULL) {
        printf("\n Could not create assembly file");
        return; 
    }

    while (worklist != NULL) {
        Node* funcNode = worklist->node;

        if (funcNode->type == FUNCTIONDECL) {
            // Start function assembly generation
            InitAsm(funcNode->name);

            // Process the statements of the function
            ProcessStatements(funcNode->statements);

            // End function assembly generation
            RetAsm();
        }

        worklist = worklist->next; 
    }

    fclose(fptr);
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS BELOW THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/
char* ProcessExpression(Node* expr) {
    switch (expr->exprCode) {
        case VARIABLE: {
            // For a variable, simply return its name (assuming it's in the appropriate location)
            return expr->name;
        }
        case CONSTANT: {
            // We will need to move the constant into a register or appropriate location first.
            char* reg = GetNextAvailReg(false);  // Assuming this function fetches the next available register
            fprintf(fptr, "\tmov %s, %ld\n", reg, expr->value);
            return reg;
        }
        case OPERATION: {
            switch (expr->opCode) {
                // Handle each operation type...
                // E.g., for addition:
                case ADD: {
                    char* leftLoc = ProcessExpression(expr->left);
                    char* rightLoc = ProcessExpression(expr->right);
                    fprintf(fptr, "\tadd %s, %s\n", leftLoc, rightLoc);
                    return leftLoc;  // Result of addition is in leftLoc
                }
                // Handle other operation types similarly...
                default:
                    fprintf(stderr, "Unknown operation type\n");
                    exit(EXIT_FAILURE);
            }
            break;
        }
        // Handle other expression types, if any...
        default:
            fprintf(stderr, "Unknown expression type\n");
            exit(EXIT_FAILURE);
    }
    return NULL;  // Should not reach here
}
/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/


