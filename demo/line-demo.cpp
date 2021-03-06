#include <opencv2/opencv.hpp>
#include <opencv2/line_descriptor/descriptor.hpp>
// DBoW2

#include "DBoW2.h" // defines Surf64Vocabulary and Surf64Database
#include "Frame.h"
#include <iostream>     // std::cin, std::cout
#include <iterator>     // std::istream_iterator
#include <unordered_map>
#include <utility>      // std::pair
//#include "DUtils.h"
//#include "DUtilsCV.h" // defines macros CVXX
//#include "DVision.h"

using namespace cv;
using namespace cv::line_descriptor;
using namespace std;
using namespace DBoW2;


// ----------------------------------------------------------------------------

void printHelp(){
    printf("usage: ./line-demo [--mode=CREATEFRAME|CREATELINEVOC|CREATEORBVOC|CREATEORBDB|CREATELINEDB|QUERYLINE|QUERYORB|QUERYBOTH] [<images path>] [-k <branch number>] [-l <level number>] [-d <database file name>] [-s <start index>] [-e <end index>] [-v [<vocabulary file name>]]\n");
}

bool readGroundTruth(string gFileName, vector<vector<double> > &groundTruth) {
    ifstream infile;
    infile.open(gFileName.c_str());
    std::string temp;
    if(!infile.is_open()) {
        printf("cannot open file %s\n", gFileName.c_str());
        return false;
    }
    while (std::getline(infile, temp)) {
        std::istringstream buffer(temp);
        std::vector<double> line((std::istream_iterator<double>(buffer)), std::istream_iterator<double>());
        groundTruth.push_back(line);
    }
    infile.close();
    return true;
}
void createFrames(string dir, int startNumber, int endNumber, string gtFileName = "") {
  assert(endNumber - startNumber >= 0);
  vector<vector<double> > groundTruth;
  bool hasGT = readGroundTruth(gtFileName, groundTruth);
  char fileName[256];
  char fileName1[256];
  cout << "Extracting Line and features from: "<< dir << endl;
  Ptr<BinaryDescriptor>   lbd = BinaryDescriptor::createBinaryDescriptor();
  for(int i = startNumber; i <= endNumber; ++i)
  {
    sprintf(fileName, "%06d.png", i);
    sprintf(fileName1, "%06d.yml", i);
    string imgName = dir + fileName;
    string outFileName = dir + fileName1;
    Frame frame(imgName, i);
    frame.detectOrbFeatures();
    frame.detectLineFeatures();
    if(hasGT && i < groundTruth.size()) {
        frame.setGroundTruth(groundTruth[i]);
    }
    else {
        printf("warning: ground truth not found for image %s\n", fileName);
    }
    frame.writeToFile(outFileName);
    if(i % 100 == 0)
        printf("create features for image %s\n", imgName.c_str());
  }
}

void loadLineFeaturesFromFile(vector<vector<Mat> > &lineFeatures, string dir, int startNumber, int endNumber) {
  assert(endNumber - startNumber >= 0);
  char fileName[256];
  char fileName1[256];
  cout << "loading line features from: "<< dir << endl;
  for(int i = startNumber; i <= endNumber; ++i)
  {
    sprintf(fileName, "%06d.png", i);
    sprintf(fileName1, "%06d.yml", i);
    string imgName = dir + fileName;
    string inFileName = dir + fileName1;
    Frame frame;
    frame.loadFromFile(inFileName);
    if(frame.getLineFeatureSize() < 1)
        printf("empty line feature\n");
    lineFeatures.push_back(frame.getLineFeatureDescs());
    if(i % 100 == 0)
        printf("loaded line features for image %s\n", imgName.c_str());
  }
  /*for(uint i = 0; i < lineFeatures.size(); ++i) {
      for(uint j = 0; j < lineFeatures[i].size(); ++j) {
          std::cout << lineFeatures[i][j] << std::endl;
      }
  }*/
}

void loadFramesFromFile(vector<Frame> &frameVec, string dir, int startNumber, int endNumber) {
  assert(endNumber - startNumber >= 0);
  char fileName[256];
  char fileName1[256];
  cout << "loading frames from: "<< dir << endl;
  for(int i = startNumber; i <= endNumber; ++i)
  {
    sprintf(fileName, "%06d.png", i);
    sprintf(fileName1, "%06d.yml", i);
    string imgName = dir + fileName;
    string inFileName = dir + fileName1;
    Frame frame;
    frame.loadFromFile(inFileName);
    if(frame.getOrbFeatureSize() < 1)
        printf("empty orb feature\n");
    if(frame.getLineFeatureSize() < 1)
        printf("empty line feature\n");
    if(frame.getFramePose().size() < 1)
        printf("empty ground truth\n");
    frameVec.push_back(frame);
    if(i % 100 == 0)
        printf("loaded frame for image %s\n", imgName.c_str());
  }
  /*for(uint i = 0; i < orbFeatures.size(); ++i) {
      for(uint j = 0; j < orbFeatures[i].size(); ++j) {
          std::cout << orbFeatures[i][j] << std::endl;
      }
  }*/
}

void loadOrbFeaturesFromFile(vector<vector<Mat> > &orbFeatures, string dir, int startNumber, int endNumber) {
  assert(endNumber - startNumber >= 0);
  char fileName[256];
  char fileName1[256];
  cout << "loading orb features from: "<< dir << endl;
  for(int i = startNumber; i <= endNumber; ++i)
  {
    sprintf(fileName, "%06d.png", i);
    sprintf(fileName1, "%06d.yml", i);
    string imgName = dir + fileName;
    string inFileName = dir + fileName1;
    Frame frame;
    frame.loadFromFile(inFileName);
    if(frame.getOrbFeatureSize() < 1)
        printf("empty orb feature\n");
    orbFeatures.push_back(frame.getOrbFeatureDescs());
    if(i % 100 == 0)
        printf("loaded orb features for image %s\n", imgName.c_str());
  }
  /*for(uint i = 0; i < orbFeatures.size(); ++i) {
      for(uint j = 0; j < orbFeatures[i].size(); ++j) {
          std::cout << orbFeatures[i][j] << std::endl;
      }
  }*/
}

