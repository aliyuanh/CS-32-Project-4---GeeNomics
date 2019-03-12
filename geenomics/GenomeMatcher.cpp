#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "Trie.h"
#include <map>
#include <algorithm>
using namespace std;

class GenomeMatcherImpl
{
public:
	GenomeMatcherImpl(int minSearchLength);
	void addGenome(const Genome& genome);
	int minimumSearchLength() const;
	bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
	bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
private:
	int m_minSearchLength;
	vector<Genome> myGenomes;
	Trie<DNAMatch> myTrie;
};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
{
	m_minSearchLength = minSearchLength;
	// This compiles, but may not be correct
}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
	// This compiles, but may not be correct
	myGenomes.push_back(genome);
	for (int i = 0; i < genome.length() - m_minSearchLength+1; i++) {
		string toAddToTrie;
		if (genome.extract(i, m_minSearchLength, toAddToTrie)) {
			DNAMatch d;
			d.genomeName = genome.name();
			d.length = m_minSearchLength;
			d.position = i;
			myTrie.insert(toAddToTrie, d);
		}
	}
}

int GenomeMatcherImpl::minimumSearchLength() const
{
	return m_minSearchLength;  // This compiles, but may not be correct
}


bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	if (fragment.size() < minimumLength) {
		return false;
	}
	if (minimumLength < m_minSearchLength) {
		return false;
	}
	vector<DNAMatch> possibleMatches;
	//find a match from the start of the fragment
	string firstSegment = fragment.substr(0, m_minSearchLength);
	vector<DNAMatch> matchesToFirst = myTrie.find(firstSegment, exactMatchOnly);
	vector<Genome> matchingGenomes;
	vector<int> matchingPositions;
	vector<string> matchingStrings;
	if (!matchesToFirst.empty()) {
		//std::cout << "we found "<< matchesToFirst.size()<<" matches!" << std::endl;
		for (int i = 0; i < matchesToFirst.size(); i++) {
			string nameToMatch = matchesToFirst[i].genomeName;
			int posToMatch = matchesToFirst[i].position;
			//cout << nameToMatch << " at pos " << posToMatch << endl;
			for (int j = 0; j < myGenomes.size(); j++) {
				if (myGenomes[j].name() == nameToMatch) {
					matchingGenomes.push_back(myGenomes[j]);
					matchingPositions.push_back(posToMatch);
					break;
				}
			}
		}
	}
	else {
		std::cout << "no matches found :(" << std::endl;
	}
	cout << "there are " << matchingGenomes.size() << " possible bois" << endl;
	for (int i = 0; i < matchingGenomes.size(); i++) {
		//for each genome, make a string for the length of the fragment 
		string fragmentOfThis = "";
		matchingGenomes[i].extract(matchingPositions[i], fragment.length(), fragmentOfThis);
		//cout << fragmentOfThis << endl;
		matchingStrings.push_back(fragmentOfThis);
	}
	//go thru the rest of the genome to see if it matches 
	vector<DNAMatch> actualMatches;
	for (int i = 0; i < matchingStrings.size(); i++) {
		if (exactMatchOnly) {
			if (matchingStrings[i] == fragment) {
				//cout << "an exact match to the fragment has been found in " << matchingGenomes[i].name() << " at position " << matchingPositions[i] << endl;
				//TODO::check for if a DNAmatch with the same genomeName exists already
				bool alreadyExists = false;
				int existingIndex = 0;
				for (int h = 0; h < actualMatches.size(); h++) {
					if (actualMatches[h].genomeName == matchesToFirst[i].genomeName) {
						alreadyExists = true;
						existingIndex = h;
						//now, find lengths of tmhe genome for each. 
						int existingLength = matchingStrings[h].length();
						int newLength = fragment.length();
						if (existingLength > newLength) {
							//do not add the actual matches
						}
						else if(newLength>existingLength){
							alreadyExists = true;
							actualMatches.erase(actualMatches.begin() + h);
							actualMatches.push_back(matchesToFirst[i]);
							matchingStrings.erase(matchingStrings.begin() + h);
						}
						else {
							alreadyExists = true;
						}
						break;
					}
					else {
						h++;
					}
				}
				if (!alreadyExists) {
					actualMatches.push_back(matchesToFirst[i]);

				}

			}
			else {
				for (int k = 0; k < minimumLength; k++) {
					if (matchingStrings[i][k] != fragment[k]) {
						break;
					}
					if (k == minimumLength - 1) {
						//check for if a DNAmatch with the same genomeName exists already
						//TODO::
						bool alreadyExists = false;
						int indexOfExist = 0;
						for (int p = 0; p < actualMatches.size(); p++) {
							if (actualMatches[p].genomeName == matchesToFirst[i].genomeName) {
								alreadyExists = true;
								indexOfExist = p;
							}
						}
						if (!alreadyExists) {
							actualMatches.push_back(matchesToFirst[i]);
						}
						else {
							//now, compare lengths.
							int existingLength = 0;
							int newLength = 0;

							for (int g = 0; g < matchingStrings[indexOfExist].size(); g++) {
								if (matchingStrings[indexOfExist][g] != fragment[g]) {
									break;
								}
								else {
									existingLength++;
								}
							}
							for (int g = 0; g < matchingStrings[i].size(); g++) {
								if (matchingStrings[i][g] != fragment[g]) {
									break;
								}
								else {
									newLength++;
								}
							}
							if (newLength > existingLength) {
								//replace the existing thing in actualMatches
								actualMatches.erase(actualMatches.begin() + indexOfExist);
								matchingStrings.erase(matchingStrings.begin() + indexOfExist);
								actualMatches.push_back(matchesToFirst[i]);
							}
							else if (newLength < existingLength) {
								//don't add it, do nothing
							}
						}

					}
				}
			}
		}
		else {
			//can only have one mismatch! if one or less, push to the actual matches
			//else, don't push 
			int numOff = 0;
			string toPush = "";
			cout << matchingStrings[i] << endl;
			//if fragmentOfThis is blank, that means there wasn't a long enough segment, so skip
			for (int k = 0; k < fragment.length(); k++) {
				if (matchingStrings[i] == "") {
					numOff = 100;
					continue;
				}
				toPush += matchingStrings[i][k];

				if (fragment[k] != matchingStrings[i][k]) {
					if (numOff == 1) {
						//cout << "too many changeroonies" << matchingStrings[i] << endl;
						//numOff++;
						continue;
					}
					if (exactMatchOnly) {
						break;
					}
					else {
						numOff++;
					}
				}

			}
			if (numOff <= 1 && toPush.length() >= minimumLength) {
				//std::cout << "adding to the actual matches " <<matchingStrings[i]<< endl;
				
				//TODO 				//check for if a DNAmatch with the same genomeName exists already
				bool alreadyExists = false;
				int indexOfExists = 0;
				for (int p = 0; p < actualMatches.size(); p++) {
					if (actualMatches[p].genomeName == matchesToFirst[i].genomeName) {
						alreadyExists = true;
						indexOfExists = p;
					}
					//cout << "NAME IS" << endl;
					cout << actualMatches[p].genomeName << endl;
				}
				if (!alreadyExists) {
					actualMatches.push_back(matchesToFirst[i]);
				}
				else {
					//something already exists, so let's check lengths! 
					int existingLength = 0;
					int newLength = 0;
					for (int h = 0; h < matchingStrings[indexOfExists].length(); h++) {
						if (matchingStrings[indexOfExists][h] != fragment[h]) {
							break;
						}
						else {
							existingLength++;
						}
					}

					for (int h = 0; h < matchingStrings[i].length(); h++) {
						if (matchingStrings[i][h] != fragment[h]) {
							break;
						}
						else {
							newLength++;
						}
					}
					cout << "existing length is " << existingLength << endl;
					cout << "new length is " << newLength << endl;
					if (newLength > existingLength) {
						//erase!
						
						actualMatches.erase(actualMatches.begin() + indexOfExists);
						matchingStrings.erase(matchingStrings.begin() + indexOfExists);
						//actualMatches.push_back(matchesToFirst[i]);
						//i = 0;
					}
					else {
						//do not insert
					}
				}
			}
			else {
				//cout << matchingStrings[i] << " is too diff from \n" << fragment << endl;
			}
		}
	}
	cout << actualMatches.size() << " actual matches" << endl;
	for (int i = 0; i < actualMatches.size(); i++) {
		cout << actualMatches[i].genomeName << " at " << actualMatches[i].position << endl;
	}
	if (actualMatches.size() == 0) {
		cout << "ouch. no matches. what a rough time" << endl;
		return false;
	}
	else {
		matches.push_back(actualMatches[0]);
		return true;
	}
	//now, go thru each string to see if it's an exact match.
	//if it's an exact match, hooray! if it's off by one, use exactMatchOnly 
	//return false;  // This compiles, but may not be correct
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	return false;  // This compiles, but may not be correct
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
	m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
	delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
	m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
	return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
