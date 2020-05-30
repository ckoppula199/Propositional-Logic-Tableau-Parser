#include <stdio.h>
#include <string.h>   /*string functions */
#include <stdlib.h>     /* malloc, free, rand */
#define ALPHA 1
#define BETA 2
#define ALPHA_DOUBLE_NEGATION 3
#define LITERAL 4

int Fsize=50; /*maximum formula length*/
int inputs =10;/* number of formulas expected in input.txt*/
int ThSize=100;/* maximum size of set of formulas, if needed*/
int TabSize=500; /*maximum length of tableau queue, if needed*/
char lookahead = ' ';
char *ptr;
int completed = 1;
int failedParse = 0;
char *buff1 = NULL;
char *buff2 = NULL;
int i;
int j;

/* A tableau will contain a list of pointers to sets (of words).  Use NULL for empty list.*/
struct tableau {
  char *root;
  struct tableau *left_child;
  struct tableau *right_child;
};

int isProposition(char *g);
int isNegation(char *g);
int isBinaryFormula(char *g);
int checkBinaryConnective();
int isFormula(char *g);
int checkTypeOfFormula(char *formula);
int isLiteral(char* one);
char *removeFirstElement(char *formula);
char *addNegation(char *formula);
char *alphaDoubleNegation(char *formula);
void check(struct tableau *t);
int checkIfBranchIsClosed(struct tableau *t, int* oldContradictions);

int match(char t) {
  if (lookahead == t) {
    ptr++;
    lookahead = ptr[0];
    return 1;
  }
  return 0;
}

int checkFormula() {  
  switch(ptr[0]) {

    case '-':
      completed++;
      if(match('-') == 0) return 0;
      checkFormula();
      completed--;
      break;

    case '(':
      completed++;
      if(match('(') == 0) return 0;
      checkFormula();
      if (checkBinaryConnective() == 0) return 0;
      checkFormula();
      if(match(')') == 0) return 0;
      completed--;
      break;

    case 'p':
      if(match('p') == 0) return 0;
      break;

    case 'q':
      if(match('q') == 0) return 0;
      break;

    case 'r':
      if(match('r') == 0) return 0;
      break;

    default:
      failedParse = 1;
      return 0;
  }
}

int checkBinaryConnective() {
  switch(ptr[0]) {

    case 'v':
      if(match('v') == 0) return 0;
      break;

    case '^':
      if(match('^') == 0) return 0;
      break;

    case '>':
      if(match('>') == 0) return 0;
      break;

    default:
      failedParse = 1;
      return 0;
  }
}

void reset() {
  lookahead = ' ';
  completed = 1;
  failedParse = 0;
  free(buff1);
  buff1 = NULL;
  free(buff2);
  buff2 = NULL;
}

int parse(char *g) {
  int formulaCheck = isFormula(g);
  if (formulaCheck == 0) return 0;
  int propositionCheck = isProposition(g);
  if (propositionCheck == 1) return 1;
  int negationCheck = isNegation(g);
  if (negationCheck == 1) return 2;
  int binaryCheck = isBinaryFormula(g);
  if (binaryCheck == 1) return 3;
}

int isFormula(char *g) {
  reset();
  ptr = g;
  lookahead = g[0];  
  checkFormula(ptr);
  //printf("failedParse %i\n", failedParse);
  //printf("completed %i\n", completed);
  if (failedParse) return 0;
  if (ptr[0] == '\0' && completed == 1) {
    return 1;
  } else {
    return 0;
  }
}

int isProposition(char *g) {
  if (strcmp(g, "p") == 0 || strcmp(g, "q") == 0 || strcmp(g, "r") == 0) {
    return 1;
  }
  return 0;
}

int isNegation(char *g) {
  if (g[0] == '-') {
    return 1;
  }
  return 0;
}

int isBinaryFormula(char *g) {
  if(g[0] == '(') {
    return 1;
  }
  return 0;
}

int findMiddle(char *g) {
  int middle = -1;
  int brackets = 0;
  int bc = 0;
  for(i = 0; g[i] != '\0'; i++) {
    if(i == 0 && g[i] == '-') {
      continue;
    }
    if (g[i] == '(') {
      brackets++;
    }
    else if (g[i] == 'v' || g[i] == '>' || g[i] == '^') {
      bc++;
    }
    if (bc == brackets) {
      //printf("middle is %i\n", i);
      //get middle ting to check alpha or beta
      //printf("first letter %c\n", *(g + 0));
      return i;
    }
  }
}

char *partone(char *g) {
  int middle = findMiddle(g);
  int temp = middle-1;
  if(isNegation(g)) {
    buff1 = malloc(sizeof(char) * (temp-1));
    memcpy(buff1, &g[2], temp-1);
    buff1[middle-2] = '\0';
  } else {
    buff1 = malloc(sizeof(char) * temp);
    memcpy(buff1, &g[1], temp);
    buff1[middle-1] = '\0';
  }
  return buff1;
}

