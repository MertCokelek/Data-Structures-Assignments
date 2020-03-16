#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct n{
    char letter, *password;
    struct n* children, *next;
};
typedef struct n node;
node* append(node* root, node* new_node){    /* Dynamically adding a new child to the parent, efficient memory usage. */
    if(root == NULL)                         /* If the node does not have a child currently */
        root = new_node;

    node* iter = root;                       /* Iterate until finding an empty space for the new node */
    while(iter->next != NULL)
        iter = iter-> next;

    iter->next = new_node; new_node->next = NULL;
    return root;
}
node* init(char letter, char* password){                /* for creating a new node. like a constructor. */
    node* newNode = (node*)malloc(sizeof(node));
    newNode->next = NULL;
    newNode->letter = letter;
    newNode->children = NULL;

    newNode->password = (char*)malloc(sizeof(char) * strlen(password));

    if(password[strlen(password)-1] == '\n')        /* if there is a \n at the end of the string, delete it. */
        password[strlen(password)-1] = '\0';
    strcpy(newNode->password, password);

    return newNode;
}
node* childExists(node* parent, char letter){ /* If a child exists in the children of the parent, return that child. */

    node* iter = parent->children;

    while(iter != NULL){

        if(iter->letter == letter)

            return iter;
        iter = iter->next;
    }

    return NULL;
}
void deleteNode(node **parent_ptr, char letter){      /* From the given parent's children, remove 1 node. */
    node* temp = *parent_ptr, *prev = *parent_ptr;   /* This is used in 'Remove' function. */

    if (temp != NULL && temp->letter == letter){
        *parent_ptr = temp->next;
        free(temp);
        return;
    }

    while (temp != NULL && temp->letter != letter){
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) return;
    prev->next = temp->next;
    free(temp);  // Free memory
}
node* add_to_tree(node* root, node* parent, char* name, char* password, int i, int* success){

    if(password[strlen(password)-1] == '\n')    /* if password contaions newline char, remove it. */
        password[strlen(password)-1] = '\0';    /* because it causes problem in login function, passwords don't match.*/

    /* Checking if the parent has a child with the same letter. */
    if(childExists(parent, name[i]) == NULL){               /* If not, add a new child to the parent. */
        if(i < strlen(name)-1){                             /* If not end of the name, keep going deeper. */
            node* newChild = init(name[i], "");             /* The new node to be added */
            parent->children = append(parent->children, newChild);      /* Realloc the children array of the parent, and add the new child. */
            node* tempParent = childExists(parent, name[i]);        /* This holds the parent of the new added node. */
            return add_to_tree(root, tempParent, name, password, i+1, success); /* Since not end of name, recursion.  */
        }

            /* Last letter and password is adding, Base Case. */
        else {
            node* newChild = init(name[i], password);
            parent->children = append(parent->children, newChild); /* Add the new node to the parent's children array. */
            *success = 1; /* If success = 1, printing name added successfully. */
            return root;
        }
    }
    else if(i == strlen(name)-1){                           /* If the last letter is to be added */
        if(childExists(parent, name[i])->password[0] != ""[0]){     /* If the name is completely registered before*/
            *success = 0;       /* Since trying to add the same name twice, succession is failed. */
        } else {
            /* If the new name is a sub string of the registered names. e.g Cemile exists, new element is Cemil.*/
            strcpy(childExists(parent, name[i])->password, password);
            *success = 1;
        }
        return root;
    }
    else {        /* If the parent has a child with the given letter, don't add it, go next letter recursively. */
        node* tempParent = childExists(parent, name[i]);
        return add_to_tree(root, tempParent, name, password, i+1, success);
    }
}
int numberOfChildrenOfNode(node* n){   /* Since the children of a parent are hold dynamically, the number of children of node is not known */
    node* iter = n->children;

    int i = 0;
    while(iter != NULL){
        i++;            /* Until end of the children array, increment i by one every time. */
        iter = iter->next;}

    return i;           /* number of children */
}
char** split(char* line, char* tok, int amount){                      /* Splits a given string into array by a token. */
    int i = 0; int tokenLen = 1001;                                  /*Used for reading files and splitting the lines.*/
    char* p = strtok(line, tok);
    char** array = (char**)malloc(sizeof(char*) * amount);
    for(i = 0; i < amount; i++)
        array[i] = (char*)malloc(sizeof(char) * (int)strlen(p) + tokenLen);
    i = 0;
    while(p != NULL){
        strcpy(array[i++], p);
        p = strtok(NULL, tok);
    }
    return array;
}
int search(node* head, char* name, int i, char** pass) {
    /* The node does not exist at root's children. */
    if (childExists(head, name[i]) == NULL) {
        if (i == 0)
            return 0;
        else
            return 1;
    }
    node* iter = childExists(head, name[i]);    /* iter = iterator node */

    /* End of searching name, there are alternatives */
    if(i == strlen(name)-1){
        if(strcmp(iter->password, "") != 0){
            *pass = iter->password;
            return 3;
        }else
            return 2;
    }
    else
        return search(iter, name, i+1, pass);
}
void login(node* head, char* name, char* password, int i, FILE* output){

    if (childExists(head, name[i]) == NULL) {
        if (i == 0) fprintf(output,"\"%s\" no record\n", name);
        else        fprintf(output,"\"%s\" incorrect username\n", name);
        return;
    }
    node* iter = childExists(head, name[i]);    /* iter = iterator node */
    if(i == strlen(name)-1){                /* End of searching name, there are alternatives */
        if(strcmp(iter->password, password) == 0)
            fprintf(output,"\"%s\" successful login\n", name);
        else if(strcmp(iter->password, "") == 0)
            fprintf(output,"\"%s\" not enough username\n", name);
        else
            fprintf(output,"\"%s\" incorrect password\n", name);
        return;
    }else
        return login(iter, name, password, i+1, output);
}
void Remove(node** root_ref, char name[], char fullName[], int* success){
    node* iter = *root_ref;
    node* parent = *root_ref;
    int i = 0; // index counter for the name
    while(childExists(iter, name[i])){
        parent = iter;
        iter = childExists(iter, name[i]);
        i++;
    }
    /**************************  BASE CASES  ************************************/

    if(strlen(name) == 1 && numberOfChildrenOfNode(iter) > 0){
        if(parent == iter){
            *success = 2;            return;
        }
        if(strcmp(name, fullName) == 0) {
            *success = 4;            return;
        }
        *success = 1;        return;
    }

    if(parent == iter){                             /* None of them have changed, no record */
        *success = 2; return;
    }
    if(parent->letter == (*root_ref)->letter){      /* incorrect username */
        *success = 3; return;
    }
    if(i == strlen(name) && numberOfChildrenOfNode(iter) > 1 && iter->password[0] == '\0'){      /* There are alternatives, Such as selim selma sel */

        *success = 4; return;
    }

    /***************************    RECURSIVE CASES ********************************/

    if(i == strlen(name)){
        if(strcmp(iter->password, "") != 0) {        // iter has a password, last letter
            if(strcmp(name, fullName) == 0){        // Cemile, Cemil -> Cemile is deleted.
                iter->password = "";
                if (numberOfChildrenOfNode(iter) == 0) {                 // iter is  a leaf, we can delete iter from tree.
                    deleteNode(&(parent->children), iter->letter);
                }

                name[i-1] = '\0';
                *success = 1;
                int x;
                return Remove(root_ref, name, fullName, &x);

            }else{                                  // Cemile, Cemil, e is deleted, recursion finished.
                *success = 1; return;
            }


        }else{                                      // Correct letter, Middle element of the name.
            if(numberOfChildrenOfNode(iter) == 0){      // iter is a leaf, remove it.
                deleteNode(&(parent->children), name[i-1]);
                name[i-1] = '\0';
                return Remove(root_ref, name, fullName, success);
            }else if(strcmp(name, fullName) == 0) {
                *success = 4;
                return;
            }else{                                      // Selim, Selin, m deleted, Selin should stay
                    name[i-1] = '\0';
                    return Remove(root_ref, name, fullName, success);
            }
        }
    }else{
        *success = 3; return;
    }
}
char* concat(char* str, char c){        /* concatanates the given char to the given string. */
    size_t len = strlen(str);
    char *str2 = malloc(len + 1 + 1 ); /* one for extra char, one for trailing zero */
    strcpy(str2, str);
    str2[len] = c;
    str2[len + 1] = '\0';
    return str2;
}
void preOrder(node* tree, char* str, int* indent, FILE* output, char* str2){
    int i;

    if(tree != NULL){
        if(numberOfChildrenOfNode(tree) > 1 && *indent == 1){
                str2 = concat(str, tree->letter);
                fprintf(output,"-%s\n", str2);
                *indent = 0;

        }
        if(strcmp(tree->password, "") != 0){    /*  Node has a password*/
            str = concat(str, tree->letter);    /* str is the output, add the node's letter to str. */
            if(str2[0] == str[0])
                fprintf(output, "\t");
            fprintf(output,"-%s\n", str);
            node* iter = tree->children;
            int n = numberOfChildrenOfNode(tree);
            /* For all the children of the parent node, call the function until reaching last letter */
            for(i = 0; i < n; i++){
                preOrder(iter, str, indent, output, str2);
                iter = iter->next;
            }
            /* Node has a password and also children, Like Cemil, Cemile */

        } else{    /* Node does not have a password, so keep going until finding a complete name. */
            str = concat(str, tree->letter);
            node* iter = tree->children;
            /* If a node has more than 2 children, print all of them. */
            for(i = 0; i < numberOfChildrenOfNode(tree); i++){
                preOrder(iter, str, indent, output, str2);
                iter = iter->next;
            }
        }
        strcpy(str, "");       /* Reset the output string to empty string for the next subtree. */
    }
}


