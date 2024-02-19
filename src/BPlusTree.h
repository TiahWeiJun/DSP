#ifndef B_PLUS_TREE_PROJECT_TREE_H
#define B_PLUS_TREE_PROJECT_TREE_H
#include <vector>
#include "Storage.h"
#include <set>

using namespace std;


const unsigned int nodeDegree = 16;

// Taking n as the max number of keys
// 4n + 8 + 8n + 8 + 1 <= 200 (block size), max n = 15, max degree = 16
class TreeNode {
    public:
        vector<int> keys; // 4n bytes
        TreeNode *parent; // 8 bytes
        vector<TreeNode*> children; // 8n + 8 bytes, list of pointers to children nodes, if not leaf node, should have n+1 elems
        vector<vector<string>*> recordLocationLists; // 8n bytes, list of list pointers to record locations (eg. "0-0", Block Index 0, Record Index 0) for duplicate handling, for leaf node, should have n elems
        TreeNode *next; // 8 bytes, pointer to next leaf node, 
        bool isLeaf; // 1 byte


        TreeNode(TreeNode *_parent, bool _isLeaf, TreeNode *_next);
};

class BPlusTree {
    private:
        int binarySearch(const std::vector<int>& arr, int value);
        TreeNode* getLeafNodeBasedOnKey(TreeNode* root, int key, int* indexBlockAccessed);
        int doesKeyExist(int key, vector<int> keyList);
        void insertParent(int key, TreeNode* oldNode, TreeNode* newNode, TreeNode* parentPointer);
        void borrowKey(TreeNode* curNode, TreeNode* newNode, bool isLeftNode, int oldFirstKey);
        void mergeLeafNodes(TreeNode* leftNode, TreeNode* rightNode, bool tookLeft);
        void modifyParentKey(TreeNode* parentNode, int oldKey, int newKey);
        void deleteKeyFromParent(TreeNode* parentNode, TreeNode* rightNode, int deleteIndex);
        TreeNode* getLeftNonLeafNode(TreeNode* curNode);
        TreeNode* getRightNonLeafNode(TreeNode* curNode);

    public:
        TreeNode* root;
        int degree; // how many children/pointers

        BPlusTree(int _degree);
        void InsertIndexEntry(int key, string recordLocation);
        vector<string> DeleteIndexEntry(int keys);
        void VisualizeTree();
        void PrintStatsForTree();
        vector<string> RetrieveBlocks(int key, int* indexBlockAccessed);
        vector<string> RetrieveBlocksByRange(int startKey, int endKey,  int* indexBlockAccessed);
        
        
};


#endif