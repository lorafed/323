/*
***********************************************************************
  DEADASSIGN.C : IMPLEMENT THE DEAD CODE ELIMINATION OPTIMIZATION HERE
************************************************************************
*/

#include "deadassign.h"

int change;
refVar *last, *head;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE HEAD AND LAST POINTERS FOR THE REFERENCED 
  VARIABLE LIST.
************************************************************************
*/

void init()
{ 
    head = NULL;
    last = head;
}

/*
***********************************************************************
  FUNCTION TO FREE THE REFERENCED VARIABLE LIST
************************************************************************
*/

void FreeList()
{
   refVar* tmp;
   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO IDENTIFY IF A VARIABLE'S REFERENCE IS ALREADY TRACKED
************************************************************************
*/
bool VarExists(char* name) {
   refVar *node;
   node = head;
   while (node != NULL) {
       if (!strcmp(name, node->name)) {
           return true;
       }
       node = node->next;
    }
    return false;
}

/*
***********************************************************************
  FUNCTION TO ADD A REFERENCE TO THE REFERENCE LIST
************************************************************************
*/
void UpdateRefVarList(char* name) {
    refVar* node = malloc(sizeof(refVar));
    if (node == NULL) return;
    node->name = name;
    node->next = NULL;
    if (head == NULL) {
        last = node;
        head = node;
    }
    else {
        last->next = node;
        last = node;
    }
}

/*
****************************************************************************
  FUNCTION TO PRINT OUT THE LIST TO SEE ALL VARIABLES THAT ARE USED/REFERRED
  AFTER THEIR ASSIGNMENT. YOU CAN USE THIS FOR DEBUGGING PURPOSES OR TO CHECK
  IF YOUR LIST IS GETTING UPDATED CORRECTLY
******************************************************************************
*/
void PrintRefVarList() {
    refVar *node;
    node = head;
    if (node == NULL) {
        printf("\nList is empty"); 
        return;
    }
    while (node != NULL) {
        printf("\t %s", node->name);
        node = node->next;
    }
}

/*
***********************************************************************
  FUNCTION TO UPDATE THE REFERENCE LIST WHEN A VARIABLE IS REFERENCED 
  IF NOT DONE SO ALREADY.
************************************************************************
*/
void UpdateRef(Node* node) {
      if (node->right != NULL && node->right->exprCode == VARIABLE) {
          if (!VarExists(node->right->name)) {
              UpdateRefVarList(node->right->name);
          }
      }
      if (node->left != NULL && node->left->exprCode == VARIABLE) {
          if (!VarExists(node->left->name)) {
              UpdateRefVarList(node->left->name);
          }
      }
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/


/*
********************************************************************
  THIS FUNCTION IS MEANT TO TRACK THE REFERENCES OF EACH VARIABLE
  TO HELP DETERMINE IF IT WAS USED OR NOT LATER
********************************************************************
*/

void TrackRef(Node* funcNode) {
    NodeList* statements = funcNode->statements;
    Node *node;
    while (statements != NULL) {
        node = statements->node;
        switch (node->stmtCode) {
            case ASSIGN:
                // If the right-hand side of the assignment is a variable, it's a reference
                if (node->right && node->right->exprCode == VARIABLE && !VarExists(node->right->name)) {
                    UpdateRefVarList(node->right->name);
                }
                // If the left-hand side of the assignment is a variable (for compound assignments), it's a reference
                if (node->left && node->left->exprCode == VARIABLE && !VarExists(node->left->name)) {
                    UpdateRefVarList(node->left->name);
                }
                break;
            case RETURN:
                // If the return statement returns a variable, it's a reference
                if (node->left && node->left->exprCode == VARIABLE && !VarExists(node->left->name)) {
                    UpdateRefVarList(node->left->name);
                }
                break;
            default:
                break;
        }
        statements = statements->next;
    }
}


/*
***************************************************************
  THIS FUNCTION IS MEANT TO DO THE ACTUAL DEADCODE REMOVAL
  BASED ON THE INFORMATION OF TRACKED REFERENCES
****************************************************************
*/
NodeList* RemoveDead(NodeList* statements) {
    refVar* varNode;
    NodeList *prev = NULL, *tmp, *first = statements;

    while (statements != NULL) {
        if (statements->node->stmtCode == ASSIGN) {
            char* varName = statements->node->left->name;
            if (!VarExists(varName)) {
                // This is dead code. Remove this statement from the list.
                if (prev) {
                    prev->next = statements->next;
                } else {
                    first = statements->next;  // If the first node is being removed.
                }
                tmp = statements;
                statements = statements->next;
                free(tmp);  // Assuming you have a mechanism to deep free these nodes.
                continue;
            }
        }
        prev = statements;
        statements = statements->next;
    }
  
    return first;
}

/*
********************************************************************
  THIS FUNCTION SHOULD ENSURE THAT THE DEAD CODE REMOVAL PROCESS
  OCCURS CORRECTLY FOR ALL THE FUNCTIONS IN THE PROGRAM
********************************************************************
*/
bool DeadAssign(NodeList* funcdecls) {
    bool madeChange = false;
    while (funcdecls != NULL) {
        Node* funcNode = funcdecls->node;

        // Initialize the list of referenced variables
        init();

        // Track variable references within the function
        TrackRef(funcNode);

        // Store the original statements of the function
        NodeList* originalStatements = funcNode->statements;

        // Remove dead code from the function's body
        funcNode->statements = RemoveDead(originalStatements);

        // If changes were made, update the change flag
        if (originalStatements != funcNode->statements) {
            change = 1;
        }

        // Move to the next function declaration
        funcdecls = funcdecls->next;
    }

    if (change == 1) madeChange = true;

    // Reset the change flag for subsequent calls
    change = 0;

    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
 