node* readInput(char* input_txt, node* trie, char* output_txt){

    FILE* inputFile = fopen(input_txt, "r");
    FILE* output = fopen(output_txt, "w");
    char line[256];             /* Every single line. */
    while(fgets(line, 256, inputFile)) {

        if(line[1] == 'a'){          /****** ADD Command **********/
            char** splitted = split(line, " ", 3);
            int addingSuccess = 0;
            trie = add_to_tree(trie, trie, splitted[1], splitted[2], 0, &addingSuccess);
            if(addingSuccess == 1) fprintf(output,"\"%s\" was added\n", splitted[1]);
            if(addingSuccess == 0) fprintf(output,"\"%s\" reserved username\n", splitted[1]);
        }

        else if(line[1] == 's'){     /****** SEARCH Command *******/
            char** splitted = split(line, " ", 2);
            char* searchingName = (char*)malloc(sizeof(char) * strlen(splitted[1]));


            if(splitted[1][strlen(splitted[1])-1] == '\n')
                splitted[1][strlen(splitted[1])-1] = '\0';



            strcpy(searchingName, splitted[1]);
            char* pass = "";

            /* splitted[1] = searching name */
            switch(search(trie, searchingName, 0, &pass)){
                case 0: fprintf(output,"\"%s\" no record\n", splitted[1]); break;
                case 1: fprintf(output,"\"%s\" incorrect username\n", splitted[1]); break;
                case 2: fprintf(output,"\"%s\" not enough username\n", splitted[1]); break;
                case 3: fprintf(output,"\"%s\" password: \"%s\"\n", splitted[1], pass); break;
                default: break;
            }
        }
        else if(line[1] == 'q'){      /****** LOGIN Command ********/
            char** splitted = split(line, " ", 3);
            char* name = splitted[1], *pass = splitted[2];
            if(pass[strlen(pass)-1] == '\n') pass[strlen(pass)-1] = '\0';
            login(trie, name, pass, 0, output);
        }
        else if(line[1] == 'd'){      /****** DELETE Command *******/
            char** splitted = split(line, " ", 2);
            int success = 0;
            if(splitted[1][strlen(splitted[1])-1] == '\n') splitted[1][strlen(splitted[1])-1] = '\0';

            char* name = (char*)malloc(sizeof(char) * strlen(splitted[1]));
            strcpy(name, splitted[1]);
            char* fullname = (char*)malloc(sizeof(char) * strlen(splitted[1]));
            strcpy(fullname, splitted[1]);
            Remove(&trie, name, fullname, &success);

            switch (success){
                case 1: fprintf(output,"\"%s\" deletion is successful\n", fullname); break;
                case 2: fprintf(output,"\"%s\" no record\n", fullname); break;
                case 3: fprintf(output,"\"%s\" incorrect username\n", fullname); break;
                case 4: fprintf(output,"\"%s\" not enough username\n", fullname); break;
                default: break;
            }
        }
                                   /****** LIST Command ******/
        else if(line[1] == 'l') {

            char *name = (char *) malloc(sizeof(char) * 50);
            strcpy(name, "");
            char *str = (char *) malloc(sizeof(char) * 50);
            strcpy(str, "");
            node* iter = trie->children; /* Trie: Root of the tree, iter: every child of the root. */
            int indent = 1;

            while(iter != NULL){
                preOrder(iter, name, &indent, output, str);
                strcpy(name, "");
                strcpy(str, "");
                iter = iter->next;
                indent = 1;
            }

        } else
            fprintf(output,"Invalid Command\n");
    }
    return trie;
}
int main(int argc, char** argv){
    node* tree = init('#', "");
    tree = readInput(argv[1], tree, "output.txt");

    return 0;
}
