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
	Genome findGenome(string nameToFind) const;
};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
{
	m_minSearchLength = minSearchLength;
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
	return m_minSearchLength;  
}

Genome GenomeMatcherImpl::findGenome(string nameToFind) const
{
	for (int i = 0; i < myGenomes.size(); i++) {
		if (myGenomes[i].name() == nameToFind) {
			return myGenomes[i];
		}
	}
	Genome a("Nonsense", "");
	return a;
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	if (fragment.size() < minimumLength) {
		return false;
	}
	if (minimumLength < m_minSearchLength) {
		return false;
	}
	string segmentToFind = fragment.substr(0, m_minSearchLength);
	vector<DNAMatch> foundSegments = myTrie.find(segmentToFind, exactMatchOnly);
	vector<DNAMatch> longEnoughSegments;

	//for each possible match, calculate the length of the match to the fragment.
	//if it's exactMatchOnly, numOff can only be 0. otherwise, it can be one
	for (int i = 0; i < foundSegments.size(); i++) {
		Genome matchingGenome = findGenome(foundSegments[i].genomeName);
		//now that we've found a matching genome, find the segment in the genome
		string toMatch = "";
		int numMatching = 0;
		int numOff = 0;
		//this gets the string
		matchingGenome.extract(foundSegments[i].position, fragment.length(), toMatch);
		//this gets the length of the segments which match (w one off if false) 
		for (int j = 0; j < toMatch.length(); j++) {
			//if it's exactMatchOnly, no mismatches! 
			//otherwise, keep going until there's another mismatch 
			if (fragment[j] == toMatch[j]) {
				numMatching++;
			}
			else {
				if (!exactMatchOnly) {
					numOff++;
					if (numOff > 1) {
						break;
					}
					else {
						numMatching++;
					}
				}
				else {
					//if it's an exactMatchOnly, one mistake --> you're out!
					break;
				}
			}
		}
		//if it's long enough, add it to the longEnoughSegments :) 
		if (numMatching >= minimumLength) {
			foundSegments[i].length = numMatching;
			longEnoughSegments.push_back(foundSegments[i]);
		}
	}

	//The above process doesn't care about duplicates :( so, this eliminates them!
	//this loops thru the whole longEnoughSegments and erases ONLY if names are shared
	for (int i = 0; i < longEnoughSegments.size(); i++) {
		for (int k = i + 1; k < longEnoughSegments.size(); k++) {
			if (longEnoughSegments[i].genomeName == longEnoughSegments[k].genomeName) {
				//eliminates the one with the shorter length
				if (longEnoughSegments[i].length > longEnoughSegments[k].length) {
					longEnoughSegments.erase(longEnoughSegments.begin()+k);
				}
				else if (longEnoughSegments[i].length < longEnoughSegments[k].length) {
					longEnoughSegments.erase(longEnoughSegments.begin() + i);
				}
				else {
					//if they're the same length, erase the one farther in 
					if (longEnoughSegments[i].position > longEnoughSegments[k].position) {
						longEnoughSegments.erase(longEnoughSegments.begin() + i);
					}
					else {
						longEnoughSegments.erase(longEnoughSegments.begin() + k);
					}
				}
				//revalidate the vector 
				i = 0;
				k = 1;
			}
		}
	}
	//sometimes, the above process doesn't check if there are 2 genomes left and they're of the same name
	//this is just a quick check and erases the one with the shorter length 
	if (longEnoughSegments.size() == 2) {
		if (longEnoughSegments[0].genomeName == longEnoughSegments[1].genomeName) {
			if (longEnoughSegments[0].length > longEnoughSegments[1].length) {
				longEnoughSegments.erase(longEnoughSegments.begin() + 1);
			}
			else {
				longEnoughSegments.erase(longEnoughSegments.begin() + 0);
			}

		}
	}
	//sets matches to the filtered results 
	matches = longEnoughSegments;
	//if we've found at least one match, return true :) 
	if (matches.empty()) {
		//if there were no matches, return false :( 
		return false;
	}
	//there were matches!
	return true; 
}
bool isGreaterThan(const GenomeMatch g1, const GenomeMatch g2)
{
	if (g1.percentMatch > g2.percentMatch) {
		return true;
	}
	else if(g2.percentMatch > g1.percentMatch){
		return false;
	}
	else {
		if (g1.genomeName < g2.genomeName) {
			return true;
		}
		else {
			return false;
		}
	}
}
bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	if (fragmentMatchLength < m_minSearchLength) {
		return false;
	}
	vector<GenomeMatch> toReturn(myGenomes.size());
	//make one GenomeMatch for each Genome 
	for (int i = 0; i < myGenomes.size(); i++) {
		toReturn[i].genomeName = myGenomes[i].name();
	}
	//get DNAMatches from findGenomesWithDNA
	vector<DNAMatch> fromHelper;
	int numIterations = query.length() / fragmentMatchLength;
	for (int i = 0; i < numIterations; i++) {
		string help;
		query.extract(i*fragmentMatchLength, fragmentMatchLength, help);
		//clear any content that may be left in fromHelper in a previous loop 
		fromHelper.clear();
		if (findGenomesWithThisDNA(help, fragmentMatchLength, exactMatchOnly, fromHelper)) {
			//increment each toReturn genomeMatch if it's in fromHelper
			for (int j = 0; j < fromHelper.size(); j++) {
				//search for the right genomeMatch
				for (int k = 0; k < toReturn.size(); k++) {
					if (toReturn[k].genomeName == fromHelper[j].genomeName) {
						toReturn[k].percentMatch++;
					}
				}
			}
		}
	}
	//now, filter these results by percentage. 
	vector<GenomeMatch> atLeastOnce;
	for (int i = 0; i < toReturn.size(); i++) {
		if (toReturn[i].percentMatch > 0) {
			//makes a new GenomeMatch, computes the percentage, and adds it only if it goes over the threshold
			GenomeMatch* yuh = new GenomeMatch;
			yuh->genomeName = toReturn[i].genomeName;
			yuh->percentMatch = 100*(toReturn[i].percentMatch / numIterations);
			if (yuh->percentMatch >= matchPercentThreshold) {
				atLeastOnce.push_back(*yuh);
			}
			else {
				delete yuh;
			}
		}
	}
	//sorts into descending order of percentage match 
	sort(atLeastOnce.begin(), atLeastOnce.end(), isGreaterThan);
	//if it's matched at least once, return true
	if (atLeastOnce.size() > 0) {
		results = atLeastOnce;
		return true;
	}
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