char *parttwo(char *g) {
  int middle = findMiddle(g);
  buff2 = malloc(sizeof(char) * (strlen(g) - middle - 1));
  memcpy(buff2, &g[middle+1], strlen(g) - middle -2);
  buff2[strlen(g) - middle - 2] = '\0';
  return buff2;
}

int checkTypeOfFormula(char *formula) {
  char *temp = formula;
  char *temp1 = formula;
  if(isLiteral(temp1)) {
    return LITERAL;
  }
  if(*(formula+0) == '-' && *(formula + 1) == '-') {
    return ALPHA_DOUBLE_NEGATION;
  }
  int negated = isNegation(formula);
  int middle = findMiddle(temp);
  if(*(formula + middle) == 'v') {
    if(negated) {
      return ALPHA;
    }
    return BETA;
  }
  if(*(formula + middle) == '^') {
    if(negated) {
      return BETA;
    }
    return ALPHA;
  }
  if(*(formula + middle) == '>') {
    if(negated) {
      return ALPHA;
    }
    return BETA;
  }
  return -1;
}

int isLiteral(char* one) {
  if(strcmp(one, "p") == 0) return 1;
  if(strcmp(one, "q") == 0) return 1;
  if(strcmp(one, "r") == 0) return 1;
  if(strcmp(one, "-p") == 0) return 1;
  if(strcmp(one, "-q") == 0) return 1;
  if(strcmp(one, "-r") == 0) return 1;
  return 0;
}

char *addNegation(char *formula) {
    char *negatedFormula = (char*) malloc((strlen(formula)+2) * sizeof(char));
    negatedFormula[0] = '-';
    i = 1;
    while(formula[i-1] != '\0') {
        negatedFormula[i] = formula[i-1];
        i++;
    }
    negatedFormula[i] = '\0';
    return negatedFormula;
}

char *removeFirstElement(char *formula) {
    char *tailFormula = (char*) malloc(strlen(formula) * sizeof(char));
    i = 0;
    while(formula[i+1] != '\0'){
        tailFormula[i] = formula[i+1];
        i++;
    }
    tailFormula[i] = '\0';
    return tailFormula;
}

char *alphaDoubleNegation(char *formula) {
  while(*(formula+0) == '-' && *(formula + 1) == '-') {
    formula = removeFirstElement(formula);
    formula = removeFirstElement(formula);
  }
  return formula;
}

void addTwoNodesToAllLeaves(struct tableau *t, char *formula1, char *formula2) {
  if(t->left_child == NULL) {
    struct tableau *leftChild = malloc(sizeof(struct tableau));
    struct tableau *rightChild = malloc(sizeof(struct tableau));

    leftChild->root = formula1;
    leftChild->left_child = NULL;
    leftChild->right_child = NULL;
    t->left_child = leftChild;

    rightChild->root = formula2;
    rightChild->left_child = NULL;
    rightChild->right_child = NULL;
    t->right_child = rightChild;
  } else {
    addTwoNodesToAllLeaves(t->left_child, formula1, formula2);
    if(t->right_child != NULL) {
      addTwoNodesToAllLeaves(t->right_child, formula1, formula2);
    }
  }
}

void addNodeToAllLeaves(struct tableau *t, char *fromula) {
  if(t->left_child == NULL) {
    struct tableau *child = malloc(sizeof(struct tableau));
    child->root = fromula;
    child->left_child = NULL;
    child->right_child = NULL;
    t->left_child = child;
  } else {
    addNodeToAllLeaves(t->left_child, fromula);
  } if(t->right_child == NULL) {
    return;
  } else {
    addNodeToAllLeaves(t->right_child, fromula);
  }
}

void check(struct tableau *t) {
  char* formula = t->root;
  int type = checkTypeOfFormula(formula);
  int middle = findMiddle(formula);
  int negated = isNegation(formula);
  switch(type) {
    case ALPHA:
      if(*(formula+middle) == '^' && negated == 0) {
        addNodeToAllLeaves(t, partone(formula));
        addNodeToAllLeaves(t, parttwo(formula));
      } else if(*(formula+middle) == 'v' && negated == 1) {
        addNodeToAllLeaves(t, addNegation(partone(formula)));
        addNodeToAllLeaves(t, addNegation(parttwo(formula)));
      } else if(*(formula+middle) == '>' && negated == 1) {
        addNodeToAllLeaves(t, partone(formula));
        addNodeToAllLeaves(t, addNegation(parttwo(formula)));
      } else {
      }
      break;
    case BETA:
      if(*(formula+middle) == 'v' && negated == 0) {
        addTwoNodesToAllLeaves(t, partone(formula), parttwo(formula));
      } else if(*(formula+middle) == '^' && negated == 1) {
        addTwoNodesToAllLeaves(t, addNegation(partone(formula)), addNegation(parttwo(formula)));
      } else if(*(formula+middle) == '>' && negated == 0) {
        addTwoNodesToAllLeaves(t, addNegation(partone(formula)), parttwo(formula));
      } else {
      }
      break;
    case ALPHA_DOUBLE_NEGATION:
      t->root = alphaDoubleNegation(t->root);
      check(t);
      break;
    case LITERAL:
      break;
    default:
      break;
  }
}


