//Kanellaki Maria Anna - 1115201400060

#include "Tree.h"


treeNode::treeNode(PatientRecord *rec)          //initializes a Tree node with data from given PatientRecord and null children
{
    record = rec;
    date = rec->getEntryDate();

    left = NULL;
    right = NULL;
    height = -1;                                //height is set by Tree class during insertion
}


treeNode::~treeNode()                           //deletes node and pointers without deleting the children
{
    //delete record;                            //patient records destroyed by list
    //delete date;                              //dates deleted by list
    left = NULL;
    right = NULL;
    delete left;
    delete right;
}


Tree::Tree()                                    //initializes empty Tree
{
    root = NULL;
}


Tree::~Tree()                                   //calls custom recursive function to delete Tree
{
    destroyTree(root);
}


void Tree::insertInTree(treeNode *node)         //inserts given treeNode
{
    if (!root)
    {
        root = node;
        node->setHeight(1);              //root height = 1
        return;
    }
    treeNode * temp = root;
    while(temp->getRight() && temp->getLeft())  //finds 1st null child and inserts new node there
    {
        //moves to suitable child depending on the date
        if (node->getDate()->earlierThan(temp->getDate()))
            temp = temp->getLeft();
        else
            temp = temp->getRight();
    }
    if (temp->getRight())
        temp->setLeft(node);
    else
        temp->setRight(node);

    node->setHeight(temp->getHeight() + 1);

    BalanceTree(node);
}


treeNode * Tree::rotateLeft(treeNode *node)     //rotates left from given node
{
    treeNode *curr = node->getRight();
    treeNode *temp = curr->getLeft();

    //rotate
    curr->setLeft(node);
    node->setRight(temp);

    //set new heights
    node->setHeight(max(node->getLeft()->getHeight(), node->getRight()->getHeight()) + 1);
    curr->setHeight(max(curr->getLeft()->getHeight(), curr->getRight()->getHeight()) + 1);

    //if root changed return new root else return null
    if (curr->getHeight() == 1)
        return curr;
    if (node->getHeight() == 1)
        return node;
    if (temp->getHeight() == 1)
        return temp;

    return NULL;
}


treeNode * Tree::rotateRight(treeNode *node)    //rotates right from given node
{
    treeNode *curr = node->getLeft();
    treeNode *temp = curr->getRight();

    //rotate
    curr->setRight(node);
    node->setLeft(temp);

    //set new heights
    node->setHeight(max(node->getLeft()->getHeight(), node->getRight()->getHeight()) + 1);
    curr->setHeight(max(curr->getLeft()->getHeight(), curr->getRight()->getHeight()) + 1);

    //if root changed return new root else return null
    if (curr->getHeight() == 1)
        return curr;
    if (node->getHeight() == 1)
        return node;
    if (temp->getHeight() == 1)
        return temp;

    return NULL;
}


void Tree::BalanceTree(treeNode *node)          //does necessary rotations to keep Tree balanced
{
    if (!node)
        return;

    if (!node->getLeft())
        return;

    if (!node->getRight())
        return;

    int balance = node->getLeft()->getHeight() - node->getRight()->getHeight();
    while (balance < -1 || balance > 1)
    {
        if (balance > 1 && node->getDate()->earlierThan(node->getLeft()->getDate()))
            rotateRight(node);

        if (balance < -1 && node->getDate()->earlierThan(node->getRight()->getDate()))
            rotateLeft(node);

        if (balance > 1 && node->getDate()->earlierThan(node->getLeft()->getDate()))
        {
            rotateLeft(node->getLeft());
            rotateRight(node);
        }

        if (balance < -1 && node->getDate()->earlierThan(node->getRight()->getDate()))
        {
            rotateRight(node->getRight());
            rotateLeft(node);
        }

        balance = node->getLeft()->getHeight() - node->getRight()->getHeight();
    }
}


void Tree::destroyTree(treeNode *node)          //destroys whole Tree recursively
{
    if (!node)
        return;

    destroyTree(node->getLeft());
    destroyTree(node->getRight());
    delete(node);
}


