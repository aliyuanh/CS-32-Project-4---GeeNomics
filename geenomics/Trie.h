#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <list>
#include <vector>
#include <iostream>//TODO: delete after testing 
template<typename ValueType>
class Trie
{
public:
	Trie();
	~Trie();
	void reset();
	void insert(const std::string& key, const ValueType& value);
	std::vector<ValueType> find(const std::string& key, bool exactMatchOnly) const;

	// C++11 syntax for preventing copying and assignment
	Trie(const Trie&) = delete;
	Trie& operator=(const Trie&) = delete;
private:
	struct Node {
		std::list<ValueType> valsToStore;
		std::vector<Node*> kids;
		char label;
	};
	Node *root;
	void freeTree(Node* boi);
	bool nodeExists(Node*boi, char ye) const;
	void printTrie(Node *toBoi) const;
};

template<typename ValueType>
Trie<ValueType>::Trie() {
	root = new Node;
}

template<typename ValueType>
Trie<ValueType>::~Trie() {
	freeTree(root);
}
template<typename ValueType>
void Trie<ValueType>::reset() {
	//free all memory from current trie
	freeTree(root);
	root = new Node;
}
template<typename ValueType>
void Trie<ValueType>::freeTree(Node * boi)
{
	if (boi == nullptr) {
		return;
	}
	for (int i = 0; i < boi->kids.size(); i++) {
		freeTree(boi->kids[i]);
	}
	delete boi;
}

template<typename ValueType>
bool Trie<ValueType>::nodeExists(Node * boi, char ye) const 
{
	if (boi->kids.empty()) {
		return false;
	}
	for (int i = 0; i < boi->kids.size(); i++) {
		if (ye == boi->kids[i]->label) {
			return true;
		}
	}
	return false;
}

template<typename ValueType>
void Trie<ValueType>::printTrie(Node *toBoi) const 
{
	Node* boi = toBoi;
	if (toBoi->kids.empty()) {
		std::cout << std::endl;
	}
	for (int i = 0; i < boi->kids.size(); i++) {
		std::cout << boi->kids[i]->label<< "  ";
		printTrie(boi->kids[i]);
	}
}



template<typename ValueType>
void Trie<ValueType>::insert(const std::string & key, const ValueType & value)
{
	Node* temp = root;
	//if temp doesn't have a child with the current char, make that child
	for (int i = 0; i < key.length(); i++) {
		if (!nodeExists(temp, key[i])) {
			Node* newBoi = new Node;
			newBoi->label = key[i];
			temp->kids.push_back(newBoi);
			temp = newBoi;
		}
		else {
			//otherwise, move along that char 
			Node *it = temp;
			for (int j = 0; j < temp->kids.size(); j++) {
				if (temp->kids[j]->label == key[i]) {
					it = temp->kids[j];
				}
			}
			temp = it;
		}
	}
	//put the value to store at the branch of the trie
	temp->valsToStore.push_back(value);
}

template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::find(const std::string & key, bool exactMatchOnly) const
{
	Node* temp = root;
	std::vector<ValueType> toReturn;
	for (int i = 0; i < key.length(); i++) {
		if (nodeExists(temp, key[i])) {
			for (int j = 0; j < temp->kids.size(); j++) {
				if (temp->kids[j]->label == key[i]) {
					temp = temp->kids[j];
					break;
				}
				else {
					if (!exactMatchOnly) {
						std::vector<ValueType> extra;
						if (i == 0) {
							continue;
						}
						for (int k = 0; k < temp->kids.size(); k++) {
							if (k == j) {
								continue;
							}
							extra = find(key.substr(0, i) + temp->kids[k]->label + key.substr(i + 1), true);
							if (!extra.empty()) {
								for (int r = 0; r < extra.size(); r++) {
									toReturn.push_back(extra[r]);
								}
							}
						}


					}
				}
			}
			
			
		}
		else {
			if (!exactMatchOnly) {
				if (i == 0) {
					return toReturn;
				}
				std::vector<ValueType> fromKids;
				for (int j = 0; j < temp->kids.size(); j++) {
					//if the key is off by one, test all its children paths with an exact match while replacing that character 
					fromKids = find(key.substr(0, i) + temp->kids[j]->label + key.substr(i + 1), true);
					if (!fromKids.empty()) {
						for (int f = 0; f < fromKids.size(); f++) {
							toReturn.push_back(fromKids[f]);
						}
					}
				}
				return toReturn;

			}

		}
	}
	//adds all the perfect matches 
	for (typename std::list<ValueType>::iterator cry = temp->valsToStore.begin(); cry != temp->valsToStore.end(); cry++) {
		toReturn.push_back(*cry);
	}
	
	return toReturn;
}
#endif // TRIE_INCLUDED
