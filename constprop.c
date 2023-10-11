/*
********************************************************************************
  CONSTPROP.C : IMPLEMENT THE DOWNSTREAM CONSTANT PROPOGATION OPTIMIZATION HERE
*********************************************************************************
*/

#include "constprop.h"

refConst *lastNode, *headNode;
/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO FREE THE CONSTANTS-ASSOCIATED VARIABLES LIST
************************************************************************
*/
bool madeChange = false;

void FreeConstList()
{
   refConst* tmp;
   while (headNode != NULL)
    {
       tmp = headNode;
       headNode = headNode->next;
       free(tmp);
    }

}

/*
*************************************************************************
  FUNCTION TO ADD A CONSTANT VALUE AND THE ASSOCIATED VARIABLE TO THE LIST
**************************************************************************
*/
void UpdateConstList(char* name, long val) {
    refConst* node = malloc(sizeof(refConst));
    if (node == NULL) return;
    node->name = name;
    node->val = val;
    node->next = NULL;
    if (headNode == NULL) {
        lastNode = node;
        headNode = node;
    }
    else {
        lastNode->next = node;
        lastNode = node;
    }
}

/*
*****************************************************************************
  FUNCTION TO LOOKUP IF A CONSTANT ASSOCIATED VARIABLE IS ALREADY IN THE LIST
******************************************************************************
*/
refConst* LookupConstList(char* name) {
    refConst *node;
    node = headNode; 
    while (node != NULL){
        if (!strcmp(name, node->name))
            return node;
        node = node->next;
    }
    return NULL;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/


/*
************************************************************************************
  THIS FUNCTION IS MEANT TO UPDATE THE CONSTANT LIST WITH THE ASSOCIATED VARIABLE
  AND CONSTANT VALUE WHEN ONE IS SEEN. IT SHOULD ALSO PROPOGATE THE CONSTANTS WHEN 
  WHEN APPLICABLE. YOU CAN ADD A NEW FUNCTION IF YOU WISH TO MODULARIZE BETTER.
*************************************************************************************
*/
void TrackConst(NodeList* statements) {
    Node* node;
    while (statements != NULL) {
        node = statements->node;
        if (node->stmtCode == ASSIGN && node->right->exprCode == CONSTANT) {
            // Add the variable and its constant value to the list
            UpdateConstList(node->left->name, node->right->value);
        } else {
            // Look for variables in the expression that have constant values
            // and replace them with their constant values
            if (node->right && node->right->exprCode == VARIABLE) {
                refConst* constInfo = LookupConstList(node->right->name);
                if (constInfo) {
                    node->right->exprCode = CONSTANT;
                    node->right->value = constInfo->val;
                    madeChange = true;  // Indicate that a change was made
                }
            }
        }
        statements = statements->next;
    }
}


bool ConstProp(NodeList* funcdecls) {
    bool localChange = false;
    while (funcdecls != NULL) {
        // Track and propagate constants for each function
        TrackConst(funcdecls->node->statements);
        if (madeChange) {
            localChange = true;
            madeChange = false;  // Reset for the next function
        }
        funcdecls = funcdecls->next;
    }
    return localChange;  // Return true if any change was made across all functions
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
