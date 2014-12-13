//--------------------LHEtoROOT-----------------------------------------------
//Author: Dan Phan (dan@umail.ucsb.edu)
//Date: December 2014
//Description/Instructions: Go to line 33, change name of file to your LHE file
//Change tree name to your root tree name
//Change name of output file, max nEvents
//----------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "TTree.h"
#include "TFile.h"
#include "Math/Vector4D.h"

using namespace std;

//Parameters
  outputName = "dansFile";
  treeName = "Dan's Tree";
//

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LorentzVector;

int nEvents = 0;

int looper(){

  //Declare TTree and TFile
  TFile *file = new TFile(Form("%s.root", outputName), "RECREATE");
  TTree *tree = new TTree("tree", Form("%s",treeName));
  
  //Declare variables that will be stored in tree
  vector <int> pdgID;
  vector <int> status;
  vector <int> mother_1;
  vector <int> mother_2;
  vector <int> colour_1;
  vector <int> colour_2;
  vector <LorentzVector> four_momentum;  //encodes 4-momentum and position
  vector <LorentzVector> four_position;
  vector <float> row1; //row underneath <event>
  int nParticles;      //entries in row1
  int process_number;
  float weight;
  float energy_scale;
  float QED_coupling;
  float QCD_coupling;
  vector <float> wgtID;  
  
  //Match up variable with branch
  tree->Branch("pdgID", &pdgID); 
  tree->Branch("status", &status);
  tree->Branch("mother_1", &mother_1);
  tree->Branch("mother_2", &mother_2);
  tree->Branch("colour_1", &colour_1);
  tree->Branch("colour_2", &colour_2);
  tree->Branch("four_momentum", &four_momentum);
  tree->Branch("four_position", &four_position);
  tree->Branch("wgtID",&wgtID); 
  tree->Branch("nParticles",&nParticles);
  tree->Branch("process_number",&process_number);
  tree->Branch("weight",&weight);
  tree->Branch("energy_scale",&energy_scale);
  tree->Branch("QED_coupling",&QED_coupling); 
  tree->Branch("QCD_coupling",&QCD_coupling); 
 
  string line;
  bool first = false; //whether on line right below event
  bool second = false; //between <event> and </event>, turns on one line after "first", both turn off after filling tree
  bool is_wgtID = false;
  
  //Opening data file
  fstream myfile ("lhe_file_toy.lhe", ios_base::in);

  while (getline(myfile,line)){          

    //if on 2nd line below event, fill variables(row1 and wgtID entries filled in other loops)
    if (second == true) {   
      do {  
        if (line.find('#') != string::npos) break;  //if line contains "stop", stop loop
        
        //Declare temp vector
        vector <float> row;

        //iss has entire string.  Now read first part of string (pdgID) into "val" 
        istringstream iss;   //opening string stream, above getline loops past first line below "yes"
        iss.str(line);      //copying line into stream
        float val;
         
        int count = 0;
     	while (iss >> val && (count < 13)) {
     	  row.push_back(val);
     	  count++;
     	}

        //Now split row into vectors for each variable
        pdgID.push_back(row[0]);  
        status.push_back(row[1]);
        mother_1.push_back(row[2]);
        mother_2.push_back(row[3]);
        colour_1.push_back(row[4]);
        colour_2.push_back(row[5]);
        LorentzVector four_momentum_temp;
        four_momentum_temp.SetPx(row[6]);
        four_momentum_temp.SetPy(row[7]);
        four_momentum_temp.SetPz(row[8]);
        four_momentum_temp.SetE(row[9]);
        four_momentum.push_back(four_momentum_temp);
        LorentzVector four_position_temp;
        four_position_temp.SetXYZT(row[10],row[11],row[12],0);  //no time information in file, we have set t=0
        four_position.push_back(four_position_temp);
      } while (getline(myfile,line));
      first = false;
      second = false;  //after array prints, turn first, second off again
    }
 
    //Line right below <event>(row1 variables)
    if (first == true) {
      second = true; //sets up next loop
      istringstream iss;
      iss.str(line);
      float val1;

      int count1 = 0;
      while (iss >> val1 && (count1 < 6)) {
        row1.push_back(val1);
        count1++;
      }
      nParticles = row1[0];
      process_number = row1[1];
      weight = row1[2];
      energy_scale = row1[3];
      QED_coupling = row1[4];
      QCD_coupling = row1[5];
      
    }
    
    //Store weights 
    if (line.find("wgt id") != string::npos) { //if line contains wgt id, start filling wgtID
      is_wgtID = true;
      do {
        if (!(line.find("wgt id") != string::npos)) { //if line doesn't contain wgt id, stop filling and break out of loop
          break;
        }
        int begin = line.find(">");
        int end = line.find("<",begin);
        const char* str_val2 = line.substr(begin+1,end-(begin+1)).c_str(); //takes number out of line
        float val2 = atof(str_val2); 
        wgtID.push_back(val2);
      } while (getline(myfile,line));
    }

    //Check every line after </event> 
    if ((line.find("<event>") != string::npos)) {  //if line contains  "<event>", we turn first to true(begins filling row1 variables)

      nEvents++;

      if (is_wgtID == false) { //if event doesn't contain wgtID information, fill wgtID with nonsense
        for (int i_ = 0; i_ < 50; i_++) {
          wgtID.push_back(-5);
        }
      }

      if (nEvents > 1) tree->Fill();
      pdgID.clear();
      status.clear();
      mother_1.clear();
      mother_2.clear();
      colour_1.clear();
      colour_2.clear();
      wgtID.clear();
      row1.clear();
      four_momentum.clear();
      four_position.clear();

      first = true; //begins process to move onto next event
    }
    
  }

  file->cd();
  tree->Write(); 

  return 0;
}
