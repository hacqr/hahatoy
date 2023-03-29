#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
using std::map;
using std::vector;
using std::string;

#define SAFE_DEL(p) \
    do{if(p!=nullptr){delete p;p=nullptr;}}while(0)

struct SNode {
    int left;
    int right;
    int depth;
    int id;
    ~SNode(){
        std::cout << "node is removed from memory" << std::endl;
    }
};

class CFolder {
public:
    CFolder() {
        _root = new SNode();
        _root->left = 1;
        _root->right = 2;
        _root->id = 0;
        _root->depth = 0;
        associateNode(_root);
    }

    ~CFolder() {
        for (auto in : _mapId2Nodes) {
            SAFE_DEL(in.second);
        }
        _mapId2Nodes.clear();
        _mapLeft2Nodes.clear();
        _mapRight2Nodes.clear();
    }

    bool addNode(int id, int newId) {
        SNode* pNode = findNode(id);
        if (pNode == nullptr) {
            return false;
        }
        SNode* pNewNode = findNode(newId);
        if (pNewNode != nullptr) {
            return false;
        }
        pNewNode = new SNode();
        pNewNode->left = pNode->right;
        pNewNode->right = pNewNode->left + 1;
        pNewNode->id = newId;
        pNewNode->depth = pNode->depth + 1;
        onInsert(pNode->right);
        associateNode(pNewNode);
        return true;
    }

    vector<SNode*> listNode(int id, bool bAll = false) {
        vector<SNode*> vecNodes;
        SNode* pNode = findNode(id);
        if (pNode == nullptr) {
            return vecNodes;
        }
        getSubNodes(pNode, vecNodes, bAll);
        return vecNodes;
    }

    bool removeNode(int id) {
        SNode* pNode = findNode(id);
        if (pNode == nullptr) {
            return false;
        }
        if (pNode == _root) {
            return false;
        }
        removeNode(pNode);
        onDelete(pNode->left, pNode->right);
        SAFE_DEL(pNode);
        return true;
    }

    bool moveNode(int srcId, int dstId) {
        vector<SNode*> vecDeletedNodes;
        SNode* pSrcNode = findNode(srcId);
        if (pSrcNode == nullptr) {
            return false;
        }
        SNode* pDstNode = findNode(dstId);
        if (pDstNode == nullptr) {
            return false;
        }
        removeNode(pSrcNode);
        onDelete(pSrcNode->left, pSrcNode->right, &vecDeletedNodes);

        int increaseVal = pSrcNode->right - pSrcNode->left + 1;
        const int diff = pDstNode->right - pSrcNode->left;
        pSrcNode->left = pDstNode->right;
        pSrcNode->right = pSrcNode->left + increaseVal - 1;
        onInsert(pDstNode->right, increaseVal);
        associateNode(pSrcNode);
        for (auto pNode: vecDeletedNodes) {
            pNode->left += diff;
            pNode->right += diff;
            associateNode(pNode);
        }
        return true;
    }

    void dump() {
        std::cout << "-------------------------------" << std::endl;
        for (auto in : _mapId2Nodes) {
            std::cout << "id:" << in.second->id << ", left:" 
                << in.second->left << ", right:" << in.second->right << std::endl;
        }
        std::cout << "left indexed size is " << _mapLeft2Nodes.size() << std::endl;
        std::cout << "right indexed size is " << _mapRight2Nodes.size() << std::endl;
        std::cout << "-------------------------------" << std::endl;
    }

    void dump(const vector<SNode*>& nodes) {
        std::cout << "-------------------------------" << std::endl;
        for (auto n : nodes) {
            std::cout << "id:" << n->id << ", left:" 
                << n->left << ", right:" << n->right << std::endl;
        }
        std::cout << "-------------------------------" << std::endl;
    }

private:
    SNode* findNode(int id) {
        auto itf = _mapId2Nodes.find(id);
        return itf!=_mapId2Nodes.end()?itf->second:nullptr;
    }

    bool checkNodeNotExist(SNode* pNode) {
        return _mapId2Nodes.find(pNode->id) == _mapId2Nodes.end();
    }

    void associateNode(SNode* pNode) {
         _mapId2Nodes[pNode->id] = pNode;
         _mapLeft2Nodes[pNode->left] = pNode;
         _mapRight2Nodes[pNode->right] = pNode;
    }

    void removeNode(SNode* pNode) {
         _mapId2Nodes.erase(pNode->id);
         _mapLeft2Nodes.erase(pNode->left);
         _mapRight2Nodes.erase(pNode->right);
    }

    void onDelete(int left, int right, vector<SNode*>* pVecDeletedNodes = nullptr) {
        vector<SNode*> tmpNodes;
        const int decreaseVal  = right-left+1;

        auto itLeftS = _mapLeft2Nodes.upper_bound(left);
        auto itLeftE = _mapLeft2Nodes.lower_bound(right);

        for (; itLeftS!=itLeftE; ) {
            if (itLeftS->second->right < right) {
                _mapRight2Nodes.erase(itLeftS->second->right);
                _mapId2Nodes.erase(itLeftS->second->id);
                if (pVecDeletedNodes != nullptr) {
                    pVecDeletedNodes->push_back(itLeftS->second);
                } else {
                    SAFE_DEL(itLeftS->second);
                }
                itLeftS = _mapLeft2Nodes.erase(itLeftS);
            } else {
                ++itLeftS;
            }
        }

        auto itf = _mapLeft2Nodes.upper_bound(right);
        for (; itf!=_mapLeft2Nodes.end();) {
            itf->second->left -= decreaseVal;
            tmpNodes.push_back(itf->second);
            itf = _mapLeft2Nodes.erase(itf);
        }
        for (auto n : tmpNodes) {
            _mapLeft2Nodes[n->left] = n;
        }

        tmpNodes.clear();
        itf = _mapRight2Nodes.upper_bound(right);
        for (; itf!=_mapRight2Nodes.end();) {
            itf->second->right -= decreaseVal;
            tmpNodes.push_back(itf->second);
            itf = _mapRight2Nodes.erase(itf);
        }
        for (auto n : tmpNodes) {
            _mapRight2Nodes[n->right] = n;
        }
    }