void loadBothFeaturesFromFile(vector<vector<Mat> > &orbFeatures, vector<vector<Mat> > &lineFeatures, string dir, int startNumber, int endNumber) {
  assert(endNumber - startNumber >= 0);
  char fileName[256];
  char fileName1[256];
  cout << "loading features from: "<< dir << endl;
  for(int i = startNumber; i <= endNumber; ++i)
  {
    sprintf(fileName, "%06d.png", i);
    sprintf(fileName1, "%06d.yml", i);
    string imgName = dir + fileName;
    string inFileName = dir + fileName1;
    Frame frame;
    frame.loadFromFile(inFileName);
    if(frame.getOrbFeatureSize() < 1 || frame.getLineFeatureSize() < 1)
        printf("empty feature\n");
    orbFeatures.push_back(frame.getOrbFeatureDescs());
    lineFeatures.push_back(frame.getLineFeatureDescs());
    if(i % 100 == 0)
        printf("loaded features for image %s\n", imgName.c_str());
  }
}

void loadFeatures(vector<vector<Mat> > &features, string dir, int startNumber, int imgNumber)
{
  //features.clear();
  assert(imgNumber > 0);
  //features.reserve(imgNumber);

  char fileName[256];
  cout << "Extracting Line features from: "<< dir << endl;
  Ptr<BinaryDescriptor>   lbd = BinaryDescriptor::createBinaryDescriptor();
  for(int i = startNumber; i < imgNumber; ++i)
  {
//    stringstream ss;
//    ss << "images/image" << i << ".png";
    sprintf(fileName, "%06d.png", i);
    string imgName = dir + fileName;

    //double startTime = (double)getTickCount();
    cv::Mat image = cv::imread(imgName, 0);
    if(image.empty()) {
        printf("cannot load image %s\n", imgName.c_str());
        continue;
    }
    //double loadEndTime = (double)getTickCount();
    vector<KeyLine> lines;
    Mat linesDescriptors;
    //lsd->detect(image, lines, 1, 1/*, mask0*/);
    lbd->detect(image, lines/*, mask0*/);
    //double detectEndTime = (double)getTickCount();
    lbd->compute(image,lines,linesDescriptors);
    //double computeDescEndTime = (double)getTickCount();
    vector<Mat> imgDescVec;
    imgDescVec.resize(linesDescriptors.rows);
    //printf("desc rows %d, cols %d\n", linesDescriptors.rows, linesDescriptors.cols);
    for(size_t j = 0; j < imgDescVec.size(); ++j) {
        //imgDescVec2d[j].resize(FLBD::L);
        imgDescVec[j] = linesDescriptors.row(j);
    }
    //double changeStructureEndTime = (double)getTickCount();
    features.push_back(imgDescVec);
    if(i % 100 == 0)
        printf("loaded features for image %s\n", imgName.c_str());
    //changeStructure(descriptors, features.back(), surf.descriptorSize());
  }
}

void createORBVoc(const vector<vector<Mat> > &features, int vocBranchNumber, int vocLevelNumber, string vocFileName)
{
    // branching factor and depth levels 
    const WeightingType weight = TF_IDF;
    const ScoringType score = L1_NORM;

    //Surf64Vocabulary voc(k, L, weight, score);
    ORBVocabulary voc(vocBranchNumber, vocLevelNumber, weight, score);

    cout << "Creating a small " << vocBranchNumber << "^" << vocLevelNumber << " vocabulary..." << endl;
    voc.create(features);
    cout << "... done!" << endl;

    cout << "Vocabulary information: " << endl
        << voc << endl << endl;

    // lets do something with this vocabulary
    /*cout << "Matching images against themselves (0 low, 1 high): " << endl;
    BowVector v1, v2;
    for(int i = 0; i < imageNumber; i++)
    {
        voc.transform(features[i], v1);
        for(int j = 0; j < imageNumber; j++)
        {
            voc.transform(features[j], v2);

            double score = voc.score(v1, v2);
            cout << "Image " << i << " vs Image " << j << ": " << score << endl;
        }
    }*/

    // save the vocabulary to disk
    cout << endl << "Saving vocabulary..." << endl;
    // voc.save("small_voc.yml.gz");
    if(vocFileName.empty()) {
        char fileName[256];
        sprintf(fileName, "%d_level_%d_voc.yml.gz", vocBranchNumber, vocLevelNumber);
        vocFileName = fileName;
    }
    voc.save(vocFileName);
    cout << "Done" << endl;
}

void createLBDVoc(const vector<vector<Mat> > &features, int vocBranchNumber, int vocLevelNumber, string vocFileName)
{
    // branching factor and depth levels 
    const WeightingType weight = TF_IDF;
    const ScoringType score = L1_NORM;

    //Surf64Vocabulary voc(k, L, weight, score);
    LBDVocabulary voc(vocBranchNumber, vocLevelNumber, weight, score);

    cout << "Creating a small " << vocBranchNumber << "^" << vocLevelNumber << " vocabulary..." << endl;
    voc.create(features);
    cout << "... done!" << endl;

    cout << "Vocabulary information: " << endl
        << voc << endl << endl;

    // lets do something with this vocabulary
    /*cout << "Matching images against themselves (0 low, 1 high): " << endl;
    BowVector v1, v2;
    for(int i = 0; i < imageNumber; i++)
    {
        voc.transform(features[i], v1);
        for(int j = 0; j < imageNumber; j++)
        {
            voc.transform(features[j], v2);

            double score = voc.score(v1, v2);
            cout << "Image " << i << " vs Image " << j << ": " << score << endl;
        }
    }*/

    // save the vocabulary to disk
    cout << endl << "Saving vocabulary..." << endl;
    // voc.save("small_voc.yml.gz");
    if(vocFileName.empty()) {
        char fileName[256];
        sprintf(fileName, "%d_level_%d_voc.yml.gz", vocBranchNumber, vocLevelNumber);
        vocFileName = fileName;
    }
    voc.save(vocFileName);
    cout << "Done" << endl;
}

