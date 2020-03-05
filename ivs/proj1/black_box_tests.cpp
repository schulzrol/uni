//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     Roland Schulz <xschul06@stud.fit.vutbr.cz>
// $Date:       $2017-01-04
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author Roland Schulz xschul06
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

//vytvoreni pripadu prazdneho stromu
class EmptyTree : public ::testing::Test
{
    protected:
        BinaryTree rbtree;
};

//vytvoreni pripadu neprazdneho stromu
class NonEmptyTree : public ::testing::Test
{
    protected:
        void SetUp() {
            for (int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
                if (inputs[i] != not_in_input) {rbtree.InsertNode(inputs[i]);}
            }
        }

        BinaryTree rbtree;
        int not_in_input = 42;
        int inputs[10] = {2,1,5,0,-2,-1,-5,4,6,7};
};

// testy nad prazdnum stromem
TEST_F(EmptyTree, InsertNode){
    std::pair<bool, Node_t *> tmppair;
    std::pair<bool, Node_t *> root;
    // nove uzly
    // black root
    int key1 = 3;
    root = rbtree.InsertNode(key1);
    EXPECT_EQ(root.first, true);
    ASSERT_FALSE(root.second == NULL);
    EXPECT_EQ(root.second->color, Color_t::BLACK);
    EXPECT_EQ(root.second->key, key1);

    // red left
    int key2 = 1;
    tmppair = rbtree.InsertNode(key2);
    EXPECT_EQ(tmppair.first, true);
    ASSERT_EQ(tmppair.second, root.second->pLeft);
    EXPECT_EQ(tmppair.second->color, Color_t::RED);
    EXPECT_EQ(tmppair.second->key, key2);

    // red right
    int key3 = 5;
    tmppair = rbtree.InsertNode(key3);
    EXPECT_EQ(tmppair.first, true);
    ASSERT_EQ(tmppair.second, root.second->pRight);
    EXPECT_EQ(tmppair.second->color, Color_t::RED);
    EXPECT_EQ(tmppair.second->key, key3);
    
    // color flip
    int key4 = 7;
    tmppair = rbtree.InsertNode(key4);
    EXPECT_EQ(tmppair.first, true);
    ASSERT_EQ(tmppair.second, root.second->pRight->pRight);
    EXPECT_EQ(tmppair.second->color, Color_t::RED);
    EXPECT_EQ(tmppair.second->key, key4);
    // check after flip
    EXPECT_EQ(root.second->color, Color_t::BLACK);
    EXPECT_EQ(root.second->key, key1);
    EXPECT_EQ(root.second->pLeft->key, key2);
    EXPECT_EQ(root.second->pLeft->color, Color_t::BLACK);
    EXPECT_EQ(root.second->pRight->key, key3);
    EXPECT_EQ(root.second->pRight->color, Color_t::BLACK);
}

TEST_F(EmptyTree, DeleteNode_NonExistent){
    int very_random_number = 42;
    EXPECT_EQ(rbtree.DeleteNode(very_random_number), false);
}
TEST_F(EmptyTree, FindNode_NonExistent){
    int very_random_number = 42;
    EXPECT_TRUE(rbtree.FindNode(very_random_number) == NULL);
}

// testy nad neprazdnym stromem
TEST_F(NonEmptyTree, InsertNode_Existent){
    std::pair<bool, Node_t *> tmppair;
    for (int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        tmppair = rbtree.InsertNode(inputs[i]);
        EXPECT_FALSE(tmppair.first);
        ASSERT_FALSE(tmppair.second == NULL);
        EXPECT_EQ(tmppair.second->key, inputs[i]);
    }
}

TEST_F(NonEmptyTree, InsertNode_NonExistent){
    std::pair<bool, Node_t *> tmppair = rbtree.InsertNode(not_in_input);
    EXPECT_TRUE(tmppair.first);
    ASSERT_FALSE(tmppair.second == NULL);
    EXPECT_EQ(tmppair.second->key, not_in_input);
}

TEST_F(NonEmptyTree, DeleteNode_Existent){
    //deletes all
    for (int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        bool deleted = rbtree.DeleteNode(inputs[i]);
        EXPECT_TRUE(deleted);
    }
    // should be null if deleted all elems
    EXPECT_TRUE(rbtree.GetRoot() == NULL);
}
TEST_F(NonEmptyTree, DeleteNode_NonExistent){
    // deletes first element to ensure its nonexistency
    ASSERT_TRUE(rbtree.DeleteNode(inputs[0]));
    EXPECT_FALSE(rbtree.DeleteNode(inputs[0]));
}
TEST_F(NonEmptyTree, FindNode_Existent){
    for (int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        Node_t* found = rbtree.FindNode(inputs[i]);
        EXPECT_FALSE(found == NULL);
        EXPECT_EQ(found->key, inputs[i]);
    }
}
TEST_F(NonEmptyTree, FindNode_NonExistent){
    Node_t* found = rbtree.FindNode(not_in_input);
    EXPECT_TRUE(found == NULL);
}

// testy zakladnich axiomu RBT - nezavisle na obsahu stromu
TEST_F(NonEmptyTree, Axiom1){
    // vsechny listy jsou cerne
    std::vector<Node_t *> leafs;
    rbtree.GetLeafNodes(leafs);
    for (int i = 0; i < leafs.size(); i++) {
        EXPECT_EQ(leafs[i]->color, Color_t::BLACK);
    }
}
TEST_F(NonEmptyTree, Axiom2){
    // cerveny uzel ma oba cerne potomky
    std::vector<Node_t *> nonleafs;
    rbtree.GetNonLeafNodes(nonleafs);
    for (int i = 0; i < nonleafs.size(); i++) {
        // detekce cerveneho uzlu
        if (nonleafs[i]->color == Color_t::RED) {
            EXPECT_EQ(nonleafs[i]->pLeft->color, Color_t::BLACK);
            EXPECT_EQ(nonleafs[i]->pRight->color, Color_t::BLACK);
        }
    }
}
TEST_F(NonEmptyTree, Axiom3){
    // kazda cesta od korene k listu ma stejny pocet cernych uzlu
    // prvni cesta nam urci kontrolni hodnotu cernych
    bool first_run = true;
    int pocet_cernych;
    int predesly_pocet_cernych;
    // pujdeme od listu nahoru
    std::vector<Node_t *> leafs;
    rbtree.GetLeafNodes(leafs);

    Node_t* root = rbtree.GetRoot();
    // jestli vubec vedou cesty k rootu
    ASSERT_FALSE(root == NULL);

    for (int i = 0; i < leafs.size(); i++) {
        pocet_cernych = 0;
        // cesta node od listu k rootu
        Node_t* pathNode = leafs[i];
        // projdeme cestu dokud nenarazime na root (nemusi se pocitat, u kazde cesty cerny)
        while (pathNode != root) {
            // on odds go up and down
            if (pathNode->color == Color_t::BLACK) { pocet_cernych++; }
            pathNode = pathNode->pParent;
        }
        // vytvorime kontrolu podle ktere budeme urcovat rovnost
        if (first_run) {
            predesly_pocet_cernych = pocet_cernych;
            first_run = false;
        }

        EXPECT_EQ(predesly_pocet_cernych, pocet_cernych);
    }
}
/*** Konec souboru black_box_tests.cpp ***/