    void onInsert(int right, const int range = 2) {
        vector<SNode*> tmpNodes;
        auto itf = _mapRight2Nodes.find(right);
        for (; itf!=_mapRight2Nodes.end();) {
            itf->second->right += range;
            tmpNodes.push_back(itf->second);
            itf = _mapRight2Nodes.erase(itf);
        }
        for (auto n : tmpNodes) {
            _mapRight2Nodes[n->right] = n;
        }

        tmpNodes.clear();
        itf = _mapLeft2Nodes.lower_bound(right);
        for (; itf!=_mapLeft2Nodes.end();) {
            itf->second->left += range;
            tmpNodes.push_back(itf->second);
            itf = _mapLeft2Nodes.erase(itf);
        }
        for (auto n : tmpNodes) {
            _mapLeft2Nodes[n->left] = n;
        }
    }

    void getSubNodes(SNode* pNode, vector<SNode*>& vecNodes, bool bAll = false) {
        auto itLeftS = _mapLeft2Nodes.upper_bound(pNode->left);
        auto itLeftE = _mapLeft2Nodes.lower_bound(pNode->right);
        for (; itLeftS!=itLeftE; ++itLeftS) {
            if (itLeftS->second->right < pNode->right) {
                if (bAll || itLeftS->second->depth == pNode->depth+1) {
                    vecNodes.push_back(itLeftS->second);
                }
            }
        }
    }

private:
    SNode* _root;
    map<int, SNode*> _mapId2Nodes;
    map<int, SNode*> _mapLeft2Nodes;
    map<int, SNode*> _mapRight2Nodes;
};

vector<string> split(const string& src, const string& delim)
{
    size_t pos = 0;
    size_t offset = 0;
    vector<string> result;

    while ((pos = src.find(delim, offset)) != string::npos)
    {
        result.push_back(src.substr(offset, pos - offset));
        offset = pos + delim.size();
    }

    if (pos == string::npos)
    {
        result.push_back(src.substr(offset, pos));
    }

    return result;
}

vector<string> parseCommand(const string& strCmd) {
    vector<string> vecLastToken;
    vector<string> vecToken = split(strCmd, " ");
    for (auto tk : vecToken) {
        if (!tk.empty()) {
            vecLastToken.push_back(std::move(tk));
        }
    }
    return vecLastToken;
}

int main() {
    CFolder folder;
    string strInput;

    const char* usage = "************* command usage *************\n"
                         "root's id is 0\n"
                         "a arg1 arg2  --    add folder \n"
                         "d arg1       --    remove folder\n"
                         "m arg1 arg2  --    move arg1 to arg2\n"
                         "p            --    print folder structure\n"
                         "q            --    quit program\n"
                         "l arg1       --    list sub folder\n"
                         "la arg1      --    list all sub folder\n"
                         "*****************************************";

#define PRINT_USAGE  std::cout << usage << std::endl;

    while (std::getline(std::cin, strInput)) {
        if (strInput == "quit") {
            exit(0);
        } else if (strInput == "p") {
            folder.dump();
        } else {
            //vector<string> vecParams = split(strInput, " ");
            vector<string> vecParams = parseCommand(strInput);
            if (vecParams.size() < 2) {
                PRINT_USAGE;
                continue;
            }
            string strOp = vecParams[0];
            if (strOp == "a") {
                if (vecParams.size() < 3) {
                    std::cout << "params is invalid!" << std::endl;
                    continue;
                }
                int id = atoi(vecParams[1].c_str());
                int newId = atoi(vecParams[2].c_str());
                if (!folder.addNode(id, newId)) {
                    std::cout << "add failed!" << std::endl;
                }
            } else if(strOp == "m") {
                if (vecParams.size() < 3) {
                    std::cout << "params is invalid!" << std::endl;
                    continue;
                }
                int srcId = atoi(vecParams[1].c_str());
                int dstId = atoi(vecParams[2].c_str());
                if (!folder.moveNode(srcId, dstId)) {
                    std::cout << "move failed!" << std::endl;
                }
            } else if (strOp == "d") {
                int id = atoi(vecParams[1].c_str());
                if (!folder.removeNode(id)) {
                    std::cout << "remove failed!" << std::endl;
                }
            } else if (strOp == "l") {
                int id = atoi(vecParams[1].c_str());
                vector<SNode*> r = folder.listNode(id);
                folder.dump(r);
            } else if (strOp == "la") {
                int id = atoi(vecParams[1].c_str());
                vector<SNode*> r = folder.listNode(id, true);
                folder.dump(r);
            } else {
                PRINT_USAGE;
            }
        }
    }
    return 0;
}
