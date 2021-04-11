//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     Samuel Valaštín <xvalas10@stud.fit.vutbr.cz>
// $Date:       $2017-01-04
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author Samuel Valaštín
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//============================================================================//
// definicia class podla : https://github.com/google/googletest/blob/master/docs/primer.md
class EmptyTree : public ::testing::Test {
    protected:
        BinaryTree emptytree;
};

class NonEmptyTree : public ::testing::Test {
    protected:
        virtual void SetUp(){
            // naplnenie stromu hodnotami
            std::vector<uint16_t> nodeValues = {5,11,25,96,2,3,7};
            for(size_t i = 0 ; i < nodeValues.size();i++){
                nonemptytree.InsertNode(nodeValues[i]);
            }
        }
        BinaryTree nonemptytree;
};

class TreeAxioms : public ::testing::Test {
    protected:
        virtual void SetUp(){
            // naplnenie stromu hodnotami
            std::vector<uint16_t> nodeValues = {7,4,1,19,25,25,29,88,33,30,44,95,97,99};
            for(size_t i = 0 ; i < nodeValues.size();i++){
                binarytreeAx.InsertNode(nodeValues[i]);
            }
        }
        BinaryTree binarytreeAx;
};

TEST_F(EmptyTree,InsertNode){
    // pred vlozenim je koren inicializovany na nullptr
    ASSERT_EQ(emptytree.GetRoot(),nullptr);
    emptytree.InsertNode(3);
    // test nad jednym uzlom
    ASSERT_NE(emptytree.GetRoot(),nullptr);
    emptytree.DeleteNode(3); // uvolnime vytvoreny uzol
    emptytree.InsertNode(7);
    std::pair<bool, BinaryTree::Node_t*> checkRes = emptytree.InsertNode(9);
    // test ci sa podarilo vlozit uzol z hodnotou 9
    ASSERT_TRUE(checkRes.first);
    emptytree.InsertNode(17);
    ASSERT_EQ(emptytree.GetRoot()->key,9);
    // uvolnenie uzlov
    emptytree.DeleteNode(17);
    emptytree.DeleteNode(9);
    emptytree.DeleteNode(7);
}

TEST_F(EmptyTree,DeleteNode){
    ASSERT_NE(emptytree.DeleteNode(4),true);
    emptytree.InsertNode(4);
    ASSERT_EQ(emptytree.DeleteNode(4),true);
    ASSERT_EQ(emptytree.GetRoot(),nullptr);
}

TEST_F(EmptyTree,FindNode){
    ASSERT_EQ(emptytree.FindNode(2),nullptr);
    emptytree.InsertNode(2);
    ASSERT_NE(emptytree.FindNode(2),nullptr);
    emptytree.DeleteNode(2);
    ASSERT_FALSE(emptytree.GetRoot());
}

TEST_F(NonEmptyTree,InsertNode){

    // na zaciatku je Rootom prvok s klucom 11
    ASSERT_EQ(nonemptytree.GetRoot()->key,11);
    nonemptytree.InsertNode(1);
    ASSERT_NE(nonemptytree.FindNode(1),nullptr);
    std::pair<bool, BinaryTree::Node_t*> checkRes = nonemptytree.InsertNode(4);
    ASSERT_TRUE(checkRes.first);
    ASSERT_EQ(checkRes.second,nonemptytree.FindNode(4));
    nonemptytree.InsertNode(6);
    ASSERT_EQ(nonemptytree.GetRoot()->key,5);
    checkRes = nonemptytree.InsertNode(6);
    ASSERT_FALSE(checkRes.first);
}

