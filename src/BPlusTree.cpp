#include "BPlusTree.h"
#include <iostream>
#include <queue>

using namespace std;


TreeNode::TreeNode(TreeNode *_parent, bool _isLeaf, TreeNode *_next) {
    this->parent = _parent;
    this->isLeaf = _isLeaf;
    this->next = _next;

}

BPlusTree::BPlusTree(int _degree) {
    this->degree = _degree;
    this->root = nullptr;

}

int BPlusTree::binarySearch(const vector<int>& arr, int value) {
    int low = 0;
    int high = arr.size();

    while (low < high) {
        int mid = low + (high - low) / 2;

        if (arr[mid] <= value) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    return low;
}

TreeNode* BPlusTree::getLeafNodeBasedOnKey(TreeNode* root, int key, int* indexBlockAccessed){
    int index;
    while (root->isLeaf == false){ // iterating through non leaf nodes
        if (indexBlockAccessed != nullptr){
            *indexBlockAccessed += 1;
        }
       
        index = this->binarySearch(root->keys, key);
        root = root->children[index];
    }
    if (indexBlockAccessed != nullptr){
        *indexBlockAccessed += 1;
    }
    return root;
}

 int BPlusTree::doesKeyExist(int key, vector<int> keyList){
    int left = 0;
    int right = keyList.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        // If the target is found at mid
        if (keyList[mid] == key) {
            return mid;
        }

        // If target is greater, ignore left half
        if (keyList[mid] < key) {
            left = mid + 1;
        }
        // If target is smaller, ignore right half
        else {
            right = mid - 1;
        }
    }

    // If target is not found in the list
    return -1;
 }


void BPlusTree::InsertIndexEntry(int key, string recordLocation) {
    // edge case where no root
    if (this->root == nullptr){
        // insert key and record in leaf node
        TreeNode *newNode = new TreeNode(nullptr, true, nullptr);
        newNode->keys.push_back(key);
        vector<string>* recordLocationList = new vector<string>{recordLocation};
        newNode->recordLocationLists.push_back(recordLocationList);

        this->root = newNode;
        return;
    }

    
    TreeNode *cur = this->root;
    
    // find leaf node where the key-value should be inserted at
    cur = this->getLeafNodeBasedOnKey(cur, key, nullptr);
    // get next node for linking the leaf nodes
    TreeNode *nextNode = cur->next;

    int keyIndex = this->doesKeyExist(key, cur->keys);

    // if key exist, handle duplicates
    if (keyIndex != - 1){
        // insert into set
        cur->recordLocationLists[keyIndex]->push_back(recordLocation);
        return;
    }

    // insert key and recordlocation into node
    int indexToInsert = this->binarySearch(cur->keys, key);
    cur->keys.insert(cur->keys.begin() + indexToInsert, key);
    vector<string>* recordLocationList = new vector<string>{recordLocation};
    cur->recordLocationLists.insert(cur->recordLocationLists.begin() + indexToInsert, recordLocationList);

    // if within size, do nothing and return
    if (cur->keys.size() < this->degree){
        return;
    }

    // else means over size, split node into 2 and distribute the nodes, Leaf node: at least floor((n+1)/2) keys. Non-leaf node: at least floor(n/2) keys
    int oldNodeSize = static_cast<int>(ceil(static_cast<double>(this->degree) / 2));

    vector<int> tempKeys = cur->keys;
    vector<vector<string>*> tempRecordLocationLists = cur->recordLocationLists;

    // set old node keys
    auto oldKeyBegin= tempKeys.begin();  
    auto oldKeyEnd = tempKeys.begin() + oldNodeSize;  
    vector<int> oldKeySlice(oldKeyBegin, oldKeyEnd);
    cur->keys = oldKeySlice;

    // set old node records
    auto oldLocationListBegin = tempRecordLocationLists.begin();  
    auto oldLocationListEnd = tempRecordLocationLists.begin() + oldNodeSize;  
    vector<vector<string>*> oldLocationSlice(oldLocationListBegin, oldLocationListEnd);
    cur->recordLocationLists = oldLocationSlice;

    // create new node
    TreeNode *newNode = new TreeNode(nullptr, true, nullptr);

    // set new node keys
    auto newKeyBegin = tempKeys.begin() + oldNodeSize;  
    auto newKeyEnd = tempKeys.end();  
    vector<int> newKeySlice(newKeyBegin, newKeyEnd);
    newNode->keys = newKeySlice;
    
    // set new node records
    auto newLocationListBegin = tempRecordLocationLists.begin() + oldNodeSize;  
    auto newLocationListEnd = tempRecordLocationLists.end();  
    vector<vector<string>*> newLocationSlice(newLocationListBegin, newLocationListEnd);
    newNode->recordLocationLists = newLocationSlice;

    // link the old and new nodes like linked list
    cur->next = newNode;
    newNode->next = nextNode;
    
    // call InsertParent() to recursively handle non leaf nodes
    this->insertParent(newNode->keys[0], cur, newNode, cur->parent);
       
    
    return;
}

