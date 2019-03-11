#include "Trie.h"
#include <iostream>
#include <string>
#include <fstream>
#include "provided.h"
#include <vector>
using namespace std;
int main() {
	/*
	Trie<int> trie;
	trie.insert("atcgg", 5);
	trie.insert("atgg", 2);
	trie.insert("atgg", 7);
	trie.insert("atg", 11);
	trie.insert("gtg", 9);
	//LOL remember to make it false if it can be off by 1 
	std::vector<int> yuh = trie.find("gta", false);
	std::cout << "the first thing" << std::endl;
	for (int i = 0; i < yuh.size(); i++) {
		//std::cout << "printin out yuh" << std::endl;
		//std::cout << yuh[i] << std::endl;
	}

	trie.reset();
	trie.insert("atg",2);
	
	string filename = "C:\\Users\\aliyu\\source\\repos\\geenomics\\geenomics\\data\\Halorubrum_chaoviator.txt";
	ifstream strm(filename);
	if (!strm) {
		cout << "yo it not be existing" << endl;
		return 0;
	}
	vector<Genome> vg;
	bool success = Genome::load(strm, vg);
	if (success) {
		cout << "Loaded	" << vg.size() << "	genomes	successfully:" << endl;			
		for (int k = 0; k != vg.size(); k++) {
			//cout << vg[k].name() << endl;
			//cout << vg[k].length() << endl;
		}
	}
	else {
		cout << "Error loading genome data";
	}
	GenomeMatcher gee(4);
	gee.addGenome(vg[0]);
	*/
	/*
	
	Trie<int> bigBerg;
	bigBerg.insert("ABCD", 1);
	bigBerg.insert("ABCD", 2);
	bigBerg.insert("ABCD", 3);
	bigBerg.insert("AXCD", 4);
	bigBerg.insert("AXCD", 5);
	bigBerg.insert("AXYD", 6);
	bigBerg.insert("AXYD", 7);
	std::vector<int> bergs = bigBerg.find("ABCD", true);
	for (int i = 0; i < bergs.size(); i++) {
		std::cout << bergs[i] << " " << std::endl;
	}
	std::cout<<std::endl;
	std::vector<int> careys = bigBerg.find("ABCE", true);
	if (careys.empty()) {
		std::cout << "careys are empty!  no matches" << std::endl;
	}
	for (int i = 0; i < careys.size(); i++) {
		std::cout << careys[i] << std::endl;
	}
	*/


	/*
	Genome	g("oryx", "GCTCGGNACACATCCGCCGCGGACGGGACGGGATTCGGGCTGTCGATTGTCTCACAGATCGTCGACGTACATGACTGGGA");
	string	f1, f2, f3;		
	bool	result1 = g.extract(0, 5, f1);		//	result1	=	true,	f1	=	“GCTCG”;		
	if (result1) {
		cout << "good" << endl;
		cout << f1 << endl;

	}
	else {
		cout << "bad" << endl;
	}
	bool	result2	=	g.extract(74,	6,	f2);	//	result2	=	true,	f1	=	“CTGGGA”;	
	if (result2) {
		cout << "good" << endl;
		cout << f2 << endl;

	}
	else {
		cout << "bad" << endl;
	}
	bool	result3	=	g.extract(74,	7,	f3);	//	result3	=	false,	f1	is	unchanged	
	if (!result3) {
		cout << "good" << endl;
		cout << f3 << endl;
	}
	else {
		cout << "bad" << endl;
	}
	*/
	Genome g1("genome 1","ACTG");
	Genome g2("genome 2", "TCGACT");
	Genome g3("genome 3", "TCTCG");
	GenomeMatcher geneMatcher(3);
	geneMatcher.addGenome(g1);
	geneMatcher.addGenome(g2);
	geneMatcher.addGenome(g3);
	vector<DNAMatch> genomeThatMatched;
	geneMatcher.findGenomesWithThisDNA("CTAC",3,false,genomeThatMatched);
	if (genomeThatMatched.empty()) {
		cout << "no matchero" << endl;
	}
	else {
		cout << "The first match is from " << genomeThatMatched[0].genomeName << " at position " << genomeThatMatched[0].position << std::endl;
	}
	return 0;
}