void createORBDatabase(const vector<Frame> &framesVec, string vocFileName, string dbFileName)
{
  cout << "Creating ORB database..." << endl;

  // load the vocabulary from disk

  ORBVocabulary voc(vocFileName);
  
  ORBDatabase db(voc, false, 0); // false = do not use direct index
  // (so ignore the last param)
  // The direct index is useful if we want to retrieve the features that 
  // belong to some vocabulary node.
  // db creates a copy of the vocabulary, we may get rid of "voc" now

  // add frames to the database
  for(size_t i = 0; i < framesVec.size(); i++)
  {
      int frameID = framesVec[i].getID();
      db.add((framesVec[i].getOrbFeatureDescs()), frameID);
      db.addPose(frameID, framesVec[i].getFramePose());
  }


  cout << "... done!" << endl;

  cout << "ORB Database information: " << endl << db << endl;


  // we can save the database. The created file includes the vocabulary
  // and the entries added
  cout << "Saving ORB database to: " << dbFileName<< " ..." << endl;
  db.save(dbFileName);
  cout << "... done!" << endl;
}

void createLBDDatabase(const vector<Frame> &framesVec, string vocFileName, string dbFileName)
{
  cout << "Creating LBD database..." << endl;

  // load the vocabulary from disk

  LBDVocabulary voc(vocFileName);
  
  LBDDatabase db(voc, false, 0); // false = do not use direct index
  // (so ignore the last param)
  // The direct index is useful if we want to retrieve the features that 
  // belong to some vocabulary node.
  // db creates a copy of the vocabulary, we may get rid of "voc" now

  // add images to the database
  for(size_t i = 0; i < framesVec.size(); i++)
  {
      int frameID = framesVec[i].getID();
      db.add((framesVec[i].getLineFeatureDescs()), frameID);
      db.addPose(frameID, framesVec[i].getFramePose());
  }

  cout << "... done!" << endl;

  cout << " LIne Database information: " << endl << db << endl;


  // we can save the database. The created file includes the vocabulary
  // and the entries added
  cout << "Saving database to: " << dbFileName<< " ..." << endl;
  db.save(dbFileName);
  cout << "... done!" << endl;
  
  // once saved, we can load it again  
  /*cout << "Retrieving database once again..." << endl;
  LBDDatabase db2("KITTI00_line_K9_L3_db.yml.gz");
  cout << "... done! This is: " << endl << db2 << endl;*/
}

void createDatabase(const vector<vector<Mat> > &features, int vocBranchNumber, int vocLevelNumber)
{
  cout << "Creating database..." << endl;

  // load the vocabulary from disk
  char vocFileName[256];
  sprintf(vocFileName, "KITTI_line_branch%d_level_%d_voc.yml.gz", vocBranchNumber, vocLevelNumber);

  LBDVocabulary voc(vocFileName);
  
  LBDDatabase db(voc, false, 0); // false = do not use direct index
  // (so ignore the last param)
  // The direct index is useful if we want to retrieve the features that 
  // belong to some vocabulary node.
  // db creates a copy of the vocabulary, we may get rid of "voc" now

  // add images to the database
  for(uint i = 0; i < features.size(); i++)
  {
    db.add(features[i]);
  }

  cout << "... done!" << endl;

  cout << "Database information: " << endl << db << endl;


  // we can save the database. The created file includes the vocabulary
  // and the entries added
  cout << "Saving database..." << endl;
  char dbFileName[256];
  sprintf(dbFileName, "KITTI_line_branch%d_level_%d_db.yml.gz", vocBranchNumber, vocLevelNumber);
  db.save(dbFileName);
  cout << "... done!" << endl;
  
  // once saved, we can load it again  
  /*cout << "Retrieving database once again..." << endl;
  LBDDatabase db2("KITTI00_line_K9_L3_db.yml.gz");
  cout << "... done! This is: " << endl << db2 << endl;*/
}

void addFramesToDatabase(const vector<Frame> &framesVec, ORBDatabase &db, string dbFileName)
{
  cout << "Adding frames to Orb database..." << endl;

  for(size_t i = 0; i < framesVec.size(); i++)
  {
      int frameID = framesVec[i].getID();
    db.add((framesVec[i].getOrbFeatureDescs()), frameID);
    db.addPose(frameID, framesVec[i].getFramePose());
  }

  cout << "... done!" << endl;

  cout << "Orb Database information: " << endl << db << endl;


  // we can save the database. The created file includes the vocabulary
  // and the entries added
  cout << "Saving orb database..." << endl;
  db.save(dbFileName);
  cout << "... done!" << endl;
  
  // once saved, we can load it again  
  /*cout << "Retrieving database once again..." << endl;
  LBDDatabase db2("KITTI00_line_K9_L3_db.yml.gz");
  cout << "... done! This is: " << endl << db2 << endl;*/
}

void addFramesToDatabase(const vector<Frame> &framesVec, LBDDatabase &db, string dbFileName)
{
  cout << "Adding frames to Line database..." << endl;

  for(size_t i = 0; i < framesVec.size(); i++)
  {
      int frameID = framesVec[i].getID();
    db.add(framesVec[i].getLineFeatureDescs(), frameID);
    db.addPose(frameID, framesVec[i].getFramePose());
  }

  cout << "... done!" << endl;

  cout << "Line Database information: " << endl << db << endl;


  // we can save the database. The created file includes the vocabulary
  // and the entries added
  cout << "Saving Line database..." << endl;
  db.save(dbFileName);
  cout << "... done!" << endl;
  
  // once saved, we can load it again  
  /*cout << "Retrieving database once again..." << endl;
  LBDDatabase db2("KITTI00_line_K9_L3_db.yml.gz");
  cout << "... done! This is: " << endl << db2 << endl;*/
}

void addImagesToDatabase(const vector<vector<Mat> > &features, LBDDatabase &db, string dbFileName)
{
  cout << "Adding images to database..." << endl;

  for(size_t i = 0; i < features.size(); i++)
  {
    db.add(features[i]);
  }

  cout << "... done!" << endl;

  cout << "Database information: " << endl << db << endl;


  // we can save the database. The created file includes the vocabulary
  // and the entries added
  cout << "Saving database..." << endl;
  db.save(dbFileName);
  cout << "... done!" << endl;
  
  // once saved, we can load it again  
  /*cout << "Retrieving database once again..." << endl;
  LBDDatabase db2("KITTI00_line_K9_L3_db.yml.gz");
  cout << "... done! This is: " << endl << db2 << endl;*/
}