void BPlusTree::insertParent(int key, TreeNode* oldNode, TreeNode* newNode, TreeNode* parentNode){
    // if no parent node
    if (parentNode == nullptr){
        // create new parent node
        TreeNode *parentNode = new TreeNode(nullptr, false, nullptr);

        // insert key to keys and old node, new node to parent node children
        parentNode->keys.push_back(key);
        parentNode->children.push_back(oldNode);
        parentNode->children.push_back(newNode);

        // set parent of old node and new node
        oldNode->parent = parentNode;
        newNode->parent = parentNode;

        // set newly created parent as root
        this->root = parentNode;

        return;
    }
    
    // if enough size to fit children
    if (parentNode->children.size() < this->degree){

        // insert key to keys and new node to parent node children
        int indexToInsert = this->binarySearch(parentNode->keys, key);      
        parentNode->keys.insert(parentNode->keys.begin() + indexToInsert, key);
        parentNode->children.insert(parentNode->children.begin() + indexToInsert + 1, newNode);
        
        // set parent of new node
        newNode->parent = parentNode;
        return;
    } 

    // not enough space, do split to get old node1, new node1, parent pointer, new key
   
    // insert child and key first, then split after
    int indexToInsert = this->binarySearch(parentNode->keys, key);
    parentNode->keys.insert(parentNode->keys.begin() + indexToInsert, key);
    parentNode->children.insert(parentNode->children.begin() + indexToInsert + 1, newNode);

    // create new node 
    TreeNode* newParentNode = new TreeNode(nullptr, false, nullptr);

    // get index on where to split
    int oldKeySize = static_cast<int>(ceil(static_cast<double>(this->degree - 1) / 2));

    vector<int> tempKeys = parentNode->keys;
    // set old parent node keys
    auto oldKeyBegin = tempKeys.begin();  
    auto oldKeyEnd = tempKeys.begin() + oldKeySize;  
    vector<int> oldKeySlice(oldKeyBegin, oldKeyEnd);
    parentNode->keys = oldKeySlice;

    // new key for the parent node of the two split nodes
    int newKey = tempKeys[oldKeySize];

    // set new parent node keys
    auto newKeyBegin = tempKeys.begin() + oldKeySize + 1;  
    auto newKeyEnd = tempKeys.end();  
    vector<int> newKeySlice(newKeyBegin, newKeyEnd);
    newParentNode->keys = newKeySlice;

    vector<TreeNode*> tempChildren = parentNode->children;
    // set old parent node children
    auto oldChildBegin = tempChildren.begin();  
    auto oldChildEnd = tempChildren.begin() + oldKeySize + 1;  
    vector<TreeNode*> oldChildSlice(oldChildBegin, oldChildEnd);
    parentNode->children = oldChildSlice;

     // set new parent node children
    auto newChildBegin = tempChildren.begin() + oldKeySize + 1;  
    auto newChildEnd = tempChildren.end();  
    vector<TreeNode*> newChildSlice(newChildBegin, newChildEnd);
    newParentNode->children = newChildSlice;


    // set parents of child nodes
    for (TreeNode* node : parentNode->children) {
        node->parent = parentNode;   
    }

    for (TreeNode* node : newParentNode->children) {
        node->parent = newParentNode;   
    }

    // call recursive function again since we split the nodes
    this->insertParent(newKey, parentNode, newParentNode, parentNode->parent);

    return;
}

