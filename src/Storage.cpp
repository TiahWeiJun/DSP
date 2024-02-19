#include "Storage.h"
#include <iostream>


Storage::Storage(unsigned int storageSize) {
    this->totalBlocks = storageSize / blockSize;
    this->numFilledBlocks = 0;
    this->curBlockIndex = 0;
    this->blockLocations.push_back(Block({}));
}


void Storage::StoreRecord(MovieRecord movieRecord) {

    // Check enough storage
    if (this->numFilledBlocks == totalBlocks){
        std::cout << "No more space in database storage";
        return;
    }

    int sizemovieRecord = sizeof(movieRecord);

    if (sizemovieRecord <= this->blockLocations[curBlockIndex].remainingBlockSize){
        Block curBlock = this->blockLocations[curBlockIndex];
        curBlock.recordsList.push_back(movieRecord);
        curBlock.remainingBlockSize -= sizemovieRecord;
        this->blockLocations[curBlockIndex] = curBlock;
    } else {
        Block newBlock = Block{};
        newBlock.recordsList.push_back(movieRecord);
        newBlock.remainingBlockSize -= sizemovieRecord;

        this->blockLocations.push_back(newBlock);
        this->curBlockIndex += 1;
        this->numFilledBlocks += 1;
    }

   
}

void Storage::PrintStatsForStorage(int totalRecordCount, int sizeRecord){
    cout << "Statistics for Database:" << "\n\n";

    int numBlocks = this->blockLocations.size();

    int recordsInBlock = this->blockLocations[0].recordsList.size();
    cout << "Total records: "<< totalRecordCount << "\n";
    cout << "Size of record: "<< sizeRecord << " bytes \n";
    cout << "Number of records in 1 block: "<< recordsInBlock << "\n";
    cout << "Number of Blocks used: "<< numBlocks << "\n\n\n";
}

void Storage::LinearScan(int key){
    int totalCount = 0;
    int dataBlockAccess = 0;
    float totalAvgRating = 0.0;

    // Start Time
    auto start = std::chrono::high_resolution_clock::now();

    for (Block block: this->blockLocations){
        dataBlockAccess += 1;
        for (MovieRecord record: block.recordsList){
            if (record.numVotes == key){
                totalCount += 1;
                totalAvgRating += record.averageRating;
            }
        }
    }

    // End Time
    auto end = std::chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    
    cout << "Linear Scan Data Block Access: " << dataBlockAccess << "\n";
    cout << "Linear Scan Total Records Fetched: " << totalCount << "\n";
    cout << "Linear Scan Data Running Time: " << duration.count() * 1000 << " milliseconds" << "\n";;
    cout << "\n";
}

void Storage::LinearScanRange(int startKey, int endKey){
    int totalCount = 0;
    int dataBlockAccess = 0;
    float totalAvgRating = 0.0;
    
    // Start Time
    auto start = std::chrono::high_resolution_clock::now();
    for (Block block: this->blockLocations){
        dataBlockAccess += 1;
        for (MovieRecord record: block.recordsList){
            
            if (record.numVotes >= startKey && record.numVotes <= endKey){
                totalAvgRating += record.averageRating;
                totalCount += 1;
                
            }
        }
    }

    // End Time
    auto end = std::chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    
    cout << "Linear Scan Data Block Access: " << dataBlockAccess << "\n";
    cout << "Linear Scan Total Records Fetched: " << totalCount << "\n";
    cout << "Linear Scan Data Running Time: " << duration.count() * 1000 << " milliseconds" << "\n";
    cout << "\n";
}

void Storage::LinearScanDelete(int deleteKey){
    // Start Time
    auto start = std::chrono::high_resolution_clock::now();

    int dataBlockAccess = 0;

    for (Block block: this->blockLocations){
        dataBlockAccess += 1;
        for (MovieRecord record: block.recordsList){
            // simulate delete record
        }
    }

    // End Time
    auto end = std::chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    
    cout << "Linear Scan Delete Data Block Access: " << dataBlockAccess << "\n";
    cout << "Linear Scan Delete Running Time: " << duration.count() * 1000 << " milliseconds" << "\n";;
    cout << "\n";
}