QueryResults queryImageInDataBase(const LBDDatabase &db, const Mat &image) {
    QueryResults results;
    if(image.empty()) {
        printf("query image is empty!\n");
        return results;
    }
    vector<KeyLine> lines;
    Mat linesDescriptors;
    //Ptr<LSDDetector>        lsd = LSDDetector::createLSDDetector();
    Ptr<BinaryDescriptor>   lbd = BinaryDescriptor::createBinaryDescriptor();

    //lsd->detect(image, lines, 1, 1/*, mask0*/);
    lbd->detect(image, lines/*, mask0*/);
    lbd->compute(image,lines,linesDescriptors);
    vector<Mat> imgDescVec;
    imgDescVec.resize(linesDescriptors.rows);
    for(size_t j = 0; j < imgDescVec.size(); ++j) {
        imgDescVec[j] = linesDescriptors.row(j);
    }
    db.query(imgDescVec, results, 4);
    cout << "query result is: " << results << endl;
    return results;
}

void queryBoth(const vector<vector<Mat> > &orbFeatures, const ORBDatabase &orbDB, const vector<vector<Mat> > &lineFeatures, const LBDDatabase &lineDB) {
    assert(orbFeatures.size() == lineFeatures.size());
    for(size_t i = 0; i < orbFeatures.size(); ++i) {
        QueryResults orbResults;
        orbDB.query(orbFeatures[i], orbResults, 4);
        QueryResults lineResults;
        lineDB.query(lineFeatures[i], lineResults, 4);
    }
}
/*struct MyQueryResult {
    //std::pair<uint, uint> ids;
    int id;
    double score;
};*/
typedef std::pair<int, double> MyPair;
//sort in decending order
struct MyCmp {
    bool operator()(const MyPair &lhs, const MyPair &rhs) {
        return lhs.second > rhs.second;
    }
};
//scale results to [0.5, 1]
void scaleSortedResult(QueryResults &resultVec, double minScore, double maxScore, double newMin = 0.5, double newMax = 1) {
    for(size_t i = 0; i < resultVec.size(); ++i) {
        double scaledScore = resultVec[i].Score;
        if(scaledScore <= minScore)
            scaledScore = minScore;
        else if(scaledScore >= maxScore)
            scaledScore = maxScore;
        scaledScore = (newMax - newMin) * (scaledScore - minScore) / (maxScore - minScore) + newMin;
        resultVec[i].Score = scaledScore;
    }
}

void queryBothFrame(vector<Frame> &frames, ORBDatabase &orbDB, LBDDatabase &lineDB, int orbResultSize = 5, int lineResultSize = 5) {
    string filename = "queryResult.yml";
    cv::FileStorage resultFS(filename.c_str(), cv::FileStorage::WRITE);
    if(!resultFS.isOpened()) throw string("Could not open file ") + filename;
    resultFS << "result" << "["; //result
    for(size_t i = 0; i < frames.size(); ++i) {
        resultFS<<"{";
        printf("queyr for id %d\n", frames[i].getID());
        QueryResults orbResults, lineResults;
        orbDB.query(frames[i].getOrbFeatureDescs(), orbResults, orbResultSize);
        scaleSortedResult(orbResults, orbResults.back().Score, orbResults.front().Score);
        printf("Orb query result:\n");
        std::cout << orbResults << std::endl;
        lineDB.query(frames[i].getLineFeatureDescs(), lineResults, lineResultSize);
        scaleSortedResult(lineResults, lineResults.back().Score, lineResults.front().Score);
        printf("line query result:\n");
        std::cout << lineResults << std::endl;
        unordered_map<int, double> orbResultMap;
        unordered_map<int, double> lineResultMap;
        for(uint k = 0; k < orbResults.size(); ++k) {
            orbResultMap.insert(std::pair<int, double>(orbResults[k].Id, orbResults[k].Score));
        }
        for(uint k = 0; k < lineResults.size(); ++k) {
            lineResultMap.insert(std::pair<int, double>(lineResults[k].Id, lineResults[k].Score));
        }
        unordered_map<int, double>::iterator orbMapIt;
        unordered_map<int, double>::iterator lineMapIt;
        unordered_map<int, double> totalResultMap;
        for(orbMapIt = orbResultMap.begin(); orbMapIt != orbResultMap.end(); ++orbMapIt) {
            double totalScore = 0.;
            int orbKey = (int)(orbMapIt->first);
            double orbScore = orbMapIt->second;
            //TODO: delete this line
            //assert(totalResultMap.find(orbKey) == totalResultMap.end());
            lineMapIt = lineResultMap.find(orbKey);
            //if line query has same id as orb result
            if(lineMapIt != lineResultMap.end()) {
                double lineScore = lineMapIt->second;
                totalScore = 0.5 * orbScore + 0.5 * lineScore;
            }
            else {
                totalScore = 0.8 * orbScore;
            }
            totalResultMap.insert(std::make_pair(orbKey, totalScore));
        }
        for(lineMapIt = lineResultMap.begin(); lineMapIt != lineResultMap.end(); ++lineMapIt) {
            int lineKey = (int)(lineMapIt->first);
            if(totalResultMap.find(lineKey) != totalResultMap.end())
                continue;
            double lineScore = lineMapIt->second;
            totalResultMap.insert(std::make_pair(lineKey, 0.8 * lineScore));
        }
        std::vector<MyPair> queryResultVec(totalResultMap.begin(), totalResultMap.end());
        std::sort(queryResultVec.begin(), queryResultVec.end(), MyCmp());
        //scaleSortedResult(queryResultVec, queryResultVec.back().second, queryResultVec.front().second, 0.5, 1.0);
        vector<double> queryPose = frames[i].getFramePose();
        resultFS << "queryImage" << "{";
        resultFS << "frameID" << frames[i].getID();
        resultFS << "frameName" << frames[i].getImageName();
        resultFS << "framePose" << frames[i].getFramePose();
        resultFS << "}"; //query image

        resultFS << "queryResults" << "[";
        printf("query frame pose:(%lf %lf)\n", queryPose[3], queryPose[11]);
        printf("final result:\n");
        for(uint j = 0; j < queryResultVec.size(); ++j) {
            int id = queryResultVec[j].first;
            double score = queryResultVec[j].second;
            vector<double> resultPose;
            if(!orbDB.getPose(id, resultPose)) {
                assert(lineDB.getPose(id, resultPose));
            }
            resultFS << "{";
            resultFS << "frameID" << id;
            resultFS << "score" << score;
            resultFS << "pose" << resultPose;
            resultFS << "}";
            printf("id: %d, score %lf: pose: (%lf %lf)\n", id, score, resultPose[3], resultPose[11]);
        }
        printf("--------------------------------------------------\n");
        resultFS << "]"; //query result
        resultFS << "}";
    }
    resultFS << "]"; //result
    resultFS.release();
}