void BPlusTree::VisualizeTree(){
    TreeNode* cur = this->root;

    queue<TreeNode*> q;
    q.push(cur);
    int level = 1;
    while (!q.empty()) {
        int nodesAtCurrentLevel = q.size();
        cout << "Level " << level << " nodes: ";
        while (nodesAtCurrentLevel > 0) {
            TreeNode *node = q.front();
            q.pop();

            
            cout << "[ " << "";

            // Print keys of the current node
            for (int num : node->keys) {
                cout << num << ",";
            }

            //Print key and children/record sizes
            // cout << node->keys.size() << ",";
            // if (node->isLeaf){
            //     cout << node->records.size() << "";
            // }else{
            //     cout << node->children.size() << "";
            // }
            
            cout << " ]" << "";

            // Enqueue children of the current node
            if (!node->isLeaf) {
                for (TreeNode* newNode : node->children) {
                    q.push(newNode);
                    
                }
            }

            nodesAtCurrentLevel--;
        }
        cout << "\n";
        level += 1;
    }


    return;
}

void BPlusTree::PrintStatsForTree(){
    TreeNode* cur = this->root;

    if (cur == nullptr){
        cout << "Tree is empty";
        return;
    }

    queue<TreeNode*> q;
    q.push(cur);
    int level = 0;
    int countNodes = 1;
    while (!q.empty()) {
        level += 1;
        int nodesAtCurrentLevel = q.size();
        
        while (nodesAtCurrentLevel > 0) {
            TreeNode *node = q.front();
            q.pop();

            // Enqueue children of the current node
            for (TreeNode* newNode : node->children) {
                countNodes += 1;

                if (!node->isLeaf){
                    q.push(newNode);    
                }
                
            }
            nodesAtCurrentLevel--;
        }
    }

    cout << "Statistics For B+ Tree:" << "\n\n";
    cout << "n: "<< this->degree - 1 << "\n";
    cout << "Total Tree Nodes: "<< countNodes << "\n";
    cout << "Total Tree Levels: "<< level << "\n";
    cout << "Content of root node: ";
    for (int key: this->root->keys){
        cout << key << " ";
    }
    cout << "\n\n";
}


vector<string> BPlusTree::RetrieveBlocks(int key, int* indexBlockAccessed){
    vector<string> recordLocationList;
    if (this->root == nullptr){
        cerr << "Tree does not exist \n";
        return recordLocationList;
    }

    TreeNode *cur = this->root;
    // find leaf node where the key should be at
    cur = this->getLeafNodeBasedOnKey(cur, key, indexBlockAccessed);

    
    // get key index if key exists, if not exists return error
    int keyIndex = this->doesKeyExist(key, cur->keys);
    if (keyIndex == -1){
        cerr << "Key does not exist in index \n";
        return recordLocationList;
    }

    return *(cur->recordLocationLists[keyIndex]);

}


vector<string> BPlusTree::RetrieveBlocksByRange(int startKey, int endKey, int* indexBlockAccessed){

    vector<string> recordLocationList;
    if (this->root == nullptr){
        cerr << "Tree does not exist \n";
        return recordLocationList;
    }

    TreeNode *cur = this->root;
    // find leaf node where the key should be at
    cur = this->getLeafNodeBasedOnKey(cur, startKey, indexBlockAccessed);

    // iterate through leaf nodes and their keys to find 
    while (cur != nullptr){
        
        for (int i = 0; i < cur->keys.size(); ++i){
            if (cur->keys[i] >= startKey && cur->keys[i] <= endKey){
                // add all the block indexes
                vector<string> actualSet = *(cur->recordLocationLists[i]);
                recordLocationList.insert(recordLocationList.end(), actualSet.begin(), actualSet.end());
            } else if (cur->keys[i] > endKey){ // outside range can terminate already since is sorted
                return recordLocationList;
            }
        }
        cur = cur->next;
        *indexBlockAccessed += 1;
    }

    return recordLocationList;
    
}

