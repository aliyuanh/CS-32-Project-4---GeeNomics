#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <istream>
using namespace std;

class GenomeImpl
{
public:
	GenomeImpl(const string& nm, const string& sequence);
	static bool load(istream& genomeSource, vector<Genome>& genomes);
	int length() const;
	string name() const;
	bool extract(int position, int length, string& fragment) const;
private:
	int m_size;
	string m_name;
	string m_sequence;
};

GenomeImpl::GenomeImpl(const string& nm, const string& sequence)
{
	m_name = nm;
	m_sequence = sequence;
	m_size = m_sequence.size();
}

bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes)
{
	//return true;
	string s;
	string name;
	string seq;
	int num = 1;
	getline(genomeSource, s);
	//if it does not start with >, not properly formatted!
	if (s[0] != '>') {
		return false;
	}
	name = s.substr(1);
	//if there is nothing after the >, return false
	if (name == "") {
		return false;
	}
	while (getline(genomeSource, s)) {
		if (s[0] == '>') {
			num++;
			if (seq == "") {
				return false;
			}
			Genome myGenome(name, seq);
			genomes.push_back(myGenome);
			name = s.substr(1);
			//if there is nothing after the >, return false
			if (name == "") {
				return false;
			}
			seq = "";
		}
		else {
			seq += s;
			//checks for improper characters within the document
			for (int i = 0; i < s.length(); i++) {
				switch (tolower(s[i])) {
				case 'a':
				case 't':
				case 'c':
				case 'g':
				case 'n':
					break;
				default:
					return false;
				}
			}
		}
	}
	Genome myGenome(name, seq);
	genomes.push_back(myGenome);
	//std::cout << genomes.size() << std::endl;

	return true;  // This compiles, but may not be correct
}

int GenomeImpl::length() const
{
	return m_size;  // This compiles, but may not be correct
}

string GenomeImpl::name() const
{
	return m_name;  // This compiles, but may not be correct
}

bool GenomeImpl::extract(int position, int length, string& fragment) const
{
	if (position + length > GenomeImpl::length()) {
		return false;
	}
	fragment = m_sequence.substr(position, length);

	return true;  // This compiles, but may not be correct
}

//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const string& nm, const string& sequence)
{
	m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
	delete m_impl;
}

Genome::Genome(const Genome& other)
{
	m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
	GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
	delete m_impl;
	m_impl = newImpl;
	return *this;
}

bool Genome::load(istream& genomeSource, vector<Genome>& genomes)
{
	return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
	return m_impl->length();
}

string Genome::name() const
{
	return m_impl->name();
}

bool Genome::extract(int position, int length, string& fragment) const
{
	return m_impl->extract(position, length, fragment);
}