void queryOrbFrame(vector<Frame> &frames, ORBDatabase &db, int resultSize = 5) {
    string filename = "queryOrbResult.yml";
    cv::FileStorage resultFS(filename.c_str(), cv::FileStorage::WRITE);
    if(!resultFS.isOpened()) throw string("Could not open file ") + filename;
    resultFS << "result" << "["; //result
    for(size_t i = 0; i < frames.size(); ++i) {
        resultFS<<"{";
        resultFS << "queryImage" << "{";
        resultFS << "frameID" << frames[i].getID();
        resultFS << "frameName" << frames[i].getImageName();
        resultFS << "framePose" << frames[i].getFramePose();
        resultFS << "}"; //query image
        QueryResults results;
        db.query(frames[i].getOrbFeatureDescs(), results, resultSize);
        scaleSortedResult(results, results.back().Score, results.front().Score);
        resultFS << "queryResults" << "[";
        for(size_t j = 0; j < results.size(); ++j) {
            int id = (int)results[j].Id;
            double score = (double)results[j].Score;
            vector<double> resultPose;
            assert(db.getPose(id, resultPose));
            resultFS << "{";
            resultFS << "frameID" << id;
            resultFS << "score" << score;
            resultFS << "pose" << resultPose;
            resultFS << "}";
        }
        resultFS << "]";
        resultFS << "}";

        //printf("query image %d done with %d\n", i, correct);
    }
    resultFS << "]"; //result
    resultFS.release();
}

void queryLineFrame(vector<Frame> &frames, LBDDatabase &db, int resultSize = 5) {
    string filename = "queryLineResult.yml";
    cv::FileStorage resultFS(filename.c_str(), cv::FileStorage::WRITE);
    if(!resultFS.isOpened()) throw string("Could not open file ") + filename;
    resultFS << "result" << "["; //result
    for(size_t i = 0; i < frames.size(); ++i) {
        resultFS<<"{";
        resultFS << "queryImage" << "{";
        resultFS << "frameID" << frames[i].getID();
        resultFS << "frameName" << frames[i].getImageName();
        resultFS << "framePose" << frames[i].getFramePose();
        resultFS << "}"; //query image
        QueryResults results;
        db.query(frames[i].getLineFeatureDescs(), results, resultSize);
        scaleSortedResult(results, results.back().Score, results.front().Score);
        resultFS << "queryResults" << "[";
        for(size_t j = 0; j < results.size(); ++j) {
            int id = (int)results[j].Id;
            double score = (double)results[j].Score;
            vector<double> resultPose;
            assert(db.getPose(id, resultPose));
            resultFS << "{";
            resultFS << "frameID" << id;
            resultFS << "score" << score;
            resultFS << "pose" << resultPose;
            resultFS << "}";
        }
        resultFS << "]";
        resultFS << "}";
    }
    resultFS << "]"; //result
    resultFS.release();
}

void queryOrb(const vector<vector<Mat> > &features, const ORBDatabase &db) {
    double truePositive = 0;
    double falsePositive = 0;
    double falseNegative = 0;
    for(size_t i = 0; i < features.size(); ++i) {
        QueryResults results;
        db.query(features[i], results, 4);
        bool correct = false;
        for(int j = 0; j < results.size(); ++j) {
            if(abs((int)results[j].Id - (int)i) <= 5) {
                truePositive++;
                correct = true;
                break;
            }
        }
        if(!correct){
            printf("false detection: query image %lu\n", i);
            cout << results << endl;
            sleep(1);
            falseNegative++;
        }
        //printf("query image %d done with %d\n", i, correct);
    }
    printf("recall rate: %lf\n", truePositive / (truePositive + falseNegative));
}

void queryLine(const vector<vector<Mat> > &features, const LBDDatabase &db) {
    double truePositive = 0;
    double falsePositive = 0;
    double falseNegative = 0;
    for(size_t i = 0; i < features.size(); ++i) {
        QueryResults results;
        db.query(features[i], results, 4);
        bool correct = false;
        for(int j = 0; j < results.size(); ++j) {
            if(abs((int)results[j].Id - (int)i) <= 5) {
                truePositive++;
                correct = true;
                break;
            }
        }
        if(!correct){
            printf("false detection: query image %lu\n", i);
            cout << results << endl;
            sleep(1);
            falseNegative++;
        }
        //printf("query image %d done with %d\n", i, correct);
    }
    printf("recall rate: %lf\n", truePositive / (truePositive + falseNegative));
}