vector<string> BPlusTree::DeleteIndexEntry(int key){
    vector<string> deleteRecordLocations;

    TreeNode* cur = this->root;
    // find leaf node where the key resides
    cur = this->getLeafNodeBasedOnKey(cur, key, nullptr);

    // get index of key 
    int keyIndex = this->doesKeyExist(key, cur->keys);
     

    if (keyIndex == -1){
        cerr << "key does not exist in B+ Tree \n";
        return deleteRecordLocations;
    }

    
    int oldFirstKey = cur->keys[0];

    // delete key
    auto deleteKeyIndex = cur->keys.begin() + keyIndex;
    cur->keys.erase(deleteKeyIndex);

    // keep track of deleted records to be deleted in storage
    deleteRecordLocations = *(cur->recordLocationLists[keyIndex]);

    
    // delete record locations from index
    auto deleteRecordIndex = cur->recordLocationLists.begin() + keyIndex;
    cur->recordLocationLists.erase(deleteRecordIndex);

    
    // if enough keys left over
    if (cur->keys.size() >= floor(static_cast<double>(this->degree) / 2)){
        // if first key is changed, then need to modify parent node
        int newFirstKey = cur->keys[0];
        if (oldFirstKey != newFirstKey){
            this->modifyParentKey(cur->parent, oldFirstKey, newFirstKey);
        }
        return deleteRecordLocations;
    }
    
    // get left leaf node and right leaf node
    TreeNode* leftLeafNode = nullptr;
    TreeNode* rightLeafNode = nullptr;
    TreeNode* parent = cur->parent;
    for (int i = 0; i < parent->children.size(); i++){
        if (parent->children[i] == cur){
            if (i-1 >= 0){
                leftLeafNode = parent->children[i-1];
            }
            if (i+1 < parent->children.size()){
                rightLeafNode = parent->children[i+1];
            }
        }
    }


    // try borrow key from left sibling
    if (leftLeafNode != nullptr && leftLeafNode->keys.size() - 1 >= floor(static_cast<double>(this->degree) / 2)){
        this->borrowKey(cur, leftLeafNode, true, oldFirstKey);
        return deleteRecordLocations;
    }

    // try borrow key from right sibling
    if (rightLeafNode != nullptr && rightLeafNode->keys.size() - 1 >= floor(static_cast<double>(this->degree) / 2)){
        this->borrowKey(cur, rightLeafNode, false, oldFirstKey);
        return deleteRecordLocations;
    }

    // cannot borrow, need to merge nodes, try left first
    if (leftLeafNode != nullptr){
        this->mergeLeafNodes(leftLeafNode, cur, true);
        return deleteRecordLocations;
    }
    if (rightLeafNode != nullptr){
        this->mergeLeafNodes(cur, rightLeafNode, false);
        return deleteRecordLocations;
    }
    
    // means is a root node, we do nothing
    return deleteRecordLocations;
    
}

