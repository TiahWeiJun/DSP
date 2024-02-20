#include <iostream>
#include "Storage.h"
#include "BPlusTree.h"
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;

int main() {
    Storage storage{static_cast<unsigned int>(100 * pow(10,6))};
    BPlusTree tree{nodeDegree};
    
    while(true) {
        cout << "Experiments:" << endl;
        cout << "1. Store data on disk and report statistics" << endl;
        cout << "2. Build a B+ Tree index on 'numVotes' and report statistics" << endl;
        cout << "3. Retrieve Movies with 'numVotes' == 500 and report the following statistics" << endl;
        cout << "4. Retrieve Movies with 30000 <= 'numVotes' <= 40000 and report the following statistics" << endl;
        cout << "5. Delete Movies with 'numVotes' == 1000, update B+ Tree and report the following statistics" << endl;
        cout << "Testing:" << endl;
        cout << "6. Visualize Tree" << endl;
        cout << "7. Exit" << endl;


        int choice;
        cin >> choice;

        switch(choice) {
            case 1: {
                cout << "Start Insert to Database" << "\n";

                ifstream inputFile("../data/data.txt");
                // ifstream inputFile("../data/testData.txt");

                if (!inputFile) {
                    cerr << "Failed to open the file." << endl;
                    return 1;
                }

                string line;
                getline(inputFile, line);

                int totalRecordCount = 0;
                int sizeRecord = 0;
                while(getline(inputFile, line)){
                    totalRecordCount+=1;
                    istringstream iss(line);

                    string token;
                    vector<string> tokens;
                    while (iss >> token){
                        tokens.push_back(token);
                    }
                    string tconst = tokens[0];
                    float averageRating = stof(tokens[1]);
                    int numVotes = stoi(tokens[2]);


                    MovieRecord newRecord{tconst, averageRating, numVotes};
                    sizeRecord = sizeof(newRecord);
                   
                    
                    storage.StoreRecord(newRecord);

                   
                }
                cout << "End Insert to Database" << "\n\n";

                storage.PrintStatsForStorage(totalRecordCount, sizeRecord);
                
                break;
            }

            case 2: {
                // iterate through storage to get record pointer and insert into b plus tree
                
                cout << "Start Insert into B+ Tree" << "\n";
                for (int blockIndex = 0; blockIndex < storage.blockLocations.size(); ++blockIndex) {
                    for (int recordIndex = 0; recordIndex < storage.blockLocations[blockIndex].recordsList.size(); ++recordIndex) {
                        MovieRecord record = storage.blockLocations[blockIndex].recordsList[recordIndex];
                        string recordLocation = to_string(blockIndex) + "-" + to_string(recordIndex);

                        tree.InsertIndexEntry(record.numVotes, recordLocation );    
                           
                    }
                }
                cout << "End Insert into B+ Tree" << "\n";

                tree.PrintStatsForTree();
                break;
            }

            case 3: {
                int target = 500;
                int totalCount = 0;
                float totalAvgRating = 0.0;

                // Start time
                auto start = std::chrono::high_resolution_clock::now();
                
                // get the block indexes 
                int indexBlocksAccessed = 0;
                vector<string> recordLocationList = tree.RetrieveBlocks(target, &indexBlocksAccessed);

                set<int> blockAccessSet;

                if (recordLocationList.size() != 0){
                    // go to disk and get actual record
                    for (string recordLocation: recordLocationList){

                        int blockIndex = stoi(recordLocation.substr(0, recordLocation.find("-")));
                        int recordIndex = stoi(recordLocation.substr(recordLocation.find("-")+1, recordLocation.size()));
                        
                        blockAccessSet.insert(blockIndex);
                        
                        totalCount += 1;
                        MovieRecord record = storage.blockLocations[blockIndex].recordsList[recordIndex];
                        totalAvgRating += record.averageRating;
                    }
                }

                

                // End Time
                auto end = std::chrono::high_resolution_clock::now();
                chrono::duration<double> duration = end - start;


                cout << "Statistics for Point Query:" << "\n\n";
                cout << "Index Block Access: "<< indexBlocksAccessed << "\n";
                cout << "Data Block Access: "<< blockAccessSet.size() << "\n";
                cout << "Average of “averageRatings” of the records: "<< totalAvgRating / totalCount << "\n";
                cout << "Total Records fetched: " << totalCount << "\n";
                cout << "Running time: " << duration.count() * 1000 << " milliseconds" << "\n";

                cout << "\n";

                // Do linear scan and print stats for it
                storage.LinearScan(target);

                break;
            }
            case 4: {
                int startKey = 30000;
                int endKey = 40000;
                int indexBlocksAccessed = 0;
                float totalAvgRating = 0.0;

                // Start time
                auto start = std::chrono::high_resolution_clock::now();

                vector<string> recordLocationList = tree.RetrieveBlocksByRange(startKey, endKey, &indexBlocksAccessed);

                // go to disk and get actual record
                int totalCount = 0;

                set<int> blockAccessSet;
                if (recordLocationList.size() != 0){
                    // go to disk and get actual record
                    for (string recordLocation: recordLocationList){

                        int blockIndex = stoi(recordLocation.substr(0, recordLocation.find("-")));
                        int recordIndex = stoi(recordLocation.substr(recordLocation.find("-")+1, recordLocation.size()));


                        const bool inSet = blockAccessSet.find(blockIndex) != blockAccessSet.end();
                        
                        blockAccessSet.insert(blockIndex);
                        
                        
                        totalCount += 1;
                        MovieRecord record = storage.blockLocations[blockIndex].recordsList[recordIndex];
                        totalAvgRating += record.averageRating;
                    }
                }
                
                auto end = std::chrono::high_resolution_clock::now();
                chrono::duration<double> duration = end - start;



                cout << "Statistics for Range Query:" << "\n\n";
                cout << "Index Block Access: "<< indexBlocksAccessed << "\n";
                cout << "Data Block Access: "<< blockAccessSet.size() << "\n";
                cout << "Average of “averageRatings” of the records: "<< totalAvgRating / totalCount << "\n";
                cout << "Total Records fetched: " << totalCount << "\n";
                cout << "Running time: " << duration.count() * 1000 << " milliseconds" << "\n";

                cout << "\n";

                // Do linear scan and print stats for it
                storage.LinearScanRange(startKey, endKey);

                break;
            }

            case 5:{
                int deleteKey = 1000;

                // cout << "Enter delete key" << endl;
                // int deleteKey;
                // cin >> deleteKey;

                auto start = std::chrono::high_resolution_clock::now();
                vector<string>deleteRecordLocationList = tree.DeleteIndexEntry(deleteKey);
                
                // simulate delete from storage
                for (string record: deleteRecordLocationList){
                    
                }

                auto end = std::chrono::high_resolution_clock::now();
                chrono::duration<double> duration = end - start;
                tree.PrintStatsForTree();

                cout << "Running time: " << duration.count() * 1000 << " milliseconds" << "\n\n";

                storage.LinearScanDelete(deleteKey);

                break;
            }

            case 6: {
                tree.VisualizeTree();
                break;
            }
            case 7:
                return 0;
            default:
                cout << "Invalid choice. Please choose a valid option." << endl;
        }
    }   
    return 0;
}