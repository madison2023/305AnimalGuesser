//
//  
//  AnimalGuesser
//
//  Created by Rachel Madison on 11/10/20.
//  Copyright © 2020 Rachel Madison. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Question_struct Question;
struct Question_struct {
    char * q;
    Question* yes;
    Question* no;
};

typedef struct QNode_struct QNode;
struct QNode_struct {
    Question* queueNode;
    int qnId; //qn id number
    int yesId; //yes id number
    int noId; //no id number
    QNode * next;
};

/**
 Creates a new Question struct
    @param input a string that is set to the q in the question struct
        
        other struct members are initialized to NULL
 */
Question * newQuestion(char * input) {
    Question * temp = malloc(sizeof(Question));
    temp->q = malloc(50);
    strcpy(temp->q, input);
    temp->yes = NULL;
    temp->no = NULL;
    return temp;
}

/**
    Creates a QNode
        @param q a Question struct that is set to the struct member queueNode
 */
QNode * newQNode(Question * q) {
    QNode * qn = (QNode*)malloc(sizeof(QNode));
    //Question * question = (Question*)malloc(sizeof(Question));
    qn->queueNode = q;
    qn->qnId = 0;
    qn->yesId = 0; //initialize these later
    qn->noId = 0;
    qn->next = NULL;
    //maybe do a previous later
    return qn;
}

/**
 gets input from terminal using gets() and returns it in a string
 */
char * get_user_input() {
    char * input = malloc(1024);
    gets(input);
    return input;
}

/**
 decides if what the user inputed means yes or no
 returns 1 for yes and 0 for no
 */
int get_yes_or_no() {

    char *answer = get_user_input();
    
    //making answer all lowercase
    for (int i = 0; i < sizeof(answer); i++) {
        answer[i] = tolower(answer[i]);
    }
    
    //works if answers start with y (yes, yeah, YEs ...)
    if (*answer == 'y') {
        return 1;
    }
    //all of the answers for no start with n so just checking for that
    else if (*answer == 'n'){
        return 0;
    }
    else {
        return -1; //neither yes or no
    }
}

/**
 asks the user a question, keeps asking different questions depending on whether the user said yes or no to the previous one
 @param question    a question struct that contains a string question that the user will be asked
 returns 1 if reached correct animal, 0 if not
 */
int ask(Question * question) {
    
    //asks question and gets answer
    printf("%s\n", question->q);
    int yesOrNo = get_yes_or_no();
    
    //case where question is a leaf node
    if (question->yes == NULL && question->no == NULL) {
        
        if (yesOrNo == 1) {
            printf("Animal guessed correctly!\n");
             return 1;
        }
        //handles case where there are no more questions to ask but user entered no, so animal was not able to be guessed correctly
        else if (yesOrNo == 0) {
            printf("Animal not guessed correctly. Please enter your animal:\n");
            char * correctAnimal = get_user_input();
            printf("Please enter a question I could ask next time to correctly guess your animal:\n");
            char * newQ = get_user_input();
            
            char *temp = question->q;
            //replacing old question with newQ
            question->q = newQ;
            
            //making new question that should result in correctly guessed animal if answer yes to newQ
            char *finishedQuestion = malloc(1024);
            strcat(finishedQuestion, "Is it a ");
            strcat(finishedQuestion, correctAnimal);
            strcat(finishedQuestion, "?");
            question->yes = newQuestion(finishedQuestion);
            
            //making the no the previous Is it a blank? question
            question->no = newQuestion(temp);
            
            return 0;
        }
        else {
            printf("Error, not a valid answer\n");
             return 0;
        }
       
    }
    //keeps asking other questions in the tree if not at a leaf node yet
    else if (yesOrNo == 1){
        ask(question->yes);
        return 0;
    }
    else {
        ask(question->no);
        return 0;
    }
}


int id = 1;
/**
postOrder, traverses tree in postorder, helper method for save_tree()
@param root     root of the tree
@param fp       file that the traversed tree is printed to
*/
void postOrder(Question * root, FILE * fp) {
    if( root != NULL) {
        postOrder(root->yes, fp);
        postOrder(root->no, fp);
        fprintf(fp, "%s\n", root->q);
        fprintf(fp, "%d\n", id);
        id++;
    }
}
/**
preOrder, traverses tree in preorder, helper method for save_tree()
@param root     root of the tree
@param fp       file that the traversed tree is printed to
*/
void preOrder(Question * root, FILE * fp) {
    
    if (root != NULL) {
        if (root->yes == NULL && root->no == NULL) {
            id = 1;
        }
        else {
            id = 0;
        }
        fprintf(fp, "%s\n", root->q);
        fprintf(fp, "%d\n", id);
        preOrder(root->yes, fp);
        preOrder(root->no, fp);
    }
}
/**
saves tree to the file it creates
 @param root is the root of the tree
 */
void save_tree(Question * root) {
    
    FILE *fp;
    char animalFile[30] = "AnimalGuesser.txt";
    fp = fopen(animalFile, "w");
    preOrder(root, fp);
    fclose(fp);
}

/**
 loads data from file into a tree of Question structs
 @param fp the file that has the questions and id numbers
 @param q   the root question
 note: this function doesn't work correctly
 */
Question * load_tree(FILE *fp, Question * q) {

    char line[50];
    //makes sure there is a line to get data from
    if ((fgets(line, 1024, fp)) == NULL) {
        return NULL;
    }
    else {
        //getting line (string)
        fgets(line, 1024, fp);
        Question * q = newQuestion(line);
        
        //every other line is an int
        fgets(line, 1024, fp);
        if (atoi(line) == 1) {
            q->yes = NULL;
            q->no = NULL;
        }
        else {
            //loading the rest of the nodes
            q->yes = load_tree(fp, q->yes);
            q->no = load_tree(fp, q->no);
        }
    }

    fclose(fp);
    return q;
    
    
    
}

//cleans up the memory from Questions
void clean_up_tree(Question * q) {
    if(q != NULL) {
        clean_up_tree(q->yes);
        clean_up_tree(q->no);
        free(q);
    }
}




int main(int argc, const char * argv[]) {
    
    FILE *fp;
    char animalFile[30] = "AnimalGuesser.txt";
    
    Question * firstQ = newQuestion("Does it have fur?");
    firstQ->yes = newQuestion("Is it a dog?");
    firstQ->no = newQuestion("Is it a fish?");
    
    if ((fp = fopen(animalFile, "r")) != NULL) {
        firstQ = load_tree(fp, firstQ);
    }

    //playing game, then asking at then end if they would like to play again
    int yesOrNo;
    do {
        ask(firstQ);
        printf("Would you like to play again:\n");
        yesOrNo = get_yes_or_no();
    } while (yesOrNo == 1);
    
    save_tree(firstQ);
    
    clean_up_tree(firstQ);
    return 0;
}