void complete(struct tableau *t){
  //carry out a pre-order traversal
  if(t == NULL) {
    return;
  }
  check(t);
  complete(t->left_child);
  complete(t->right_child); 
}

void debug(struct tableau *t) {

  if(t == NULL) {
    return;
  }
  debug(t->left_child);
  debug(t->right_child);
}

int checkIfBranchIsClosed(struct tableau *t, int* oldContradictions) {
  int contradictions[6];
  memcpy(contradictions, oldContradictions, 6 * sizeof(int));
  char *formula = t->root;
  if(strcmp(formula, "p") == 0) {
    contradictions[0] = 1;
  } else if(strcmp(formula, "q") == 0) {
    contradictions[1] = 1;
  } else if(strcmp(formula, "r") == 0) {
    contradictions[2] = 1;
  } else if(strcmp(formula, "-p") == 0) {
    contradictions[3] = 1;
  } else if(strcmp(formula, "-q") == 0) {
    contradictions[4] = 1;
  } else if(strcmp(formula, "-r") == 0) {
    contradictions[5] = 1;
  }
  for(i=0;i<3;i++) {
    if(contradictions[i] == 1 && contradictions[i+3] == 1) {
      return 1;
    }
  }
  if(t->left_child == NULL && t->right_child == NULL) {
    return 0;
  } else if(t->left_child != NULL && t->right_child == NULL) {
    checkIfBranchIsClosed(t->left_child, contradictions);
  } else if(t->left_child != NULL && t->right_child != NULL) {
    int a = checkIfBranchIsClosed(t->left_child, contradictions);
    int b = checkIfBranchIsClosed(t->right_child, contradictions);
    if(a==1 && b==1) {
      return 1;
    } else {
      return 0;
    }
  }

}

int closed(struct tableau *t) {
  int contradictions[6] = {0,0,0,0,0,0};
  char* formula = t->root;
  if(strcmp(formula, "p") == 0) {
    contradictions[0] = 1;
  } else if(strcmp(formula, "q") == 0) {
    contradictions[1] = 1;
  } else if(strcmp(formula, "r") == 0) {
    contradictions[2] = 1;
  } else if(strcmp(formula, "-p") == 0) {
    contradictions[3] = 1;
  } else if(strcmp(formula, "-q") == 0) {
    contradictions[4] = 1;
  } else if(strcmp(formula, "-r") == 0) {
    contradictions[5] = 1;
  }
  if(t->left_child == NULL && t->right_child == NULL) {
    return 0;
  } else if(t->left_child != NULL && t->right_child == NULL) {
    checkIfBranchIsClosed(t->left_child, contradictions);
  } else if(t->left_child != NULL && t->right_child != NULL) {
    int a = checkIfBranchIsClosed(t->left_child, contradictions);
    int b = checkIfBranchIsClosed(t->right_child, contradictions);
    if(a==1 && b==1) {
      return 1;
    } else {
      return 0;
    }
  }
}


int main()
{ 
    FILE *fp, *fpout;
    char *name = malloc(Fsize);
    struct tableau *t=(struct tableau *)malloc(sizeof(struct tableau));
 
  /* reads from input.txt, writes to output.txt*/
    if ((  fp=fopen("input.txt","r"))==NULL){printf("Error opening file");exit(1);}
    if ((  fpout=fopen("output.txt","w"))==NULL){printf("Error opening file");exit(1);}

    int j;
    
    for(j=0;j<inputs;j++)
    {
        fscanf(fp, "%s",name);/*read formula*/
        switch (parse(name))
        {
            case(0): fprintf(fpout, "%s is not a formula.  \n", name);break;
            case(1): fprintf(fpout, "%s is a proposition. \n ", name);break;
            case(2): fprintf(fpout, "%s is a negation.  \n", name);break;
            case(3):fprintf(fpout, "%s is a binary. The first part is %s and the second part is %s  \n", name, partone(name), parttwo(name));break;
            default:fprintf(fpout, "What the f***!  ");
        }

           
        
        if (parse(name)!=0)
        {
            struct tableau t;
            t.root = name;
            t.left_child = NULL;
            t.right_child = NULL;
            complete(&t);
            debug(&t);
            if (closed(&t))  fprintf(fpout, "%s is not satisfiable.\n", name);
            else fprintf(fpout, "%s is satisfiable.\n", name);
        }
        else  fprintf(fpout, "I told you, %s is not a formula.\n", name);
    }

 
    fclose(fp);
    fclose(fpout);
    free(name);
    free(t);

  return(0);
}





        