int main(int argc, char** argv) {

    if(argc < 6) {
        printHelp();
        return -1;
    }
    char* mode = 0;
    enum ModeOption {UNKNOWN = -1, CREATEFRAME, CREATEORBVOC, CREATELINEVOC, CREATEBOTHVOC, CREATEORBDB, CREATELINEDB, CREATEBOTHDB, QUERYLINE, QUERYORB, QUERYBOTH, ADDORBDATA, ADDLINEDATA, ADDBOTHDATA};
    ModeOption modeOpt = UNKNOWN;
    const char *mode_option = "--mode=";
    string orbDBFileName, lineDBFileName, orbVocFileName, lineVocFileName, gtFileName;
    int vocBranchNumber = -1;
    int vocLevelNumber = -1;
    int startIndex = 0;
    int endIndex = 0;
    string dirPath;
    for(int i = 1; i < argc; ++i) {
        if(strncmp(mode_option, argv[i], strlen(mode_option)) == 0) {
            mode = argv[i] + strlen(mode_option);
            printf("mode is %s\n", mode);
            if(strncmp(mode, "CREATEFRAME", strlen(mode)) == 0) {
                modeOpt = CREATEFRAME;
                printf("set mode to CREATEFRAME\n");
            }
            else if(strncmp(mode, "CREATEORBVOC", strlen(mode)) == 0) {
                modeOpt = CREATEORBVOC;
                printf("set mode to CREATEORBVOC\n");
            }
            else if(strncmp(mode, "CREATELINEVOC", strlen(mode)) == 0) {
                modeOpt = CREATELINEVOC;
                printf("set mode to CREATELINEVOC\n");
            }
            else if(strncmp(mode, "CREATEBOTHVOC", strlen(mode)) == 0) {
                modeOpt = CREATEBOTHVOC;
                printf("set mode to CREATEBOTHVOC\n");
            }
            else if(strncmp(mode, "CREATEORBDB", strlen(mode)) == 0) {
                modeOpt = CREATEORBDB;
                printf("set mode to CREATEORBDB\n");
            }
            else if(strncmp(mode, "CREATELINEDB", strlen(mode)) == 0) {
                modeOpt = CREATELINEDB;
                printf("set mode to CREATELINEDB\n");
            }
            else if(strncmp(mode, "CREATEBOTHDB", strlen(mode)) == 0) {
                modeOpt = CREATEBOTHDB;
                printf("set mode to CREATEBOTHDB\n");
            }
            else if(strncmp(mode, "QUERYLINE", strlen(mode)) == 0) {
                modeOpt = QUERYLINE;
                printf("set mode to QUERYLINE\n");
            }
            else if(strncmp(mode, "QUERYORB", strlen(mode)) == 0) {
                modeOpt = QUERYORB;
                printf("set mode to QUERYORB\n");
            }
            else if(strncmp(mode, "QUERYBOTH", strlen(mode)) == 0) {
                modeOpt = QUERYBOTH;
                printf("set mode to QUERYBOTH\n");
            }
            else if(strncmp(mode, "ADDORBDATA", strlen(mode)) == 0) {
                modeOpt = ADDORBDATA;
                printf("set mode to ADDORBDATA\n");
            }
            else if(strncmp(mode, "ADDLINEDATA", strlen(mode)) == 0) {
                modeOpt = ADDLINEDATA;
                printf("set mode to ADDLINEDATA\n");
            }
            else if(strncmp(mode, "ADDBOTHDATA", strlen(mode)) == 0) {
                modeOpt = ADDBOTHDATA;
                printf("set mode to ADDBOTHDATA\n");
            }
            else
                modeOpt = UNKNOWN;
        }
        else if(strncmp(argv[i], "-k", 2) == 0) {
            vocBranchNumber = atoi(argv[++i]);
            printf("voc branch number %d\n", vocBranchNumber);
        }
        else if(strncmp(argv[i], "-level", 6) == 0) {
            vocLevelNumber = atoi(argv[++i]);
            printf("voc level number %d\n", vocLevelNumber);
        }
        else if(strncmp(argv[i], "-od", 3) == 0 || strncmp(argv[i], "-do", 3) == 0) {
            orbDBFileName = argv[++i];
            printf("orbDBFileName: %s\n", orbDBFileName.c_str());
        }
        else if(strncmp(argv[i], "-ld", 3) == 0 || strncmp(argv[i], "-dl", 3) == 0) {
            lineDBFileName = argv[++i];
            printf("lineDBFileName: %s\n", lineDBFileName.c_str());
        }
        else if((strncmp(argv[i], "-vo", 3) == 0) || (strncmp(argv[i], "-ov", 3) == 0)){
            orbVocFileName = argv[++i];
            printf("orbVocFileName: %s\n", orbVocFileName.c_str());
        }  
        else if((strncmp(argv[i], "-vl", 3) == 0) || (strncmp(argv[i], "-lv", 3) == 0)){
            lineVocFileName = argv[++i];
            printf("lineVocFileName: %s\n", lineVocFileName.c_str());
        }
        else if(strncmp(argv[i], "-gt", 3) == 0){
            gtFileName = argv[++i];
            printf("ground truth FileName: %s\n", gtFileName.c_str());
        }
        else if(strncmp(argv[i], "-s", 2) == 0) {
            startIndex = atoi(argv[++i]);
            printf("start index %d\n", startIndex);
        }
        else if(strncmp(argv[i], "-e", 2) == 0) {
            endIndex = atoi(argv[++i]);
            printf("end index %d\n", endIndex);
        }
        else if(argv[i][0] != '-') {
            dirPath = argv[i];
        }
    }
    if(dirPath.empty()) {
        printf("please indicate images path\n");
        printHelp();
        return -1;
    }
    if(startIndex > endIndex) {
        printf("start index not small than end index: start %d, end %d\n", startIndex, endIndex);
        printHelp();
        return -1;
    }
    if((modeOpt == CREATEBOTHVOC || modeOpt == CREATEORBVOC || modeOpt == CREATELINEVOC) && vocBranchNumber < 0) {
        printf("please indicate branch number using -k\n");
        printHelp();
        return -1;
    }
    if((modeOpt == CREATEBOTHVOC || modeOpt == CREATEORBVOC || modeOpt == CREATELINEVOC) && vocLevelNumber < 0) {
        printf("please indicate level number using -level\n");
        printHelp();
        return -1;
    }
    if(modeOpt == CREATEORBVOC && orbVocFileName.empty()) {
        printf("please indicate orb vocabulary file name using -ov\n");
        printHelp();
        return -1;
    }
    if(modeOpt == CREATELINEVOC && lineVocFileName.empty()) {
        printf("please indicate line vocabulary file name using -lv\n");
        printHelp();
        return -1;
    }
    if(modeOpt == CREATEBOTHVOC){
        printf("line voc file name %s\n", lineVocFileName.c_str());
        if(lineVocFileName.empty()) {
            printf("please indicate line vocabulary file name using -lv\n");
            printHelp();
            return -1;
        }
        if(orbVocFileName.empty()) {
            printf("please indicate orb vocabulary file name using -ov\n");
            printHelp();
            return -1;
        }
    }
    if(modeOpt == CREATEORBDB && orbVocFileName.empty()) {
        printf("please indicate orb vocabulary file name using -ov\n");
        printHelp();
        return -1;
    }
    if(modeOpt == CREATELINEDB && lineVocFileName.empty()) {
        printf("please indicate line vocabulary file name using -lv\n");
        printHelp();
        return -1;
    }
    if(modeOpt == CREATEBOTHDB) { 
        printf("line voc file name %s\n", lineVocFileName.c_str());
        if(lineVocFileName.empty()) {
            printf("please indicate line vocabulary file name using -lv\n");
            printHelp();
            return -1;
        }
        if(orbVocFileName.empty()) {
            printf("please indicate orb vocabulary file name using -ov\n");
            printHelp();
            return -1;
        }
        if(lineDBFileName.empty()) {
            printf("please indicate line database file name using -ld\n");
            printHelp();
            return -1;
        }
        if(orbDBFileName.empty()) {
            printf("please indicate orb database file name using -od\n");
            printHelp();
            return -1;
        }
    }
    if(modeOpt == CREATEORBDB && orbDBFileName.empty()) {
        printf("please indicate orb database file name using -od\n");
        printHelp();
        return -1;
    }
    if(modeOpt == CREATELINEDB && lineDBFileName.empty()) {
        printf("please indicate line database file name using -ld\n");
        printHelp();
        return -1;
    }
    if(modeOpt == ADDORBDATA || modeOpt == QUERYORB) {
        if (orbDBFileName.find("yml") == std::string::npos) {
            printf("incorrect orb database file: %s, please use -od to indicate orb database\n", orbDBFileName.c_str());
            printHelp();
            return -1;
        }
    }
    if(modeOpt == ADDLINEDATA || modeOpt == QUERYLINE) {
        if (lineDBFileName.find("yml") == std::string::npos) {
            printf("incorrect line database file: %s, please use -ld to indicate line database\n", lineDBFileName.c_str());
            printHelp();
            return -1;
        }
    }
    if(modeOpt == ADDBOTHDATA || modeOpt == QUERYBOTH) {
        if (orbDBFileName.find("yml") == std::string::npos) {
            printf("incorrect orb database file: %s, please use -od to indicate orb database\n", orbDBFileName.c_str());
            printHelp();
            return -1;
        }
        if (lineDBFileName.find("yml") == std::string::npos) {
            printf("incorrect orb database file: %s, please use -ld to indicate line database\n", orbDBFileName.c_str());
            printHelp();
            return -1;
        }
    }
    //string dir = argv[1];
    switch(modeOpt) {
        case CREATEFRAME:
            {
                printf("creating features\n");
                long startTime = getTickCount();
                createFrames(dirPath, startIndex, endIndex, gtFileName);
                //printf("feature size %lu\n", features.size());
                //loadFeatures(features, dir1, imageNumber1);
                long endFeatureTime = getTickCount();
                printf("elapsed time [load features]: %lf sec\n",double(endFeatureTime-startTime) / (double)getTickFrequency());
                break;
            }
        case CREATEORBVOC:
            {
                printf("creating orb vocabulary\n");

                vector<vector<Mat> > orbFeatures;
                //vector<vector<Mat> > lineFeatures;
                long startTime = getTickCount();
                //loadFeaturesFromFile(orbFeatures, lineFeatures, dirPath, startIndex, endIndex);
                loadOrbFeaturesFromFile(orbFeatures, dirPath, startIndex, endIndex);
                //printf("feature size %lu\n", features.size());
                //loadFeatures(features, dir1, imageNumber1);
                printf("orb feature size: %lu\n", orbFeatures.size());
                long endFeatureTime = getTickCount();
                printf("elapsed time [load features]: %lf sec\n",double(endFeatureTime-startTime) / (double)getTickFrequency());
                createORBVoc(orbFeatures, vocBranchNumber, vocLevelNumber, orbVocFileName);
                long endCreateVocTime = getTickCount();
                printf("elapsed time [create ORB Vocabulary]: %lf sec\n",double(endCreateVocTime-endFeatureTime) / (double)getTickFrequency());
                break;
            }
        case CREATELINEVOC:
            {
                printf("creating line vocabulary\n");
                //string dir0 = "/media/ys/Storage/Dataset/KITTI/10/image_0/";
                //int imageNumber0 = 1200;
                //string dir1 = "/media/ys/Storage/Dataset/KITTI/10/image_0/";
                //int imageNumber1 = 10;

                vector<vector<Mat> > lineFeatures;
                long startTime = getTickCount();
                loadLineFeaturesFromFile(lineFeatures, dirPath, startIndex, endIndex);
                //printf("feature size %lu\n", features.size());
                //loadFeatures(features, dir1, imageNumber1);
                printf("line feature size: %lu\n", lineFeatures.size());
                long endFeatureTime = getTickCount();
                printf("elapsed time [load features]: %lf sec\n",double(endFeatureTime-startTime) / (double)getTickFrequency());
                createLBDVoc(lineFeatures, vocBranchNumber, vocLevelNumber, lineVocFileName);
                long endCreateVocTime = getTickCount();
                printf("elapsed time [create Vocabulary]: %lf sec\n",double(endCreateVocTime-endFeatureTime) / (double)getTickFrequency());
                break;
            }
        case CREATEBOTHVOC:
            {
                printf("creating orb and line vocabulary\n");
                vector<vector<Mat> > orbFeatures;
                vector<vector<Mat> > lineFeatures;
                long startTime = getTickCount();
                loadBothFeaturesFromFile(orbFeatures, lineFeatures, dirPath, startIndex, endIndex);
                printf("orb feature size: %lu\n", orbFeatures.size());
                printf("line feature size: %lu\n", lineFeatures.size());
                long endFeatureTime = getTickCount();
                printf("elapsed time [load features]: %lf sec\n",double(endFeatureTime-startTime) / (double)getTickFrequency());
                createORBVoc(orbFeatures, vocBranchNumber, vocLevelNumber, orbVocFileName);
                createLBDVoc(lineFeatures, vocBranchNumber, vocLevelNumber, lineVocFileName);
                long endCreateVocTime = getTickCount();
                printf("elapsed time [create Vocabulary]: %lf sec\n",double(endCreateVocTime-endFeatureTime) / (double)getTickFrequency());
                break;
            }
        case CREATEORBDB:
            {
                printf("creating ORB Database\n");
                vector<vector<Mat> > orbFeatures;
                long startTime = getTickCount();
                vector<Frame> frameVec;
                loadFramesFromFile(frameVec, dirPath, startIndex, endIndex);
                //loadOrbFeaturesFromFile(orbFeatures, dirPath, startIndex, endIndex);
                long endFeatureTime = getTickCount();
                printf("elapsed time [load features]: %lf sec\n",double(endFeatureTime-startTime) / (double)getTickFrequency());
                createORBDatabase(frameVec, orbVocFileName, orbDBFileName);
                long endCreateVocTime = getTickCount();
                printf("elapsed time [create Vocabulary]: %lf sec\n",double(endCreateVocTime-endFeatureTime) / (double)getTickFrequency());
                break;
            }
        case CREATELINEDB:
            {
                printf("creating Line Database\n");
                vector<vector<Mat> > lineFeatures;
                long startTime = getTickCount();
                vector<Frame> frameVec;
                loadFramesFromFile(frameVec, dirPath, startIndex, endIndex);
                //loadLineFeaturesFromFile(lineFeatures, dirPath, startIndex, endIndex);
                long endFeatureTime = getTickCount();
                printf("elapsed time [load features]: %lf sec\n",double(endFeatureTime-startTime) / (double)getTickFrequency());
                createLBDDatabase(frameVec, lineVocFileName, lineDBFileName);
                //createLBDDatabase(lineFeatures, lineVocFileName, lineDBFileName);
                long endCreateVocTime = getTickCount();
                printf("elapsed time [create Vocabulary]: %lf sec\n",double(endCreateVocTime-endFeatureTime) / (double)getTickFrequency());
                break;
            }
        case CREATEBOTHDB:
            {
                printf("creating ORB and Line Database\n");
                long startTime = getTickCount();
                vector<Frame> frameVec;
                loadFramesFromFile(frameVec, dirPath, startIndex, endIndex);
                long endFeatureTime = getTickCount();
                printf("elapsed time [load features]: %lf sec\n",double(endFeatureTime-startTime) / (double)getTickFrequency());
                createORBDatabase(frameVec, orbVocFileName, orbDBFileName);
                createLBDDatabase(frameVec, lineVocFileName, lineDBFileName);
                //createORBDatabase(orbFeatures, orbVocFileName, orbDBFileName);
                //createLBDDatabase(lineFeatures, lineVocFileName, lineDBFileName);
                long endCreateVocTime = getTickCount();
                printf("elapsed time [create Vocabulary]: %lf sec\n",double(endCreateVocTime-endFeatureTime) / (double)getTickFrequency());
                break;
            }
        case QUERYORB:
            {
                printf("query image using ORB database\n");
                cout << "Retrieving ORB database: " << orbDBFileName <<endl;
                ORBDatabase orbDB(orbDBFileName);
                cout << "... done! ORB database is: " << endl << orbDB << endl;
                //vector<vector<Mat> > orbFeatures;
                //loadOrbFeaturesFromFile(orbFeatures, dirPath, startIndex, endIndex);
                //vector<vector<double> > groundTruth;
                //bool hasGT = readGroundTruth(gtFileName, groundTruth);
                //queryOrb(orbFeatures, db1);
                vector<Frame> frameVec;
                loadFramesFromFile(frameVec, dirPath, startIndex, endIndex);
                queryOrbFrame(frameVec, orbDB);
                break;
            }
        case QUERYLINE:
            {
                printf("query image using line database\n");
                cout << "Retrieving Line database: " << lineDBFileName <<endl;
                LBDDatabase lineDB(lineDBFileName);
                cout << "... done! Line database is: " << endl << lineDB << endl;
                //vector<vector<Mat> > lineFeatures;
                //loadFeatures(lineFeatures, dirPath, startIndex, endIndex);
                //vector<vector<double> > groundTruth;
                //bool hasGT = readGroundTruth(gtFileName, groundTruth);
                //queryLine(lineFeatures, db2);
                vector<Frame> frameVec;
                loadFramesFromFile(frameVec, dirPath, startIndex, endIndex);
                queryLineFrame(frameVec, lineDB);
                break;
            }
        case QUERYBOTH:
            {
                printf("query image using ORB and LINE database\n");
                cout << "Retrieving ORB database: " << orbDBFileName <<endl;
                ORBDatabase orbDB(orbDBFileName);
                cout << "... done! ORB databse is: " << endl << orbDB << endl;
                cout << "Retrieving Line database: " << lineDBFileName <<endl;
                LBDDatabase lineDB(lineDBFileName);
                cout << "... done! Line database is: " << endl << lineDB << endl;
                //vector<vector<Mat> > orbFeatures;
                //vector<vector<Mat> > lineFeatures;
                //loadBothFeaturesFromFile(orbFeatures, lineFeatures, dirPath, startIndex, endIndex);
                //vector<vector<double> > groundTruth;
                //bool hasGT = readGroundTruth(gtFileName, groundTruth);
                //queryBoth(orbFeatures, db1, lineFeatures, db2);
                vector<Frame> frameVec;
                loadFramesFromFile(frameVec, dirPath, startIndex, endIndex);
                queryBothFrame(frameVec, orbDB, lineDB, 5);
                break;
            }
        case ADDORBDATA:
            {
                cout << "Retrieving ORB database: " << orbDBFileName <<endl;
                LBDDatabase db1(orbDBFileName);
                cout << "... done! This is: " << endl << db1 << endl;
                vector<Frame> frameVec;
                loadFramesFromFile(frameVec, dirPath, startIndex, endIndex);
                addFramesToDatabase(frameVec, db1, orbDBFileName);
                break;
            }
        case ADDLINEDATA:
            {
                cout << "Retrieving Line database: " << lineDBFileName <<endl;
                LBDDatabase db2(lineDBFileName);
                cout << "... done! This is: " << endl << db2 << endl;
                vector<Frame> frameVec;
                loadFramesFromFile(frameVec, dirPath, startIndex, endIndex);
                addFramesToDatabase(frameVec, db2, lineDBFileName);
                break;
            }
        case ADDBOTHDATA:
            {
                cout << "Retrieving ORB database: " << orbDBFileName <<endl;
                LBDDatabase db1(orbDBFileName);
                cout << "... done! This is: " << endl << db1 << endl;
                cout << "Retrieving Line database: " << lineDBFileName <<endl;
                LBDDatabase db2(lineDBFileName);
                cout << "... done! This is: " << endl << db2 << endl;
                vector<Frame> frameVec;
                loadFramesFromFile(frameVec, dirPath, startIndex, endIndex);
                addFramesToDatabase(frameVec, db1, orbDBFileName);
                addFramesToDatabase(frameVec, db2, lineDBFileName);
                break;
            }
        case UNKNOWN:
            printf("unknown mode option\n");
            break;
    }
    return 0;

}