void BPlusTree::borrowKey(TreeNode* curNode, TreeNode* newNode, bool isLeftNode, int oldFirstKey){

    int borrowKey;
    vector<string>* borrowRecordLocationList;
    int oldKey;
    int newKey;
    if (isLeftNode){

        // take key out from the back
        borrowKey = newNode->keys.back();
        newNode->keys.pop_back();

        // take record location out from back
        borrowRecordLocationList = newNode->recordLocationLists.back();
        newNode->recordLocationLists.pop_back();

        // insert to front of cur node key and recordList
        curNode->keys.insert(curNode->keys.begin(), borrowKey);
        curNode->recordLocationLists.insert(curNode->recordLocationLists.begin(), borrowRecordLocationList);

        // get new key to be changed at parent
        newKey = curNode->keys[0];
    
        // modify parent
        this->modifyParentKey(curNode->parent, oldFirstKey, newKey);
        
    }else{
        // get old key
        oldKey = newNode->keys[0];

        // take out from the front
        borrowKey = newNode->keys[0];
        newNode->keys.erase(newNode->keys.begin());

        // take record location out from front
        borrowRecordLocationList = newNode->recordLocationLists[0];
        newNode->recordLocationLists.erase(newNode->recordLocationLists.begin());

        // insert to back of cur node
        curNode->keys.push_back(borrowKey);
        curNode->recordLocationLists.push_back(borrowRecordLocationList);

        // set new key to be changed at parent
        newKey = newNode->keys[0];

        // modify parent
        this->modifyParentKey(newNode->parent, oldKey, newKey);
    }

    return;
}


void BPlusTree::mergeLeafNodes(TreeNode* leftNode, TreeNode* rightNode, bool tookLeft){
    // move everything to the left node and delete right node
    leftNode->keys.insert(leftNode->keys.end(), rightNode->keys.begin(), rightNode->keys.end());
    leftNode->recordLocationLists.insert(leftNode->recordLocationLists.end(), rightNode->recordLocationLists.begin(), rightNode->recordLocationLists.end());

    // relink the nodes
    TreeNode* nextNode = rightNode->next;
    leftNode->next = nextNode;

    int deleteKey;
    if (tookLeft) {
        TreeNode* parent = leftNode->parent;
        for (int i = 0;i < parent->children.size(); i++){
            if (parent->children[i] == leftNode){
                deleteKey = parent->keys[i];
            }
        }
    } else {
        TreeNode* parent = rightNode->parent;
        for (int i = 0;i < parent->children.size(); i++){
            if (parent->children[i] == rightNode){
                deleteKey = parent->keys[i-1];
            }
        }
    }

    this->deleteKeyFromParent(rightNode->parent, rightNode, deleteKey);

 
    return;
}