int Tree::countDiseased(treeNode * node)        //accesses recursively all elements of the tree, counts them and returns their count
{
    if (!root)
        return 0;

    int count = 1;

    if (node->getLeft())
        count += countDiseased(node->getLeft());

    if (node->getRight())
        count += countDiseased(node->getRight());

    return count;
}


void Tree::countDiseasedDates(treeNode* node, Date *from, Date *to, int *count) //accesses recursively all elements of the tree and counts those between the 2 dates. returns their count
{
    if (!node)
        return;

    //checks both enter and exit date
    if (from->earlierThan(node->getDate()) && node->getDate()->earlierThan(to))
        *(count) += 1;
    else if (node->getRecord()->getExitDate() && from->earlierThan(node->getRecord()->getExitDate()) && node->getDate()->earlierThan(node->getRecord()->getExitDate()))
        *(count) += 1;

    if (node->getLeft())
        countDiseasedDates(node->getLeft(), from, to, count);

    if (node->getRight())
        countDiseasedDates(node->getRight(), from, to, count);
}


void Tree::countDiseasedCountry(treeNode *node, Date *from, Date *to, string disease, int *count) //accesses recursively all elements of the tree and counts those between the 2 dates that had the given disease. returns their count
{                                                                                                 //created to work with country table
     if (!node)
        return;

    //checks both enter and exit date
    if (disease == node->getRecord()->getDiseaseId())
    {
        if (from->earlierThan(node->getDate()) && node->getDate()->earlierThan(to))
            *(count) += 1;
        else if (node->getRecord()->getExitDate() && from->earlierThan(node->getRecord()->getExitDate()) && node->getDate()->earlierThan(node->getRecord()->getExitDate()))
            *(count) += 1;
    }

    if (node->getLeft())
        countDiseasedCountry(node->getLeft(), from, to, disease, count);

    if (node->getRight())
        countDiseasedCountry(node->getRight(), from, to, disease, count);
}


//new functions
void Tree::countEnterOrExit(treeNode *node, Date *from, Date *to, int *count, string status)                //exactly like CountDiseasedDates but counts only entered or exited
{                                                                                                           //status points to which dates are counted
    if (!node)
        return;

    if (status == "ENTER")      //counts only entered
    {
        if (from->earlierThan(node->getDate()) && node->getDate()->earlierThan(to))
            *(count) += 1;
    }
    else                        //counts only exited
    {
        if (node->getRecord()->getExitDate() && from->earlierThan(node->getRecord()->getExitDate()) && node->getDate()->earlierThan(to))
            *(count) += 1;
    }

    if (node->getLeft())
        countEnterOrExit(node->getLeft(), from, to, count, status);

    if (node->getRight())
        countEnterOrExit(node->getRight(), from, to, count, status);
}


void Tree::countEnterOrExitCountry(treeNode *node, Date *from, Date *to, string disease, int *count, string status) //exactly like CountDiseasedCountry but counts only entered or exited
{                                                                                                                   //status points to which dates are counted
    if (!node)
        return;

    if (disease == node->getRecord()->getDiseaseId())
    {
        if (status == "ENTER")      //counts only entered
        {
            if (from->earlierThan(node->getDate()) && node->getDate()->earlierThan(to))
                *(count) += 1;
        }
        else                        //counts only exited
        {
            if (node->getRecord()->getExitDate() && from->earlierThan(node->getRecord()->getExitDate()) && node->getDate()->earlierThan(to))
                *(count) += 1;
        }
    }

    if (node->getLeft())
        countEnterOrExitCountry(node->getLeft(), from, to, disease, count, status);

    if (node->getRight())
        countEnterOrExitCountry(node->getRight(), from, to, disease, count, status);
}


//getters and setters for both classes
treeNode *treeNode::getLeft() const
{
    return left;
}


treeNode *treeNode::getRight() const
{
    return right;
}


Date *treeNode::getDate() const
{
    return date;
}


void treeNode::setLeft(treeNode *left)
{
    treeNode::left = left;
}


void treeNode::setRight(treeNode *right)
{
    treeNode::right = right;
}


void treeNode::setHeight(int height)
{
    treeNode::height = height;
}


int treeNode::getHeight() const
{
    return height;
}


PatientRecord *treeNode::getRecord() const
{
    return record;
}


treeNode *Tree::getRoot() const
{
    return root;
}