TEST_F(NonEmptyTree,DeleteNode){

    ASSERT_EQ(nonemptytree.GetRoot()->key,11);
    bool del = nonemptytree.DeleteNode(11);
    ASSERT_TRUE(del);
    ASSERT_EQ(nonemptytree.FindNode(11),nullptr);
    EXPECT_EQ(nonemptytree.GetRoot()->key,7);
    del = nonemptytree.DeleteNode(7);
    ASSERT_TRUE(del);
    EXPECT_EQ(nonemptytree.GetRoot()->key,5);
    del = nonemptytree.DeleteNode(7);
    ASSERT_FALSE(del);
    nonemptytree.DeleteNode(5);
    EXPECT_EQ(nonemptytree.GetRoot()->key,3);
    nonemptytree.DeleteNode(3);
    del = nonemptytree.DeleteNode(25);
    ASSERT_TRUE(del);
    del = nonemptytree.DeleteNode(96);
    ASSERT_TRUE(del);
    del = nonemptytree.DeleteNode(25);
    ASSERT_FALSE(del);
    EXPECT_EQ(nonemptytree.GetRoot()->key,2);
    del = nonemptytree.DeleteNode(2);
    ASSERT_EQ(nonemptytree.GetRoot(),nullptr);
}

TEST_F(NonEmptyTree,FindNode){

    std::vector<uint16_t> nodeValues = {5,11,25,96,2,3,7};
    for(size_t i = 0 ; i < nodeValues.size();i++){
        EXPECT_EQ(nonemptytree.FindNode(nodeValues[i])->key,nodeValues[i]);
    }

    nonemptytree.DeleteNode(11);
    ASSERT_EQ(nonemptytree.FindNode(11),nullptr);
    std::pair<bool, BinaryTree::Node_t*> checkRes = nonemptytree.InsertNode(11);
    ASSERT_EQ(checkRes.second,nonemptytree.FindNode(11));

    for(size_t i = 0 ; i < nodeValues.size();i++){
        nonemptytree.DeleteNode(nodeValues[i]);
        ASSERT_EQ(nonemptytree.FindNode(nodeValues[i]),nullptr);
    }

    checkRes = nonemptytree.InsertNode(66);
    ASSERT_EQ(checkRes.second,nonemptytree.FindNode(66));
    nonemptytree.DeleteNode(66);
}

TEST_F(TreeAxioms,Axiom1){
    std::vector<Node_t *> leafs;
    binarytreeAx.GetLeafNodes(leafs);
    
    if(!leafs.empty()){
        for(size_t i = 0 ; i < leafs.size();i++){
            ASSERT_EQ(leafs[i]->color,BLACK);
        }
    }
}

TEST_F(TreeAxioms,Axiom2){
    std::vector<Node_t *>allNodes;
    binarytreeAx.GetAllNodes(allNodes);

    for(size_t i = 0; i < allNodes.size();i++){

        if(allNodes[i]->color == RED){

            if((allNodes[i]->pLeft) && (allNodes[i]->pRight)){
                ASSERT_EQ(allNodes[i]->pLeft->color,BLACK);
                ASSERT_EQ(allNodes[i]->pRight->color,BLACK);
                // obaja synovia sú čierny
                ASSERT_EQ(allNodes[i]->pLeft->color,allNodes[i]->pRight->color);
            } else if(allNodes[i]->pLeft){
                ASSERT_EQ(allNodes[i]->pLeft->color,BLACK);

            } else if(allNodes[i]->pRight){
                ASSERT_EQ(allNodes[i]->pRight->color,BLACK);
            }
        }
    }
}

TEST_F(TreeAxioms,Axiom3){
    std::vector<Node_t *> leafs;
    std::vector<uint16_t> countBlackNodes;
    binarytreeAx.GetLeafNodes(leafs);
    Node_t *root = binarytreeAx.GetRoot();
    Node_t *actNode;
    uint16_t counter = 0;

    for(size_t i = 0; i < leafs.size();i++){
        actNode = leafs[i];
        
        while(actNode != root){
            actNode = actNode->pParent;
            if(actNode->color == BLACK){
                counter++;
            }
        }
        // ulozenie poctu ciernych uzlov do vektoru
        countBlackNodes.push_back(counter);
        counter = 0;
    }
    if(!countBlackNodes.empty()){
        uint16_t pattern = countBlackNodes[0];
        for(size_t i = 1 ; i < countBlackNodes.size();i++){
            ASSERT_EQ(pattern,countBlackNodes[i]);
        }
    }
}
/*** Konec souboru black_box_tests.cpp ***/