void BPlusTree::deleteKeyFromParent(TreeNode* currentNode, TreeNode* deleteNode, int deleteKey){
    // base case for root node
    if (currentNode == this->root){
        // size = 1 means need to remove entire root node cos we deleting key from it
        if (currentNode->keys.size() == 1){
            if (currentNode->children[0] == deleteNode){
                this->root = currentNode->children[1];
                return;
            }
            if (currentNode->children[1] == deleteNode){
                this->root = currentNode->children[0];
                return;
            }
        }
    }

    // delete key
    auto deleteKeyIndex = find(currentNode->keys.begin(), currentNode->keys.end(), deleteKey);
    currentNode->keys.erase(deleteKeyIndex);

    // delete child pointer
    auto deleteChildIndex = find(currentNode->children.begin(), currentNode->children.end(), deleteNode);
    currentNode->children.erase(deleteChildIndex);

     // if enough keys left over
    if (currentNode->keys.size() >= floor(static_cast<double>(this->degree - 1) / 2)){
        return;
    }

    // if root we return cos it wont have any left or right sibling nodes
    if (this->root == currentNode){
        return;
    }

    // get left and right sibling node
    int leftSibilingIndex = -1;
    int rightSibilingIndex = -1;
    TreeNode* leftSibilingNode = nullptr;
    TreeNode* rightSibilingNode = nullptr;
    TreeNode* parent = currentNode->parent;
    for (int i = 0; i < parent->children.size(); i++){
        if (parent->children[i] == currentNode){
            if (i-1 >= 0){
                leftSibilingIndex = i - 1;
                leftSibilingNode = parent->children[i-1];
            }
            if (i+1 < parent->children.size()){
                rightSibilingIndex = i + 1;
                rightSibilingNode = parent->children[i+1];
            }
        }
    }

    int borrowKey;
    TreeNode* childPointer;
    if (leftSibilingNode != nullptr){
        // enough size to borrow
        if (leftSibilingNode->keys.size() - 1 >= floor(static_cast<double>(this->degree) / 2)){
            // borrowed key take from parent
            borrowKey = parent->keys[leftSibilingIndex];
            // set parent to last key of left node
            parent->keys[leftSibilingIndex] = leftSibilingNode->keys.back();
            // remove last key of left node
            leftSibilingNode->keys.pop_back();

            childPointer = leftSibilingNode->children.back();
            // remove last child pointer of left node
            leftSibilingNode->children.pop_back();

            // insert borrow key and borrow pointer to start of current node
            currentNode->keys.insert(currentNode->keys.begin(), borrowKey);
            currentNode->children.insert(currentNode->children.begin(), childPointer);

            return;

        }
    } 

    if (rightSibilingNode != nullptr){
        // enough size to borrow
        if (rightSibilingNode->keys.size() - 1 >= floor(static_cast<double>(this->degree) / 2)){
            // borrowed key take from parent
            borrowKey = parent->keys[rightSibilingIndex - 1];
            // set parent to last key of left node
            parent->keys[rightSibilingIndex - 1] = rightSibilingNode->keys[0];
            // remove first key of left node
            rightSibilingNode->keys.erase(rightSibilingNode->keys.begin());

            childPointer = rightSibilingNode->children[0];
            // remove last child pointer of left node
            rightSibilingNode->children.erase(rightSibilingNode->children.begin());

            // insert borrow key and borrow pointer to end of current node
            currentNode->keys.push_back(borrowKey);
            currentNode->children.push_back(childPointer);

            return;

        }
 
        // try merge with left node
        if (leftSibilingNode != nullptr){
            // move everything to the left node and delete right node
            leftSibilingNode->keys.push_back(parent->keys[leftSibilingIndex]);
            leftSibilingNode->keys.insert(leftSibilingNode->keys.end(), currentNode->keys.begin(), currentNode->keys.end());
            leftSibilingNode->children.insert(leftSibilingNode->children.end(), currentNode->children.begin(), currentNode->children.end());

            this->deleteKeyFromParent(parent, currentNode ,parent->keys[leftSibilingIndex]);
            return;
        }

        // try merge with right node
        if (rightSibilingNode != nullptr){
            // move everything to the left node and delete right node
            currentNode->keys.push_back(parent->keys[rightSibilingIndex - 1]);
            currentNode->keys.insert(currentNode->keys.end(), rightSibilingNode->keys.begin(), rightSibilingNode->keys.end());
            currentNode->children.insert(currentNode->children.end(), rightSibilingNode->children.begin(), rightSibilingNode->children.end());

            this->deleteKeyFromParent(parent, rightSibilingNode, parent->keys[rightSibilingIndex - 1]);
            return;
        }
    } 
}

TreeNode* BPlusTree::getLeftNonLeafNode(TreeNode* curNode){
    TreeNode* parent = curNode->parent;
    if (parent == nullptr){
        return nullptr;
    }

    for (int i = 1; i < parent->children.size(); ++i){
        if (parent->children[i] == curNode){
            return parent->children[i-1];
        }
    }
    return nullptr;
}

TreeNode* BPlusTree::getRightNonLeafNode(TreeNode* curNode){
    TreeNode* parent = curNode->parent;
    if (parent == nullptr){
        return nullptr;
    }

    for (int i = 0; i < parent->children.size() - 1; ++i){
        if (parent->children[i] == curNode){
            return parent->children[i+1];
        }
    }
    return nullptr;
}



void BPlusTree::modifyParentKey(TreeNode* parentNode, int oldKey, int newKey){

    bool oldKeyFound = false;
    for (int i = 0; i < parentNode->keys.size(); ++i){
        if (parentNode->keys[i] == oldKey){
            parentNode->keys[i] = newKey;
            oldKeyFound = true;
            break;
        }
    }

    // if cannot find old key in this parent, recursively find parent
    if (!oldKeyFound && parentNode->parent != nullptr){
        this->modifyParentKey(parentNode->parent, oldKey, newKey);
    }

    return;
}