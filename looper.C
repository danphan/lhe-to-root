#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "TTree.h"
#include "TFile.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LorentzVector;

using namespace std;

int nEventsMax = 5;
int nEvents = 0;

int looper(){

  LorentzVector alex;
  alex.SetE(4);
  alex.SetPx(5);
  alex.SetPy(6);
  alex.SetPz(7);

  cout << alex.eta() << endl;

  //Declare TTree and TFile
  TFile *file = new TFile("dansFile.root", "RECREATE");
  TTree *tree = new TTree("tree", "Dan's Tree");
  
  //Declare variables that will be stored in tree
  vector <int> pdgID;
  vector <int> a;
  vector <int> b;
  vector <int> c;
  vector <int> d;
  vector <int> e;
  vector <float> f;
  vector <float> g;
  vector <float> h;
  vector <float> i;
  vector <float> j;
  vector <float> k;
  vector <float> l;
  vector <float> row1; //row underneath <event>
  vector <float> wgtID;
  
  //Match up variable with branch
  tree->Branch("pdgID", &pdgID); 
  tree->Branch("a", &a);
  tree->Branch("b", &b);
  tree->Branch("c", &c);
  tree->Branch("d", &d);
  tree->Branch("e", &e);
  tree->Branch("f", &f);
  tree->Branch("g", &g);
  tree->Branch("h", &h);
  tree->Branch("i", &i);
  tree->Branch("j", &j);
  tree->Branch("k", &k);
  tree->Branch("l", &l);
  tree->Branch("row1",&row1); //row beneath <event>
  tree->Branch("wgtID",&wgtID); 
  
  string line;
  string subline1 = "<event>";
  string subline2 = "#";  //end of data to be stored, "iss >> val" messes up when lines have text
  string subline3 = "wgt id";
  bool first = false;
  bool second = false;
  bool is_wgtID = false;
  
  fstream myfile ("lhe_file_toy.lhe", ios_base::in);  //opening data file
  while (getline(myfile,line)) {          //looping through lines
    if (nEvents > nEventsMax){
      tree->Write(); 
      return 0;
    }
    if (second == true) {   //if second is on, we do stuff to line after(since we call getline again)
      do {  //keeps looping until we find "stop"
        if (line.find(subline2) != string::npos) {break;}  //if line contains "stop", stop loop
        
        //Declare temp vector, val
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
 
        //Now move row to "pdgID" vector 
        pdgID.push_back(row[0]);  //do we need to worry if this is zero?
        a.push_back(row[1]);
        b.push_back(row[2]);
        c.push_back(row[3]);
        d.push_back(row[4]);
        e.push_back(row[5]);
        f.push_back(row[6]);
        g.push_back(row[7]);
        h.push_back(row[8]);
        i.push_back(row[9]);
        j.push_back(row[10]);
        k.push_back(row[11]);
        l.push_back(row[12]); 
      } while (getline(myfile,line));
      first = false;
      second = false;  //after array prints, turn next off again
    }
    if (first == true) {
      second = true;
//      vector <float> row1;  //row1 is row right under <event>

      istringstream iss;
      iss.str(line);
      float val1;

      int count1 = 0;
      while (iss >> val1 && (count1 < 6)) {
        row1.push_back(val1);
        count1++;
      }
    }
    
    if (line.find(subline3) != string::npos) { //if line contains wgt id, start filling wgtID
      is_wgtID = true;
      do {
        if (!(line.find(subline3) != string::npos)) { //if line doesn't contain wgt id, stop filling and break out of loop
          break;
        }
        int begin = line.find(">");
        int end = line.find("<",begin);
        const char* str_val2 = line.substr(begin+1,end-(begin+1)).c_str(); //takes number out of line
        float val2 = atof(str_val2); //does stof only work for C++ 11?
        wgtID.push_back(val2);
      } while (getline(myfile,line));
      tree->Fill();  //after done filling, fill tree
      pdgID.clear();
    }
    if (line.find(subline1) != string::npos) {  //if line contains  "yes", we turn second on
      first = true;
      nEvents++;
      cout << nEvents << endl;
    }

//    if (is_wgtID == false) { //if file doesn't include wgt id, fill it with nonsense
//      for (int i_ = 0; i_ < 50; i_++) {
//        wgtID.push_back(-5);
//      }
//    tree->Fill();
//  }

    
  }

  file->cd();
  tree->Write(); 

  return 0;
}
