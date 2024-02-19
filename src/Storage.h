#ifndef B_PLUS_TREE_PROJECT_STORAGE_H
#define B_PLUS_TREE_PROJECT_STORAGE_H

#include <vector>
#include <memory>
#include <cstring>
#include <string>

using namespace std;

const unsigned int blockSize = 200;

struct MovieRecord {
    int numVotes;
    float averageRating;
    char tconst[9];

    MovieRecord(string _tconst, float _averageRating, int _numVotes ){
        this->numVotes = _numVotes;
        this->averageRating = _averageRating;
        strcpy(tconst, _tconst.c_str());

    }
};

struct Block {
    int remainingBlockSize;
    vector<MovieRecord> recordsList;

    Block(){
        this->remainingBlockSize = blockSize - 4; // minus 4 bytes to store remaining block size
    }
};



class Storage {
    public:
        vector<Block> blockLocations;
        int totalBlocks;
        int numFilledBlocks;
        int curBlockIndex;

        Storage(unsigned int storageSize);

        void StoreRecord(MovieRecord movieRecord);
        void PrintStatsForStorage(int totalRecordCount, int sizeRecord);
        void LinearScan(int key);
        void LinearScanRange(int startKey, int endKey);
        void LinearScanDelete(int deleteKey);
};

#endif 
