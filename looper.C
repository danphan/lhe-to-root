//--------------------LHEtoROOT-----------------------------------------------
//Author: Dan Phan (dan@umail.ucsb.edu)
//Date: December 2014
//Description/Instructions: Go to line 22, change name of file to your LHE file
//Change tree name to your desired root tree name
//Change name of output file to your desired output filename
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
char* filename  = "/home/users/danphan/lhe-to-root/lhe_file_toy";
char* outputName = "LHE_File";
char* treeName = "LHE_Tree";

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LorentzVector;

//fill tree for only maxEvents number of events
int numEvents = 0; //initializing counter of events
int maxEvents = 1e9; //if you want to fill all events, make maxEvents huge

//function to take store reweights
float store_reweights(string line) {

  float wgt;
  int begin = line.find(">"); //beginning of number
  int end = line.find("<",begin);  //end of number
  float wgt= ::atof(line.substr(begin+1,end-(begin+1)).c_str()); //defines wgt as substring of line holding reweight
                                                                 //between begin and end
  return wgt;
}

int looper(){


  //Declare TTree and TFile
  TFile *file = new TFile(Form("%s.root", outputName), "RECREATE");
  TTree *tree = new TTree("tree", Form("%s",treeName));  //tree called "tree"
  
  //Declare variables that will be stored in tree
  vector <int> pdgID;
  vector <int> status;
  vector <int> mother_1;
  vector <int> mother_2;
  vector <int> colour_1;
  vector <int> colour_2;
  vector <LorentzVector> four_momentum;  //encodes 4-momentum and position
  vector <LorentzVector> four_position;

  int nParticles;      //variables in row1, first row beneath "<event>"
  int process_number;
  float weight;
  float energy_scale;
  float QED_coupling;
  float QCD_coupling;

  vector <float> reweight; //variable not in main block of code
  
  //Match up variable with branch
  
  //variables to be filled in second_line_below loop
  tree->Branch("pdgID", &pdgID); 
  tree->Branch("status", &status);
  tree->Branch("mother_1", &mother_1);
  tree->Branch("mother_2", &mother_2);
  tree->Branch("colour_1", &colour_1);
  tree->Branch("colour_2", &colour_2);
  tree->Branch("four_momentum", &four_momentum);
  tree->Branch("four_position", &four_position);
  
  //filled separate from first_below_event and second_line_below loops
  tree->Branch("reweight",&reweight); 

  //filled in first_below_event loop
  tree->Branch("nParticles",&nParticles);
  tree->Branch("process_number",&process_number);
  tree->Branch("weight",&weight);
  tree->Branch("energy_scale",&energy_scale);
  tree->Branch("QED_coupling",&QED_coupling); 
  tree->Branch("QCD_coupling",&QCD_coupling); 
 
  string line; //line of LHE file, to be looped over in main part of looper()

  bool first_line_below = false; //whether on line right below event
  bool second_line_below = false; //between <event> and </event>, is turned to true one line after "first_line_below", both turn off after filling tree
  bool is_reweight = false;  //notes whether data contains reweight data

  //declare vectors used to fill other variables
  vector<float> row;  //row used to fill variables in second_line_below loop
  vector<float> row1; //row used to fill variables in first_line_below part of looper() 

  //Opening data file
  fstream myfile (Form("%s.lhe",filename), ios_base::in);

  while (getline(myfile,line)){  //loops through file and fills line(the variable) with that line of the file


 
    if (numEvents > maxEvents) break;  //fills tree only for maxEvents number of events

    //if on 2nd line below "<event>", fill variables(row1 and reweight entries filled in other loops)
    if (second_line_below == true) {   
      do {  
        if (line.find('#') != string::npos) break;  //if line contains '#', line does not contain data anymore, break out of loop
      
        //iss contains line
        istringstream iss;   //opening string stream
        iss.str(line);      //copying line into stream

        float val; 
         
     	while (iss >> val) row.push_back(val);  //fill row with data from line

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

        row.clear();  //clears row so we can fill it again with next line of data

     } while (getline(myfile,line));

      first_line_below = false;
      second_line_below = false;  // turn first_line_below, second_line_below off so we know we're done with those loops
    }
 
    //Line right below <event>(row1 variables)
    if (first_line_below == true) {
      second_line_below = true; //sets up loop over second_line_below variables

      istringstream iss;
      iss.str(line);

      float val1;

      while (iss >> val1) row1.push_back(val1);  //fill row1 with numbers in line

      nParticles = row1[0];
      process_number = row1[1];
      weight = row1[2];
      energy_scale = row1[3];
      QED_coupling = row1[4];
      QCD_coupling = row1[5];

      row1.clear(); //clears row1 for next event 
      
    }
    
    //Store reweights 
    if (line.find("wgt id") != string::npos) { //if line contains wgt id, start filling reweight

      //wgt is substring of line that contains the reweight information
      float wgt = store_reweights(line);
      reweight.push_back(wgt); //fill reweight vector

      is_reweight = true; //note that events contain reweight information
    }

    //Check every line after </event> to see if we've reached the next event 
    if ((line.find("<event>") != string::npos)) {  //if line contains  "<event>", we turn first_line_below to true(begins filling row1 variables)

      //if file doesn't contain reweight info, fill reweight with nonsense 
      if (is_reweight == false) reweight.push_back(-5);

      if (pdgID.size() != 0){
        tree->Fill();   //only fill tree if vectors are nonempty(pdgID picked arbitrarily)
        numEvents++;

      }
  
      //clear vectors so we can fill them again for next event
      pdgID.clear();  
      status.clear();
      mother_1.clear();
      mother_2.clear();
      colour_1.clear();
      colour_2.clear();
      reweight.clear();
      four_momentum.clear();
      four_position.clear();

      first_line_below = true; //begins process to move onto next event
    }
    
  }

  tree->Fill(); //last event isn't filled, because there's no next "<event>" to trigger it

  file->cd();
  tree->Write(); 

  return 0;
}
